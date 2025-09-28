// The tests here cover a variety of RESP messages, including edge cases and stress tests.
// They ensure that the RespParser correctly interprets valid messages and gracefully handles invalid ones.
// The tests use Google Test framework for assertions and test organization.
// The tests were generated with the help of ChatGPT based on the RespParser implementation.

#include <gtest/gtest.h>
#include "../src/Resp/RespParser.h"

// Helper: assert type and cast
template <typename T>
T as(const RespObject &obj, RespType expected)
{
    EXPECT_EQ(obj.get_type(), expected);
    if (obj.get_type() != expected) {
        throw std::runtime_error("Type mismatch in RespObject");
    }
    return std::get<T>(obj.value);
}

// -----------------------
// Basic parsing
// -----------------------

TEST(RespParserTest, ParsesPing)
{
    std::string_view msg = "*1\r\n$4\r\nPING\r\n";
    auto result = RespParser::parse(msg);
    ASSERT_NE(result, nullptr);

    auto arr = as<std::vector<RespObject>>(*result, RespType::Array);
    ASSERT_EQ(arr.size(), 1);
    EXPECT_EQ(as<std::string>(arr[0], RespType::BulkString), "PING");
}

TEST(RespParserTest, ParsesSetWithSpecialChars)
{
    std::string key = "my$key*1";
    std::string value = "hello!@#";
    std::string msg = "*3\r\n$3\r\nSET\r\n$" + std::to_string(key.size()) + "\r\n" + key +
                      "\r\n$" + std::to_string(value.size()) + "\r\n" + value + "\r\n";

    auto result = RespParser::parse(msg);
    ASSERT_NE(result, nullptr);

    auto arr = as<std::vector<RespObject>>(*result, RespType::Array);
    ASSERT_EQ(arr.size(), 3);

    EXPECT_EQ(as<std::string>(arr[0], RespType::BulkString), "SET");
    EXPECT_EQ(as<std::string>(arr[1], RespType::BulkString), key);
    EXPECT_EQ(as<std::string>(arr[2], RespType::BulkString), value);
}

TEST(RespParserTest, ParsesGetWithSpecialChars)
{
    std::string key = "*weird:key$";
    std::string msg = "*2\r\n$3\r\nGET\r\n$" + std::to_string(key.size()) + "\r\n" + key + "\r\n";

    auto result = RespParser::parse(msg);
    ASSERT_NE(result, nullptr);

    auto arr = as<std::vector<RespObject>>(*result, RespType::Array);
    ASSERT_EQ(arr.size(), 2);
    EXPECT_EQ(as<std::string>(arr[1], RespType::BulkString), key);
}

// -----------------------
// Arrays of integers
// -----------------------

TEST(RespParserTest, ParsesIntegerArray)
{
    std::string msg = "*4\r\n:0\r\n:-42\r\n:2147483647\r\n:-2147483648\r\n";
    auto result = RespParser::parse(msg);
    ASSERT_NE(result, nullptr);

    auto arr = as<std::vector<RespObject>>(*result, RespType::Array);
    ASSERT_EQ(arr.size(), 4);

    EXPECT_EQ(as<long long>(arr[0], RespType::Integer), 0);
    EXPECT_EQ(as<long long>(arr[1], RespType::Integer), -42);
    EXPECT_EQ(as<long long>(arr[2], RespType::Integer), 2147483647);
    EXPECT_EQ(as<long long>(arr[3], RespType::Integer), -2147483648);
}

// -----------------------
// Nested arrays
// -----------------------

TEST(RespParserTest, ParsesNestedArray)
{
    std::string msg = "*2\r\n*2\r\n$1\r\nA\r\n$1\r\nB\r\n*2\r\n$1\r\nC\r\n$1\r\nD\r\n";
    auto result = RespParser::parse(msg);
    ASSERT_NE(result, nullptr);

    auto outer = as<std::vector<RespObject>>(*result, RespType::Array);
    ASSERT_EQ(outer.size(), 2);

    auto first = as<std::vector<RespObject>>(outer[0], RespType::Array);
    EXPECT_EQ(as<std::string>(first[0], RespType::BulkString), "A");
    EXPECT_EQ(as<std::string>(first[1], RespType::BulkString), "B");

    auto second = as<std::vector<RespObject>>(outer[1], RespType::Array);
    EXPECT_EQ(as<std::string>(second[0], RespType::BulkString), "C");
    EXPECT_EQ(as<std::string>(second[1], RespType::BulkString), "D");
}

// -----------------------
// Maps (RESP3)
// -----------------------

TEST(RespParserTest, ParsesMap)
{
    std::string msg = "%2\r\n+first$key\r\n:123\r\n+second:key\r\n$5\r\nvalue\r\n";
    auto result = RespParser::parse(msg);
    ASSERT_NE(result, nullptr);

    auto map = as<std::unordered_map<std::string, RespObject>>(*result, RespType::Map);
    EXPECT_EQ(as<long long>(map.at("first$key"), RespType::Integer), 123);
    EXPECT_EQ(as<std::string>(map.at("second:key"), RespType::BulkString), "value");
}

// -----------------------
// Empty strings
// -----------------------

