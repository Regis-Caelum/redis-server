<?php
$host = "127.0.0.1";
$port = 3000;

// Connect to TCP server
$fp = fsockopen($host, $port, $errno, $errstr, 5);
if (!$fp) {
    echo "Error: $errstr ($errno)\n";
    exit(1);
}

// Helper function
function sendCommand($fp, $command, $label) {
    fwrite($fp, $command);
    $response = fread($fp, 1024);
    echo "$label response: $response\n";
}

// -----------------------
// Test cases
// -----------------------
function test1_Ping($fp) {
    sendCommand($fp, "*1\r\n$4\r\nPING\r\n", "Test 1: PING");
}

function test2_Set($fp) {
    $key = "mykey";
    $value = "hello";
    $cmd = "*3\r\n$3\r\nSET\r\n$" . strlen($key) . "\r\n$key\r\n$" . strlen($value) . "\r\n$value\r\n";
    sendCommand($fp, $cmd, "Test 2: SET");
}

function test3_Get($fp) {
    $key = "mykey";
    $cmd = "*2\r\n$3\r\nGET\r\n$" . strlen($key) . "\r\n$key\r\n";
    sendCommand($fp, $cmd, "Test 3: GET");
}

function test4_Invalid($fp) {
    $invalidCommands = [
        "*0\r\n",                      
        "*2\r\n$3\r\nFOO\r\n",         
        "$5\r\nhello\r\n",              
        ":abc\r\n",                     
        "*2\r\n$3\r\nSET\r\n$3\r\nkey\r\n"
    ];
    foreach ($invalidCommands as $i => $cmd) {
        sendCommand($fp, $cmd, "Test 4: Invalid command " . ($i + 1));
    }
}

function test5_SetIntegerArray($fp) {
    $key = "intArrayKey";
    $values = [1, 2, 3, 4];
    $cmd = "*3\r\n$3\r\nSET\r\n$" . strlen($key) . "\r\n$key\r\n*" . count($values) . "\r\n";
    foreach ($values as $val) {
        $cmd .= ":" . $val . "\r\n";
    }
    sendCommand($fp, $cmd, "Test 5: SET integer array");
}

function test7_GetIntegerArray($fp) {
    $key = "intArrayKey";
    $cmd = "*2\r\n$3\r\nGET\r\n$" . strlen($key) . "\r\n$key\r\n";
    sendCommand($fp, $cmd, "Test 7: GET integer array");
}

// -----------------------
// Extended parser validation tests
// -----------------------
function test8_EmptyString($fp) {
    $cmd = "*2\r\n$3\r\nSET\r\n$0\r\n\r\n$0\r\n\r\n";
    sendCommand($fp, $cmd, "Test 8: Empty string key/value");
}

function test9_IntegerEdgeCases($fp) {
    $key = "intKey";
    $values = [0, -1, 2147483647, -2147483648];
    $cmd = "*3\r\n$3\r\nSET\r\n$" . strlen($key) . "\r\n$key\r\n*" . count($values) . "\r\n";
    foreach ($values as $val) {
        $cmd .= ":" . $val . "\r\n";
    }
    sendCommand($fp, $cmd, "Test 9: Integer edge cases");
}

function test10_NestedArray($fp) {
    $key = "nestedArrayKey";
    $cmd = "*3\r\n$3\r\nSET\r\n$" . strlen($key) . "\r\n$key\r\n*2\r\n";
    $cmd .= "*2\r\n$1\r\nA\r\n$1\r\nB\r\n";
    $cmd .= "*2\r\n$1\r\nC\r\n$1\r\nD\r\n";
    sendCommand($fp, $cmd, "Test 10: Nested array");
}

// -----------------------
// New decimal tests
// -----------------------
function test11_SetDecimal($fp) {
    $key = "decimalKey";
    $values = [0.1, -0.5, 12345.6789, -9876.54321];
    $cmd = "*3\r\n$3\r\nSET\r\n$" . strlen($key) . "\r\n$key\r\n*" . count($values) . "\r\n";
    foreach ($values as $val) {
        $cmd .= "," . $val . "\r\n";
    }
    sendCommand($fp, $cmd, "Test 11: SET decimal values");
}

