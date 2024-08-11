#ifndef SHA256_H
#define SHA256_H

#include <cstdint>
#include <vector>
#include <string>

// Function prototypes for SHA-256 hashing
void sha256_update(const std::vector<uint8_t>& data);
std::vector<uint8_t> sha256_finalize();
std::vector<uint8_t> sha256_compute(const std::vector<uint8_t>& input);
void transform();

#endif // SHA256_H