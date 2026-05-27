#include <iostream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define HTTP_PORT 5000
#define DB_HOST "127.0.0.1"
#define DB_PORT 8888
#define ACCESS_PASS "MY_PASS_2026"

std::string db_call(const std::string& cmd) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(DB_PORT);
    inet_pton(AF_INET, DB_HOST, &addr.sin_addr);
    connect(sock, (sockaddr*)&addr, sizeof(addr));

    send(sock, ACCESS_PASS.c_str(), ACCESS_PASS.size(), 0);
    char buf[1024];
    recv(sock, buf, 1024, 0);

    send(sock, cmd.c_str(), cmd.size(), 0);
    recv(sock, buf, 1024, 0);
    close(sock);
    return std::string(buf);
}

void handle_http(int client) {
    char buf[4096];
    recv(client, buf, 4096, 0);
    std::string req(buf);

    std::string cmd, key, val;
    size_t kpos = req.find("\"key\":\"");
    if (kpos != std::string::npos) {
        size_t e = req.find("\"", kpos + 7);
        key = req.substr(kpos + 7, e - kpos - 7);
    }
    size_t vpos = req.find("\"value\":\"");
    if (vpos != std::string::npos) {
        size_t e = req.find("\"", vpos + 9);
        val = req.substr(vpos + 9, e - vpos - 9);
    }
    if (req.find("set") != std::string::npos) cmd = "set";
    else if (req.find("get") != std::string::npos) cmd = "get";
    else if (req.find("del") != std::string::npos) cmd = "del";

    std::string full;
    if (cmd == "set") full = "set " + key + " " + val;
    else if (cmd == "get") full = "get " + key;
    else if (cmd == "del") full = "del " + key;

    std::string res = db_call(full);
    std::string http = "HTTP/1.1 200 OK\r\nContent‑Type: text/plain\r\nContent‑Length: " + std::to_string(res.size()) + "\r\n\r\n" + res;
    send(client, http.c_str(), http.size(), 0);
    close(client);
}

int main() {
    int s = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(HTTP_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;
    bind(s, (sockaddr*)&addr, sizeof(addr));
    listen(s, 5);
    std::cout << "HTTP API running on :5000\n";
    while (1) handle_http(accept(s, nullptr, nullptr));
    return 0;
}