function test12_GetDecimal($fp) {
    $key = "decimalKey";
    $cmd = "*2\r\n$3\r\nGET\r\n$" . strlen($key) . "\r\n$key\r\n";
    sendCommand($fp, $cmd, "Test 12: GET decimal values");
}

function test13_NestedArrayMixedTypes($fp) {
    $key = "mixedNestedKey";

    // Outer array: ["SET", "mixedNestedKey", [...nested array...]]
    $cmd = "*3\r\n$3\r\nSET\r\n$" . strlen($key) . "\r\n$key\r\n*3\r\n";

    // Inner array 1: [string, integer]
    $cmd .= "*2\r\n$5\r\nHello\r\n:42\r\n";

    // Inner array 2: [decimal, string]
    $cmd .= "*2\r\n,3.14\r\n$5\r\nWorld\r\n";

    // Inner array 3: [integer, decimal, string]
    $cmd .= "*3\r\n:7\r\n,-0.5\r\n$4\r\nTest\r\n";

    sendCommand($fp, $cmd, "Test 13: Nested array with mixed types");
}

function test14_GetNestedArrayMixedTypes($fp) {
    $key = "mixedNestedKey";
    $cmd = "*2\r\n$3\r\nGET\r\n$" . strlen($key) . "\r\n$key\r\n";
    sendCommand($fp, $cmd, "Test 14: GET nested array with mixed types");
}

function test15_SimpleMap($fp) {
    $key = "simpleMapKey";

    // Outer array: ["SET", "simpleMapKey", map]
    $cmd = "*3\r\n$3\r\nSET\r\n$" . strlen($key) . "\r\n$key\r\n%2\r\n";  // Map with 2 key-value pairs

    // Map entries
    $cmd .= "+first\r\n:1\r\n";
    $cmd .= "+second\r\n:2\r\n";

    sendCommand($fp, $cmd, "Test 15: SET simple map");
}

function test16_GetSimpleMap($fp) {
    $key = "simpleMapKey";
    $cmd = "*2\r\n$3\r\nGET\r\n$" . strlen($key) . "\r\n$key\r\n";
    sendCommand($fp, $cmd, "Test 16: GET simple map");
}

function test17_ComplexNestedStructure($fp) {
    $key = "complexKey";

    // Outer array: ["SET", "complexKey", [...]]
    $cmd = "*3\r\n$3\r\nSET\r\n$" . strlen($key) . "\r\n$key\r\n*4\r\n";

    // Inner array 1: [string, integer, decimal]
    $cmd .= "*3\r\n$5\r\nHello\r\n:100\r\n,3.1415\r\n";

    // Inner map 1: {"a": 1, "b": ["X", 2, ,0.5]}
    $cmd .= "%2\r\n+first\r\n:1\r\n+second\r\n*3\r\n$1\r\nX\r\n:2\r\n,0.5\r\n";

    // Inner array 2: [nested array, empty string, negative decimal]
    $cmd .= "*3\r\n*2\r\n:7\r\n$3\r\nEnd\r\n$0\r\n\r\n,-42.42\r\n";

    // Inner map 2: {"mapKey": {"nested": [1,2,3]}, "empty": ""}
    $cmd .= "%2\r\n+mapKey\r\n*3\r\n:1\r\n:2\r\n:3\r\n+empty\r\n$1\r\nM\r\n";

    sendCommand($fp, $cmd, "Test 17: Ultra-complex nested structure");
}

function test18_GetComplexNestedStructure($fp) {
    $key = "complexKey";
    $cmd = "*2\r\n$3\r\nGET\r\n$" . strlen($key) . "\r\n$key\r\n";
    sendCommand($fp, $cmd, "Test 18: GET ultra-complex nested structure");
}

function test19_DeepNestedArrays($fp)
{
    $key = "deepNestedKey";

    // Outer array: ["SET", "deepNestedKey", deeply nested arrays 10 levels deep]
    $cmd = "*3\r\n$3\r\nSET\r\n$" . strlen($key) . "\r\n$key\r\n";

    // Build a 10-level nested array like [[[[...["deep", 999]...]]]]
    $depth = 10;
    for ($i = 0; $i < $depth; $i++) {
        $cmd .= "*1\r\n";
    }
    $cmd .= "*2\r\n$4\r\ndeep\r\n:999\r\n"; // innermost
    for ($i = 0; $i < $depth; $i++) {
        $cmd .= "\r\n"; // close each level
    }

    sendCommand($fp, $cmd, "Test 19: Deeply nested arrays (10 levels)");
}

