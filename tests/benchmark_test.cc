#include <gtest/gtest.h>
#include <chrono>
#include "../src/Resp/RespParser.h"

TEST(RespParserBenchmark, HugeMessageInMs)
{
    std::ostringstream oss;
    oss << "*1000\r\n";
    for (int i = 1; i <= 1000; ++i)
    {
        oss << ":" << i << "\r\n";
    }
    std::string hugeMsg = oss.str();

    auto start = std::chrono::high_resolution_clock::now();
    auto result = RespParser::parse(hugeMsg);
    auto end = std::chrono::high_resolution_clock::now();

    ASSERT_NE(result, nullptr);

    auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "Parsing time for 1000-element array: " << duration_ms << " ms\n";
}

TEST(RespParserBenchmark, AverageParseTimeMs)
{
    std::ostringstream oss;
    oss << "*1000\r\n";
    for (int i = 1; i <= 1000; ++i)
    {
        oss << ":" << i << "\r\n";
    }
    std::string hugeMsg = oss.str();

    const int runs = 500;
    long long total_ms = 0;

    for (int i = 0; i < runs; ++i)
    {
        auto start = std::chrono::high_resolution_clock::now();
        auto result = RespParser::parse(hugeMsg);
        auto end = std::chrono::high_resolution_clock::now();

        ASSERT_NE(result, nullptr);

        total_ms += std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    }

    std::cout << "Average parse time over " << runs
              << " runs: " << (total_ms / runs) << " ms\n";
}
