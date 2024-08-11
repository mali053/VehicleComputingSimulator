#ifndef SHA256_H
#define SHA256_H

#include <cstdint>
#include <string>
#include <vector>

class Sha256
{
private:
    uint32_t state[8];          // Internal state of the SHA-256 hash function
    uint32_t result[8];         // Intermediate hash values
    uint8_t message[64];        // Message block (64 bytes)
    size_t messageSize;         // Size of the current message block
    size_t bitLength;           // Total length of the input message in bits

    void padding();             // Applies padding to the message
    void transform();           // Processes a message block to update hash values

public:
    // Constructor: Initializes the SHA-256 state and message block
    Sha256();

    // Updates the hash with additional data
    void update(const std::vector<uint8_t>& data);

    // Finalizes the hash computation and returns the hash value
    std::vector<uint8_t> finalize();

    // Returns the current hash value
    std::vector<uint8_t> getHash();

    // Computes the SHA-256 hash of the given input
    std::vector<uint8_t> computeSHA256(const std::vector<uint8_t>& input);
};

#endif // SHA256_H