TEST(RespParserTest, ParsesEmptyStringsAsMonostate)
{
    std::string msg = "*2\r\n$0\r\n\r\n$7\r\n!@#$%^&\r\n";
    auto result = RespParser::parse(msg);
    ASSERT_NE(result, nullptr);

    // Entire message is invalid → parser returns monostate
    EXPECT_TRUE(std::holds_alternative<std::monostate>(result->value));
}

// -----------------------
// Invalid inputs
// -----------------------

TEST(RespParserTest, InvalidInputsReturnMonostate)
{
    std::vector<std::string> invalids = {
        "*1\r\n$8\r\nPING\r\n!\r\n*\r\n",  // length mismatch
        "*2\r\n$3\r\nFOO\r\n",             // missing second element
        "$5\r\nhi\r\n",                     // length mismatch
        ":abc\r\n",                         // invalid integer
    };

    for (const auto &msg : invalids)
    {
        auto result = RespParser::parse(msg);
        ASSERT_NE(result, nullptr);
        EXPECT_TRUE(std::holds_alternative<std::monostate>(result->value));
    }
}

// -----------------------
// Stress / large array
// -----------------------

TEST(RespParserTest, ParsesBigArray)
{
    std::ostringstream oss;
    oss << "*3\r\n$3\r\nSET\r\n$11\r\nbigArrayKey\r\n*1000\r\n";
    for (int i = 1; i <= 1000; ++i)
    {
        oss << ":" << i << "\r\n";
    }

    auto result = RespParser::parse(oss.str());
    ASSERT_NE(result, nullptr);

    auto arr = as<std::vector<RespObject>>(*result, RespType::Array);
    auto bigArray = as<std::vector<RespObject>>(arr[2], RespType::Array);
    EXPECT_EQ(bigArray.size(), 1000);
    EXPECT_EQ(as<long long>(bigArray[0], RespType::Integer), 1);
    EXPECT_EQ(as<long long>(bigArray[999], RespType::Integer), 1000);
}

TEST(RespParserTest, ParsesHugeMemoryHungryMessage)
{
    std::ostringstream oss;

    // Top-level array of 3 elements, each element will itself be huge
    oss << "*3\r\n";

    // -------------------
    // 1st element: a huge map
    // -------------------
    oss << "%1000\r\n";  // map with 1000 key-value pairs
    for (int i = 0; i < 1000; ++i) {
        std::string key = "key" + std::to_string(i);
        oss << "+" << key << "\r\n";
        if (i % 2 == 0) {
            oss << ":" << i << "\r\n";                  // integer value
        } else {
            std::string val = "val" + std::to_string(i);
            oss << "$" << val.size() << "\r\n" << val << "\r\n";  // bulk string
        }
    }

    // -------------------
    // 2nd element: huge array of numbers
    // -------------------
    oss << "*10000\r\n";  // array of 10000 numbers
    for (int i = 1; i <= 10000; ++i) {
        if (i % 3 == 0) {
            oss << "," << (i + 0.123) << "\r\n";      // decimal
        } else {
            oss << ":" << i << "\r\n";               // integer
        }
    }

    // -------------------
    // 3rd element: deeply nested arrays and maps
    // -------------------
    oss << "*100\r\n";  // 100 nested elements
    for (int i = 0; i < 100; ++i) {
        // Each element is an array of 10 maps
        oss << "*10\r\n";
        for (int j = 0; j < 10; ++j) {
            oss << "%5\r\n";  // map with 5 keys
            for (int k = 0; k < 5; ++k) {
                std::string key = "n" + std::to_string(i) + "_" + std::to_string(j) + "_" + std::to_string(k);
                oss << "+" << key << "\r\n";
                if (k % 2 == 0) {
                    oss << ":" << k << "\r\n";
                } else {
                    std::string val = "v" + std::to_string(k);
                    oss << "$" << val.size() << "\r\n" << val << "\r\n";
                }
            }
        }
    }

    std::string hugeMsg = oss.str();
    auto result = RespParser::parse(hugeMsg);
    ASSERT_NE(result, nullptr);

    // Verify top-level array
    auto topArray = as<std::vector<RespObject>>(*result, RespType::Array);
    ASSERT_EQ(topArray.size(), 3);

    // Verify first map
    auto hugeMap = as<std::unordered_map<std::string, RespObject>>(topArray[0], RespType::Map);
    EXPECT_EQ(as<long long>(hugeMap.at("key0"), RespType::Integer), 0);
    EXPECT_EQ(as<std::string>(hugeMap.at("key1"), RespType::BulkString), "val1");

    // Verify second array
    auto hugeNumbers = as<std::vector<RespObject>>(topArray[1], RespType::Array);
    EXPECT_EQ(as<long long>(hugeNumbers[0], RespType::Integer), 1);
    EXPECT_DOUBLE_EQ(as<long double>(hugeNumbers[2], RespType::Decimal), 3.123);

    // Verify third deeply nested array
    auto nestedArrays = as<std::vector<RespObject>>(topArray[2], RespType::Array);
    ASSERT_EQ(nestedArrays.size(), 100);
    auto firstNested = as<std::vector<RespObject>>(nestedArrays[0], RespType::Array);
    ASSERT_EQ(firstNested.size(), 10);
    auto firstMap = as<std::unordered_map<std::string, RespObject>>(firstNested[0], RespType::Map);
    EXPECT_EQ(as<long long>(firstMap.at("n0_0_0"), RespType::Integer), 0);
}