function test20_BigArray1000($fp)
{
    $key = "bigArrayKey";

    // Outer array: ["SET", "bigArrayKey", array of 1000 integers]
    $cmd = "*3\r\n$3\r\nSET\r\n$" . strlen($key) . "\r\n$key\r\n*1000\r\n";
    for ($i = 1; $i <= 1000; $i++) {
        $cmd .= ":" . $i . "\r\n";
    }

    sendCommand($fp, $cmd, "Test 20: Big array with 1000 integers");
}

function test21_BigMap500($fp)
{
    $key = "bigMapKey";

    // Outer array: ["SET", "bigMapKey", map with 500 entries]
    $cmd = "*3\r\n$3\r\nSET\r\n$" . strlen($key) . "\r\n$key\r\n%" . 500 . "\r\n";
    for ($i = 1; $i <= 500; $i++) {
        $field = "field" . $i;
        $cmd .= "+" . $field . "\r\n:" . $i . "\r\n";
    }

    sendCommand($fp, $cmd, "Test 21: Big map with 500 entries");
}

function test22_MixedMonster($fp)
{
    $key = "monsterKey";

    // Outer array: ["SET", "monsterKey", array with mixed large content]
    $cmd = "*3\r\n$3\r\nSET\r\n$" . strlen($key) . "\r\n$key\r\n*5\r\n";

    // 1. Large string
    $bigString = str_repeat("A", 10000);
    $cmd .= "$" . strlen($bigString) . "\r\n$bigString\r\n";

    // 2. Big integer
    $cmd .= ":999999999\r\n";

    // 3. Decimal
    $cmd .= ",123456.789\r\n";

    // 4. Nested array with 100 elements
    $cmd .= "*" . 100 . "\r\n";
    for ($i = 0; $i < 100; $i++) {
        $cmd .= ":" . $i . "\r\n";
    }

    // 5. Map with 10 keys
    $cmd .= "%10\r\n";
    for ($i = 1; $i <= 10; $i++) {
        $cmd .= "+k$i\r\n:$i\r\n";
    }

    sendCommand($fp, $cmd, "Test 22: Mixed monster structure");
}

// -----------------------
function test6_Quit($fp) {
    sendCommand($fp, "*1\r\n$4\r\nQUIT\r\n", "Test 6: QUIT");
}

// -----------------------
// Pick which tests to run
// -----------------------
$testsToRun = [
    1,
    2,
    3,
    4,
    5,
    7,
    8,
    9,
    10,
    11,
    12,
    13,
    14,
    15,
    16,
    17,
    18,
    19,
    20,
    21,
    22
];

// Map numbers to function names
$testFunctions = [
    1 => 'test1_Ping',
    2 => 'test2_Set',
    3 => 'test3_Get',
    4 => 'test4_Invalid',
    5 => 'test5_SetIntegerArray',
    6 => 'test6_Quit',
    7 => 'test7_GetIntegerArray',
    8 => 'test8_EmptyString',
    9 => 'test9_IntegerEdgeCases',
    10 => 'test10_NestedArray',
    11 => 'test11_SetDecimal',
    12 => 'test12_GetDecimal',
    13 => 'test13_NestedArrayMixedTypes',
    14 => 'test14_GetNestedArrayMixedTypes',
    15 => 'test15_SimpleMap',
    16 => 'test16_GetSimpleMap',
    17 => 'test17_ComplexNestedStructure',
    18 => 'test18_GetComplexNestedStructure',
    19 => 'test19_DeepNestedArrays',
    20 => 'test20_BigArray1000',
    21 => 'test21_BigMap500',
    22 => 'test22_MixedMonster',
];

// Execute selected tests
foreach ($testsToRun as $num) {
    if (isset($testFunctions[$num]) && is_callable($testFunctions[$num])) {
        $func = $testFunctions[$num];
        $func($fp);
    } else {
        echo "Error: Test $num not found\n";
    }
}

// Always quit at the end
test6_Quit($fp);

fclose($fp);
