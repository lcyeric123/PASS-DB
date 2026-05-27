<?php
$host = "127.0.0.1";
$port = 8888;
$access_pass = "MY_PASS_2026";

// 连接 PASS‑DB
$sock = fsockopen($host, $port);
if (!$sock) {
    echo "数据库连接失败";
    exit;
}

// 发送认证密码
fwrite($sock, $access_pass . "\n");
fgets($sock);

// 接收网页传来的指令
$cmd = $_POST["cmd"];
$key = $_POST["key"];
$value = isset($_POST["value"]) ? $_POST["value"] : "";

// 拼装指令发给数据库
if ($cmd === "set") {
    $send = "set $key $value";
} elseif ($cmd === "get") {
    $send = "get $key";
} elseif ($cmd === "del") {
    $send = "del $key";
}

fwrite($sock, $send);
$res = fgets($sock);
fclose($sock);

echo "执行结果：" . $res;
echo "<br><a href='index.html'>返回页面</a>";
?>
