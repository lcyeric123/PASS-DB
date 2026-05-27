# PASS‑DB
Plain And Simple Storage Database
极简自定义协议加密文件数据库 | Self‑encrypted lightweight file database

## 项目介绍
PASS‑DB 是一个基于文本文件的微型数据库，使用自制异或加密，数据存入 data.txt 时自动加密，读取时解密。
提供 Python 版本和 C++ 版本，通信协议完全一致，可互通使用。

PASS‑DB is a tiny text‑based database with self‑made XOR encryption.
Two implementations: Python and C++, same socket protocol.

## 文件列表
- README.md
- LICENSE
- server.py        Python 服务端
- client.py        Python 客户端
- server.cpp       C++ 服务端
- client.cpp       C++ 客户端
- data.txt         自动生成的加密数据文件

## 通用指令
写入数据 | Save data
'''
set key value  
'''
get key        读取数据 | Read data
del key        删除数据 | Delete data
list           列出全部 | List all data

## 安全设置
连接密码：MY_PASS_2026
加密密钥：0x39
可在代码中自行修改。

## Python 使用方法
无需第三方库，直接运行：
python server.py
python client.py

## C++ 编译运行
Linux / macOS:
g++ server.cpp -o passdb_server -std=c++11
g++ client.cpp -o passdb_client -std=c++11

Windows MinGW:
g++ server.cpp -o passdb_server.exe -std=c++11
g++ client.cpp -o passdb_client.exe -std=c++11
