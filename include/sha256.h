#ifndef SHA256_H
#define SHA256_H

#include <cstdint>
#include <vector>
#include <string>

//Structure to hold the state of the SHA-256 hash computation.
struct SHA256State{
    uint32_t result[8];        // Holds the current hash state
    uint8_t message[64];       // Buffer to store the current message block
    size_t messageSize = 0;    // Size of the current message block
    size_t bitLength = 0;      // Total length of the message in bits
};

// Function prototypes for SHA-256 hashing
void sha256_update(SHA256State& state, const std::vector<uint8_t>& data);
void padding(SHA256State& state);
std::vector<uint8_t> sha256_finalize(SHA256State& state);
std::vector<uint8_t> sha256_compute(const std::vector<uint8_t>& input);
void transform(SHA256State& state);

#endif // SHA256_H