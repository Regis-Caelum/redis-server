<?php
$host = "127.0.0.1"; // server IP
$port = 3000;        // server port

// Connect to TCP server
$fp = fsockopen($host, $port, $errno, $errstr, 5);
if (!$fp) {
    echo "Error: $errstr ($errno)\n";
    exit(1);
}

// -----------------------
// 1. Test PING
// -----------------------
// $pingCommand = "*1\r\n$4\r\nPING\r\n";
// fwrite($fp, $pingCommand);
// $response = fread($fp, 1024);
// echo "PING response: $response\n";

// -----------------------
// 2. Test SET command
// -----------------------
$key = "mykey";
$value = "hello";

// $setCommand = "*3\r\n$3\r\nSET\r\n$" . strlen($key) . "\r\n$key\r\n$" . strlen($value) . "\r\n$value\r\n";
// fwrite($fp, $setCommand);
// $response = fread($fp, 1024);
// echo "SET response: $response\n";

// -----------------------
// 3. Test GET command
// -----------------------
$getCommand = "*2\r\n$3\r\nGET\r\n$" . strlen($key) . "\r\n$key\r\n";
fwrite($fp, $getCommand);
$response = fread($fp, 1024);
echo "GET response: $response\n";

// -----------------------
// Close connection
// -----------------------
fclose($fp);
?>
