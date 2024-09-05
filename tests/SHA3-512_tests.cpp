#include "gtest/gtest.h"
#include <vector>
#include "../include/SHA3-512.h"
#include <iomanip>

// General function to update and finalize the SHA-256 hash
void sha3_512_test(const std::vector<std::vector<uint8_t>>& chunks, const std::string& expectedHash) 
{
    SHA3_512 state;
    
    // Loop to update the hash with chunks of data
    for (const auto& chunk : chunks)
        state.update(chunk.data(), chunk.size());

    // Compute the final hash
    std::string hash;
    state.finalize(hash);

    // Compare the resulting hash with the expected hash
    ASSERT_EQ(hash, expectedHash);
}

// Test for hash of an empty string
TEST(SHA3_512Test, EmptyStringHash)
{
    std::vector<std::vector<uint8_t>> data = {{}};
    std::string expectedHash = "a69f73cca23a9ac5c8b567dc185a756e97c98216"
                                "4fe25859e0d1dcc1475c80a615b2123af1f5f94"
                                "c11e3e9402c3ac558f500199d95b6d3e3017585"
                                "86281dcd26";
    sha3_512_test(data, expectedHash);
}

// Test for hash of the string "abc"
TEST(SHA3_512Test, ABCStringHash)
{
    std::vector<std::vector<uint8_t>> data = {{'a', 'b', 'c'}};
    std::string expectedHash = "b751850b1a57168a5693cd924b6b096e08f621827"
                                "444f70d884f5d0240d2712e10e116e9192af3c91"
                                "a7ec57647e3934057340b4cf408d5a56592f8274"
                                "eec53f0";
    sha3_512_test(data, expectedHash);
}

// Test for hash of a string longer than 64 bytes
TEST(SHA3_512Test, StringLongerThan64BytesHash)
{
    std::string longString = "The quick brown fox jumps over the lazy dog";
    std::vector<std::vector<uint8_t>> data = {std::vector<uint8_t>(longString.begin(), longString.end())};
    std::string expectedHash = "01dedd5de4ef14642445ba5f5b97c15e47b9ad931"
                                "326e4b0727cd94cefc44fff23f07bf543139939b"
                                "49128caf436dc1bdee54fcb24023a08d9403f9b4"
                                "bf0d450";
    sha3_512_test(data, expectedHash);
}

// Test for hash of a string containing special characters
TEST(SHA3_512Test, SpecialCharsHash)
{
    std::string specialString = "!@#$%^&*()";
    std::vector<std::vector<uint8_t>> data = {std::vector<uint8_t>(specialString.begin(), specialString.end())};
    std::string expectedHash = "fbbcb3e21184dd4061de0b85c4756f74e36a36112"
                                "5733e2c7470232fc66f71c902d1e6ff7eb60cfe6"
                                "b47e8b72e1429b4ff21de0fa150a2b3e8d8e29e2"
                                "64d56ab";
    sha3_512_test(data, expectedHash);
}

// Test for multiple updates
TEST(SHA3_512Test, MultipleUpdatesHash)
{
    std::vector<std::vector<uint8_t>> data = {
        {'a', 'b'},  // First update with two bytes
        {'c', 'd'}  // Second update with two more bytes
    };
    std::string expectedHash = "6eb7b86765bf96a8467b72401231539cbb830f6c641"
                                "20954c4567272f613f1364d6a80084234fa3400d30"
                                "6b9f5e10c341bbdc5894d9b484a8c7deea9cbe4e265";
    sha3_512_test(data, expectedHash);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}