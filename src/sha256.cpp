#include "../include/sha256.h"
#include <cstring>

using namespace std;

// Constants used in SHA-256 processing
#define CHOOSE(x, y, z) (((x) & (y)) ^ (~(x) & (z)))
#define MAJORITY(x, y, z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define ROTRIGHT(a, b) (((a) >> (b)) | ((a) << (32 - (b))))
#define SIGMA0(x) (ROTRIGHT(x, 2) ^ ROTRIGHT(x, 13) ^ ROTRIGHT(x, 22))
#define SIGMA1(x) (ROTRIGHT(x, 6) ^ ROTRIGHT(x, 11) ^ ROTRIGHT(x, 25))
#define GAMMA0(x) (ROTRIGHT(x, 7) ^ ROTRIGHT(x, 18) ^ ((x) >> 3))
#define GAMMA1(x) (ROTRIGHT(x, 17) ^ ROTRIGHT(x, 19) ^ ((x) >> 10))

const uint32_t bytes[64] = {
    // SHA-256 constants
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
    0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
    0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
    0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
    0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
    0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

// Global variables used in SHA-256 computation
static uint32_t result[8];
static uint8_t message[64];
static size_t messageSize;
static size_t bitLength;

/**
 * Updates the SHA-256 hash with the provided data.
 * Processes data in 64-byte chunks, calling `transform` for each chunk.
 * 
 * @param data A vector of bytes to be added to the hash.
 */
void sha256_update(const std::vector<uint8_t>& data)
{
    size_t length = data.size();
    for (size_t i = 0; i < length; i++) {
        message[messageSize++] = static_cast<uint8_t>(data[i]);
        if (messageSize == 64) {
            // Transform function is called for each 64-byte chunk
            transform();
            messageSize = 0;
            bitLength += 512;
        }
    }
}

/**
 * Adds padding to the message and appends the length of the original message.
 * The message is padded so its length is congruent to 56 modulo 64.
 */
void padding()
{
    uint64_t currentLength = messageSize;
    uint8_t paddingEnd = currentLength < 56 ? 56 : 64;

    // Append the 0x80 byte (0b10000000)
    message[currentLength++] = 0x80;

    // Pad with 0x00 bytes until the length of the message is 56 bytes mod 64
    memset(message + currentLength, 0, paddingEnd - currentLength);
    currentLength = paddingEnd;

    // If message length is >= 56, process the current block and reset for new padding
    if (messageSize >= 56) {
        transform();
        memset(message, 0, 56);
        currentLength = 56;
    }

    // Append the length of the original message in bits (64-bit big-endian)
    bitLength += messageSize * 8;
    for (int i = 0; i < 8; i++) {
        message[63 - i] = bitLength >> (i * 8);
    }

    transform();
}

/**
 * Processes a 64-byte block of the message and updates the hash state.
 * Applies the SHA-256 compression function to the current block.
 */
void transform()
{
    uint32_t temp[64];
    for (uint8_t i = 0, j = 0; i < 16; i++, j += 4)
        temp[i] = (message[j] << 24) | (message[j + 1] << 16) | (message[j + 2] << 8) | (message[j + 3]);

    for (uint8_t k = 16; k < 64; k++)
        temp[k] = GAMMA1(temp[k - 2]) + temp[k - 7] + GAMMA0(temp[k - 15]) + temp[k - 16];

    // Save the current state
    uint32_t state[8];
    for (uint8_t i = 0; i < 8; i++)
        state[i] = result[i];

    // Process the message in 64-byte chunks
    for (size_t i = 0; i < 64; i++) {
        uint32_t choose, sum, majority, newA, newE;

        choose = CHOOSE(state[4], state[5], state[6]);
        majority = MAJORITY(state[0], state[1], state[2]);
        sum = temp[i] + bytes[i] + state[7] + choose + SIGMA1(state[4]);
        newA = SIGMA0(state[0]) + majority + sum;
        newE = state[3] + sum;

        state[7] = state[6];
        state[6] = state[5];
        state[5] = state[4];
        state[4] = newE;
        state[3] = state[2];
        state[2] = state[1];
        state[1] = state[0];
        state[0] = newA;
    }
    // Add the current chunk's hash to the result
    for (uint8_t i = 0; i < 8; i++) {
        result[i] += state[i];
    }
}

/**
 * Finalizes the SHA-256 hash computation.
 * Applies padding and appends the length of the original message.
 * Returns the final hash as a vector of bytes.
 * 
 * @return A vector containing the SHA-256 hash value.
 */
vector<uint8_t> sha256_finalize()
{
    padding();
    vector<uint8_t> hash(32); // SHA-256 hash size is 32 bytes (256 bits)
    for (int i = 0; i < 8; i++) {
        hash[i * 4] = (result[i] >> 24) & 0xFF;
        hash[i * 4 + 1] = (result[i] >> 16) & 0xFF;
        hash[i * 4 + 2] = (result[i] >> 8) & 0xFF;
        hash[i * 4 + 3] = result[i] & 0xFF;
    }
    return hash;
}

/**
 * Computes the SHA-256 hash for the given input.
 * Resets the internal state, updates with the input data, and returns the final hash.
 * 
 * @param input A vector of bytes to be hashed.
 * @return A vector containing the SHA-256 hash value.
 */
vector<uint8_t> sha256_compute(const std::vector<uint8_t>& input)
{
    // Reset state
    result[0] = 0x6a09e667;
    result[1] = 0xbb67ae85;
    result[2] = 0x3c6ef372;
    result[3] = 0xa54ff53a;
    result[4] = 0x510e527f;
    result[5] = 0x9b05688c;
    result[6] = 0x1f83d9ab;
    result[7] = 0x5be0cd19;

    messageSize = 0;
    bitLength = 0;

    // Update with input data and finalize
    sha256_update(input);
    return sha256_finalize();
}