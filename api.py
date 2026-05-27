from flask import Flask, request
import socket

app = Flask(__name__)

DB_HOST = "127.0.0.1"
DB_PORT = 8888
ACCESS_PASS = "MY_PASS_2026"

def call_db(cmd):
    s = socket.socket()
    s.connect((DB_HOST, DB_PORT))
    s.send((ACCESS_PASS + "\n").encode())
    s.recv(1024)
    s.send(cmd.encode())
    res = s.recv(1024).decode()
    s.close()
    return res

@app.route("/api", methods=["POST"])
def api():
    data = request.json
    cmd_type = data.get("cmd")
    key = data.get("key")
    val = data.get("value", "")

    if cmd_type == "set":
        return call_db(f"set {key} {val}")
    elif cmd_type == "get":
        return call_db(f"get {key}")
    elif cmd_type == "del":
        return call_db(f"del {key}")
    return "error"

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000)
