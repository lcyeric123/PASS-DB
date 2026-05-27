import socket

DB_FILE = "data.txt"
ACCESS_PASS = "MY_PASS_2026"
SECRET_KEY = 0x39

def encrypt(text: str) -> str:
    return ''.join(chr(ord(c) ^ SECRET_KEY) for c in text)

def decrypt(text: str) -> str:
    return encrypt(text)

def load_db():
    db = {}
    try:
        with open(DB_FILE, "r", encoding="utf-8") as f:
            for line in f:
                line = line.strip()
                if "=" in line:
                    k, v_enc = line.split("=", 1)
                    db[k] = decrypt(v_enc)
    except FileNotFoundError:
        pass
    return db

def save_db(db):
    with open(DB_FILE, "w", encoding="utf-8") as f:
        for k, v in db.items():
            f.write(f"{k}={encrypt(v)}\n")

def handle(cmd: str) -> str:
    parts = cmd.strip().split(maxsplit=2)
    if not parts:
        return "error empty"
    op = parts[0]
    db = load_db()

    if op == "set":
        if len(parts) < 3:
            return "error args"
        k, v = parts[1], parts[2]
        db[k] = v
        save_db(db)
        return "ok"
    elif op == "get":
        if len(parts) < 2:
            return "error args"
        k = parts[1]
        return db.get(k, "null")
    elif op == "del":
        if len(parts) < 2:
            return "error args"
        k = parts[1]
        if k in db:
            del db[k]
            save_db(db)
            return "ok"
        return "not found"
    elif op == "list":
        return "\n".join(f"{k}={v}" for k, v in db.items())
    return "unknown cmd"

def main():
    sock = socket.socket()
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    sock.bind(("127.0.0.1", 8888))
    sock.listen(1)
    print("PASS‑DB Python Server running on 127.0.0.1:8888")

    while True:
        conn, _ = sock.accept()
        auth = conn.recv(1024).decode().strip()
        if auth != ACCESS_PASS:
            conn.send(b"denied\n")
            conn.close()
            continue
        conn.send(b"auth ok\n")

        while True:
            data = conn.recv(1024).decode()
            if not data:
                break
            res = handle(data)
            conn.send((res + "\n").encode())
        conn.close()

if __name__ == "__main__":
    main()
