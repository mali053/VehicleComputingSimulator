#ifndef SHA3_512_H
#define SHA3_512_H

#include <vector>
#include <string>
#include <cstdint>  // For fixed-width integer types
#include <sstream>  // For std::ostringstream
#include "return_codes.h"

class SHA3_512
{
public:
    SHA3_512();  // Constructor to initialize the state
    CK_RV update(const uint8_t* input, std::size_t length); // Update the hash with more data
    CK_RV finalize(std::string & output); // Finalize and get the hash value
    CK_RV compute(const std::vector<uint8_t>& input, std::string & output); // Single-step hashing

private:
    uint64_t S[5][5];     // State matrix
    uint8_t buffer[576];  // Buffer to hold input data
    std::size_t buffer_length; // Current length of data in the buffer

    void round(uint64_t A[5][5], uint64_t RC);
    void f_function(uint64_t A[5][5]);
    void padding(uint8_t input[], std::size_t &in_len, int &absorb_times) ;
    void assign_S_xor_p(uint64_t S[5][5], uint64_t *p);
    void endianSwap(uint64_t &x);
    std::string hashPartToHexString(uint64_t S[5][5]);
};

#endif // SHA3_512_H
