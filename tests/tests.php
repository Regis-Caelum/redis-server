<?php
$host = "127.0.0.1"; // server IP
$port = 3000;        // server port

// Connect to TCP server
$fp = fsockopen($host, $port, $errno, $errstr, 5);
if (!$fp) {
    echo "Error: $errstr ($errno)\n";
    exit(1);
}

// Helper function to send a command and print response
function sendCommand($fp, $command, $label) {
    fwrite($fp, $command);
    $response = fread($fp, 1024);
    echo "$label response: $response\n";
}

// -----------------------
// 1. Test PING
// -----------------------
$pingCommand = "*1\r\n$4\r\nPING\r\n";
sendCommand($fp, $pingCommand, "PING");

// -----------------------
// 2. Test SET command (simple string)
// -----------------------
$key = "mykey";
$value = "hello";
$setCommand = "*3\r\n$3\r\nSET\r\n$" . strlen($key) . "\r\n$key\r\n$" . strlen($value) . "\r\n$value\r\n";
sendCommand($fp, $setCommand, "SET");

// -----------------------
// 3. Test GET command
// -----------------------
$getCommand = "*2\r\n$3\r\nGET\r\n$" . strlen($key) . "\r\n$key\r\n";
sendCommand($fp, $getCommand, "GET");

// -----------------------
// 4. Test invalid commands
// -----------------------
$invalidCommands = [
    "*0\r\n",                               // Empty array
    "*2\r\n$3\r\nFOO\r\n",                  // Missing argument
    "$5\r\nhello\r\n",                       // Bulk string without array
    ":abc\r\n",                              // Invalid integer
    "*2\r\n$3\r\nSET\r\n$3\r\nkey\r\n"       // SET missing value
];

foreach ($invalidCommands as $i => $cmd) {
    sendCommand($fp, $cmd, "Invalid test " . ($i + 1));
}

// -----------------------
// 5. Test SET key with integer array
// -----------------------
$key = "intArrayKey";
$values = [1, 2, 3, 4];

// Construct RESP array: ["SET", "intArrayKey", [1,2,3,4]]
$setArrayCommand = "*3\r\n";
$setArrayCommand .= "$3\r\nSET\r\n";
$setArrayCommand .= "$" . strlen($key) . "\r\n$key\r\n";

// Build inner array for integers
$setArrayCommand .= "*" . count($values) . "\r\n";
foreach ($values as $val) {
    $setArrayCommand .= ":" . $val . "\r\n";
}

sendCommand($fp, $setArrayCommand, "SET integer array");

// -----------------------
// 6. Test QUIT command (at the very end)
// -----------------------
$quitCommand = "*1\r\n$4\r\nQUIT\r\n";
sendCommand($fp, $quitCommand, "QUIT");

// -----------------------
// Close connection (server should also close after QUIT)
// -----------------------
fclose($fp);