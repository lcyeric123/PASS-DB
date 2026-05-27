import socket

PASS = "MY_PASS_2026"

def send_command(cmd):
    s = socket.socket()
    s.connect(("127.0.0.1", 8888))
    s.send((PASS + "\n").encode())
    auth = s.recv(1024).decode()
    if "denied" in auth:
        return "Access Denied"
    s.send(cmd.encode())
    return s.recv(1024).decode()

if __name__ == "__main__":
    print(send_command("set username testuser"))
    print(send_command("get username"))
    print(send_command("list"))
    print(send_command("del username"))
