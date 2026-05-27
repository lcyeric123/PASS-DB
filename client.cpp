#include <iostream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8888
const std::string ACCESS_PASS = "MY_PASS_2026";

std::string send_cmd(const std::string &cmd) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
    connect(sock, (sockaddr *)&addr, sizeof(addr));

    send(sock, ACCESS_PASS.c_str(), ACCESS_PASS.size(), 0);
    char auth[1024]{};
    recv(sock, auth, 1024, 0);
    if (std::string(auth).find("denied") != std::string::npos) {
        close(sock); return "Access Denied";
    }

    send(sock, cmd.c_str(), cmd.size(), 0);
    char res[1024]{};
    recv(sock, res, 1024, 0);
    close(sock);
    return std::string(res);
}

int main() {
    std::cout << send_cmd("set username testuser") << "\n";
    std::cout << send_cmd("get username") << "\n";
    std::cout << send_cmd("list") << "\n";
    std::cout << send_cmd("del username") << "\n";
    return 0;
}
