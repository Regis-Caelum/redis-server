<?php
$host = "127.0.0.1"; // server IP
$port = 3000;        // server port

// Connect to TCP server
$fp = fsockopen($host, $port, $errno, $errstr, 5);
if (!$fp) {
    echo "Error: $errstr ($errno)\n";
    exit(1);
}

// Prepare RESP PING command
// RESP for: PING
// *1\r\n$4\r\nPING\r\n
$pingCommand = "*1\r\n$4\r\nPING\r\n";

// Send command
fwrite($fp, $pingCommand);

// Read server response
$response = fread($fp, 1024);

echo "Server response: $response\n";

// Close connection
fclose($fp);
?>
