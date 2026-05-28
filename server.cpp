#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <utility>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

#define PORT 8888
#define DB_FILE "data.txt"
const std::string ACCESS_PASS = "MY_PASS_2026";
const int SECRET_KEY = 0x39;

std::string encrypt_decrypt(std::string text) {
    for (char &c : text) c ^= SECRET_KEY;
    return text;
}

std::vector<std::pair<std::string, std::string>> load_db() {
    std::vector<std::pair<std::string, std::string>> db;
    std::ifstream file(DB_FILE);
    if (!file.is_open()) return db;
    std::string line;
    while (std::getline(file, line)) {
        size_t pos = line.find('=');
        if (pos == std::string::npos) continue;
        std::string k = line.substr(0, pos);
        std::string v_enc = line.substr(pos + 1);
        db.emplace_back(k, encrypt_decrypt(v_enc));
    }
    file.close();
    return db;
}

void save_db(const std::vector<std::pair<std::string, std::string>> &db) {
    std::ofstream file(DB_FILE);
    for (auto &p : db) {
        std::string v_enc = encrypt_decrypt(p.second);
        file << p.first << "=" << v_enc << "\n";
    }
    file.close();
}

std::string handle_command(const std::string &cmd) {
    auto db = load_db();
    size_t pos1 = cmd.find(' ');
    if (pos1 == std::string::npos) return "error empty\n";
    std::string op = cmd.substr(0, pos1);

    if (op == "set") {
        size_t pos2 = cmd.find(' ', pos1 + 1);
        if (pos2 == std::string::npos) return "error args\n";
        std::string k = cmd.substr(pos1 + 1, pos2 - pos1 - 1);
        std::string v = cmd.substr(pos2 + 1);
        bool found = false;
        for (auto &p : db) {
            if (p.first == k) { p.second = v; found = true; break; }
        }
        if (!found) db.emplace_back(k, v);
        save_db(db);
        return "ok\n";
    } else if (op == "get") {
        std::string k = cmd.substr(pos1 + 1);
        for (auto &p : db) if (p.first == k) return p.second + "\n";
        return "null\n";
    } else if (op == "del") {
        std::string k = cmd.substr(pos1 + 1);
        for (size_t i = 0; i < db.size(); i++) {
            if (db[i].first == k) { db.erase(db.begin() + i); save_db(db); return "ok\n"; }
        }
        return "not found\n";
    } else if (op == "list") {
        std::string res;
        for (auto &p : db) res += p.first + "=" + p.second + "\n";
        res += "\n";
        return res;
    }
    return "unknown cmd\n";
}

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket failed");
        return 1;
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind failed");
        close(server_fd);
        return 1;
    }

    if (listen(server_fd, 1) < 0) {
        perror("listen failed");
        close(server_fd);
        return 1;
    }

    std::cout << "PASS‑DB running on 127.0.0.1:" << PORT << "\n";

    while (true) {
        int conn = accept(server_fd, nullptr, nullptr);
        if (conn < 0) continue;

        char buf[1024]{};
        ssize_t n = recv(conn, buf, sizeof(buf)-1, 0);
        if (n <= 0) { close(conn); continue; }

        std::string authStr(buf);
        if (authStr.find(ACCESS_PASS) == std::string::npos) {
            send(conn, "denied\n", 7, 0);
            close(conn);
            continue;
        }

        send(conn, "auth ok\n", 8, 0);
        memset(buf, 0, sizeof(buf));
        n = recv(conn, buf, sizeof(buf)-1, 0);
        if (n > 0) {
            std::string result = handle_command(std::string(buf));
            send(conn, result.c_str(), result.size(), 0);
        }
        close(conn);
    }

    close(server_fd);
    return 0;
}
