#include "../include/aes.h"
#include <stdexcept>
#include <random>
#include <iostream>
#ifdef USE_SYCL
#include <cstring>
#include <stdexcept>
#include <random>
#include <CL/sycl.hpp>
#include <iostream>
#include <fstream>
using namespace cl::sycl;
#endif

void generateRandomIV(unsigned char* iv) 
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);
    for (unsigned int i = 0; i < 16; i++) 
        iv[i] = static_cast<unsigned char>(dis(gen));
}

/** 
 @brief Pads the input message to ensure it is a multiple of the block size.
 @param message The message to be padded.
 @param length The original length of the message.
 @param paddedLength The length of the padded message.
 */
void padMessage(unsigned char *&message, unsigned int &length,
                unsigned int &paddedLength)
{
    size_t originalLength = length;

    paddedLength =
        ((originalLength + BLOCK_BYTES_LEN - 1) / BLOCK_BYTES_LEN) * BLOCK_BYTES_LEN;
    unsigned char *paddedMessage = new unsigned char[paddedLength];

    memcpy(paddedMessage, message, originalLength);

    unsigned char paddingValue =
        static_cast<unsigned char>(paddedLength - originalLength);
    for (size_t i = originalLength; i < paddedLength; i++)
        paddedMessage[i] = paddingValue;

    message = paddedMessage;
    length = paddedLength;
}

/**
 @brief Removes padding from the message.
 @param message The padded message.
 @param length The length of the padded message, which will be updated to the unpadded length.
 */
void unpadMessage(unsigned char *message, unsigned int &length)
{
    size_t originalLength = length;
    unsigned char paddingValue = message[originalLength - 1];
    length = originalLength - paddingValue;
}

/**
 @brief Generates a random AES key of the specified length.
 @param keyLength The length of the key (128, 192, or 256 bits).
 @return A pointer to the generated key.
 */
unsigned char *generateKey(AESKeyLength keyLength)
{

    // Allocate memory for the key
    unsigned char *key = new unsigned char[aesKeyLengthData[keyLength].keySize];

    // Initialize a random device and a random number generator
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<unsigned char> dis(0, 255);

    // Fill the key with random bytes
    for (int i = 0; i < aesKeyLengthData[keyLength].keySize; i++)
        key[i] = dis(gen);

    return key;
}

/**
 @brief Encrypts a message using AES.
 @param in The input message to be encrypted.
 @param inLen The length of the input message.
 @param key The encryption key.
 @param out The encrypted output message.
 @param outLen The length of the encrypted message.
 */
void encryptAES(unsigned char in[], unsigned int inLen, unsigned char *key,
                unsigned char *&out, unsigned int &outLen, AESKeyLength keyLength)
{
    unsigned char *iv = new unsigned char[16];
    generateRandomIV(iv);
    encrypt(in, inLen, key, out, outLen, iv, keyLength);
    unsigned char *newOut = new unsigned char[outLen + 16];
    memcpy(newOut, out, outLen);
    memcpy(newOut + outLen, iv, 16);
    delete[] out;
    out = newOut;
    outLen += 16;
}

/**
 @brief Decrypts a message using AES.
 @param in The input message to be decrypted.
 @param inLen The length of the input message.
 @param key The decryption key.
 @param out The decrypted output message.
 @param outLen The length of the decrypted message.
 */
void decryptAES(const unsigned char in[], unsigned int inLen,
                unsigned char *key, unsigned char *&out, unsigned int &outLen, AESKeyLength keyLength)
{
    unsigned int messageLen = inLen - 16;
    decrypt(in, messageLen, key, out, outLen, in + messageLen, keyLength);
}

/**
 @brief Decrypts a message block using the provided key and IV.
 @param in The input message block to be decrypted.
 @param inLen The length of the input block.
 @param key The decryption key.
 @param out The decrypted output block.
 @param outLen The length of the decrypted block.
 @param iv The initialization vector.
 */
void decrypt(const unsigned char in[], unsigned int inLen, unsigned char *key,
             unsigned char *&out, unsigned int &outLen, const unsigned char *iv,AESKeyLength keyLength)
{
    checkLength(inLen);
    unsigned char block[BLOCK_BYTES_LEN];
    outLen = inLen;
    out = new unsigned char[outLen];
    unsigned char *roundKeys =
        new unsigned char[(aesKeyLengthData[keyLength].numRound + 1) * NUM_BLOCKS * 4];
    keyExpansion(key, roundKeys, keyLength);
    memcpy(block, iv, BLOCK_BYTES_LEN);

    for (unsigned int i = 0; i < outLen; i += BLOCK_BYTES_LEN) {
        decryptBlock(in + i, out + i, roundKeys, keyLength);
        xorBlocks(block, out + i, out + i, BLOCK_BYTES_LEN);
        memcpy(block, in + i, BLOCK_BYTES_LEN);
    }
    
    unpadMessage(out, outLen);
    delete[] roundKeys;
}

/**
 @brief Encrypts a message block using the provided key and IV.
 @param in The input message block to be encrypted.
 @param inLen The length of the input block.
 @param key The encryption key.
 @param out The encrypted output block.
 @param outLen The length of the encrypted block.
 @param iv The initialization vector.
 */
void encrypt(unsigned char in[], unsigned int inLen, unsigned char *key,
             unsigned char *&out, unsigned int &outLen, const unsigned char *iv, AESKeyLength keyLength)
{
    padMessage(in, inLen, outLen);
    unsigned char block[BLOCK_BYTES_LEN];
    out = new unsigned char[outLen];
    unsigned char *roundKeys =
        new unsigned char[(aesKeyLengthData[keyLength].numRound + 1) * NUM_BLOCKS * 4];
    keyExpansion(key, roundKeys, keyLength);
    memcpy(block, iv, BLOCK_BYTES_LEN);
    for (unsigned int i = 0; i < outLen; i += BLOCK_BYTES_LEN) {
        xorBlocks(block, in + i, block, BLOCK_BYTES_LEN);
        encryptBlock(block, out + i, roundKeys, keyLength);
        memcpy(block, out + i, BLOCK_BYTES_LEN);
    }

    delete[] roundKeys;
}

/**
 @brief Checks if the input length is a multiple of the block length.
 @param len The length of the input data.
 @throws std::length_error if the length is not a multiple of the block length.
 */
void checkLength(unsigned int len)
{
    if (len % BLOCK_BYTES_LEN != 0)
        throw std::length_error("Plaintext length must be divisible by " +
                                std::to_string(BLOCK_BYTES_LEN));
}

#ifdef USE_SYCL

/**
 @brief Encrypts a single block of data using SYCL.
 @param in The input block to be encrypted.
 @param out The output block to store the encrypted data.
 @param roundKeys The expanded key for encryption.
 */
void encryptBlock(const unsigned char in[], unsigned char out[],
                  unsigned char *roundKeys, AESKeyLength keyLength)
{    
    queue queue;
    // State array initialization
    unsigned char state[4][NUM_BLOCKS];

    // Buffers for SYCL
    buffer<unsigned char, 1> in_buf(in, range<1>(4 * NUM_BLOCKS));
    buffer<unsigned char, 1> state_buf(
        reinterpret_cast<unsigned char *>(state),
        range<1>(4 * NUM_BLOCKS));
    buffer<unsigned char, 1> roundKeys_buf(
        roundKeys, range<1>(4 * (aesKeyLengthData[keyLength].numRound + 1) * NUM_BLOCKS));
    buffer<unsigned char, 1> out_buf(out, range<1>(4 * NUM_BLOCKS));

    // Initialize state array with input block
    queue.submit([&](handler &handler) {
         auto in_acc = in_buf.get_access<access::mode::read>(handler);
         auto state_acc = state_buf.get_access<access::mode::write>(handler);
         handler.parallel_for(range<1>(4 * NUM_BLOCKS), [=](id<1> idx) {
             unsigned int i = idx % 4;  // Row
             unsigned int j = idx / 4;  // Column
             state_acc[i * NUM_BLOCKS + j] = in_acc[i + 4 * j];
         });
     }).wait();

    // Initial round key addition
    addRoundKey(state, roundKeys);

    // Main rounds
    for (unsigned int round = 1; round < aesKeyLengthData[keyLength].numRound; round++) {
        subBytes(state);
        shiftRows(state);
        mixColumns(state);
        addRoundKey(state, roundKeys + round * 4 * NUM_BLOCKS);
    }

    // Final round
    subBytes(state);
    shiftRows(state);
    addRoundKey(state, roundKeys + aesKeyLengthData[keyLength].numRound * 4 * NUM_BLOCKS);

    // Copy state array to output block
    queue.submit([&](handler &handler) {
         auto state_acc = state_buf.get_access<access::mode::read>(handler);
         auto out_acc = out_buf.get_access<access::mode::write>(handler);
         handler.parallel_for(range<1>(4 * NUM_BLOCKS), [=](id<1> idx) {
             unsigned int i = idx % 4;  // Row
             unsigned int j = idx / 4;  // Column
             out_acc[i + 4 * j] = state_acc[i * NUM_BLOCKS + j];
         });
     }).wait();
}

/**
 @brief Decrypts a single block of data using SYCL.
 Uses SYCL to perform AES decryption on the input block and stores the result in the output block.
 @param in Input block to be decrypted.
 @param[out] out Output block to store the decrypted data.
 @param roundKeys Expanded key for decryption.
 */
void decryptBlock(const unsigned char in[], unsigned char out[],
                  unsigned char *roundKeys, AESKeyLength keyLength)
{
    unsigned char state[4][NUM_BLOCKS];
    unsigned int i, j, round;

    // Initialize state array with input block
    for (i = 0; i < 4; i++)
        for (j = 0; j < NUM_BLOCKS; j++)
            state[i][j] = in[i + 4 * j];

    // Initial round key addition
    addRoundKey(state, roundKeys + aesKeyLengthData[keyLength].numRound * 4 * NUM_BLOCKS);

    // Main rounds
    for (round = aesKeyLengthData[keyLength].numRound - 1; round >= 1; round--) {
        invSubBytes(state);
        invShiftRows(state);
        addRoundKey(state, roundKeys + round * 4 * NUM_BLOCKS);
        invMixColumns(state);
    }

    // Final round
    invSubBytes(state);
    invShiftRows(state);
    addRoundKey(state, roundKeys);

    // Copy state array to output block
    for (i = 0; i < 4; i++)
        for (j = 0; j < NUM_BLOCKS; j++)
            out[i + 4 * j] = state[i][j];
}

/**
 @brief Multiplies a byte by x in GF(2^8).
 Multiplies the input byte by 2 in GF(2^8) and applies the irreducible polynomial if necessary.
 @param b Input byte.
 @return Result of the multiplication.
 */
 unsigned char xtime(unsigned char b)
{
    return (b << 1) ^ (((b >> 7) & 1) * 0x1b);
}

/**
 @brief Multiplies two bytes in GF(2^8).
 Multiplies two bytes using the Galois field (GF) arithmetic.
 @param x First byte.
 @param y Second byte.
 @return Result of the multiplication.
 */
 unsigned char multiply(unsigned char x, unsigned char y)
{
    unsigned char result = 0;
    unsigned char temp = y;
    for (int i = 0; i < 8; i++) {
        if (x & 1)
            result ^= temp;
        bool carry = temp & 0x80;
        temp <<= 1;
        if (carry)
            temp ^= 0x1b;
        x >>= 1;
    }
    return result;
}

/**
 @brief Applies the SubBytes transformation using SYCL.
 Substitutes bytes in the state array using the S-box.
 @param state 2D array representing the state of the AES block.
 */
void subBytes(unsigned char state[4][NUM_BLOCKS])
{
    queue queue;
    buffer<unsigned char, 2> stateBuffer(state[0],
                                               range<2>(4, NUM_BLOCKS));
    buffer<unsigned char, 2> sBoxBuffer(sBox[0], range<2>(16, 16));
    queue.submit([&](handler &handler) {
         auto stateAcc =
             stateBuffer.get_access<access::mode::read_write>(handler);
         auto sBoxAcc = sBoxBuffer.get_access<access::mode::read>(handler);
         handler.parallel_for<class SubBytes>(
             range<2>(4, NUM_BLOCKS), [=](id<2> id) {
                 size_t i = id[0];
                 size_t j = id[1];
                 stateAcc[i][j] = sBoxAcc[state[i][j] / 16][state[i][j] % 16];
             });
     }).wait();
}

/**
 @brief Applies the ShiftRows transformation using SYCL.
 Shifts the rows of the state array in the AES block.
 @param state 2D array representing the state of the AES block.
 */
void shiftRows(unsigned char state[4][NUM_BLOCKS])
{
    queue queue;
    queue.submit([&](handler &handler) {
         handler.parallel_for(range<1>(4), [=](id<1> i) {
             unsigned char tmp[NUM_BLOCKS];
             for (size_t k = 0; k < NUM_BLOCKS; k++)
                 tmp[k] = state[i][(k + i) % NUM_BLOCKS];
             for (size_t k = 0; k < NUM_BLOCKS; k++)
                 state[i][k] = tmp[k];
         });
     }).wait();
}

/**
 @brief Applies the MixColumns transformation using SYCL.
 Mixes the columns of the state array in the AES block.
 @param state 2D array representing the state of the AES block.
 */
void mixColumns(unsigned char state[4][NUM_BLOCKS])
{
    queue queue;

    queue.parallel_for(range<1>(4), [=](id<1> idx) {
         size_t i = idx[0];
         unsigned char a[4];
         unsigned char b[4];

         for (size_t j = 0; j < 4; j++) {
             a[j] = state[j][i];
             b[j] = xtime(state[j][i]);
         }

         state[0][i] = b[0] ^ a[1] ^ b[1] ^ a[2] ^ a[3];
         state[1][i] = a[0] ^ b[1] ^ a[2] ^ b[2] ^ a[3];
         state[2][i] = a[0] ^ a[1] ^ b[2] ^ a[3] ^ b[3];
         state[3][i] = b[0] ^ a[0] ^ a[1] ^ a[2] ^ b[3];
     }).wait();
}

/**
 @brief Applies the AddRoundKey transformation using SYCL.
 XORs the state array with the round key in the AES block.
 @param state 2D array representing the state of the AES block.
 @param roundKey Round key to be XORed with the state array.
 */
void addRoundKey(unsigned char state[4][NUM_BLOCKS], unsigned char *roundKey)
{
    queue queue;
    buffer<unsigned char, 2> stateBuffer(state[0],
                                               range<2>(4, NUM_BLOCKS));
    buffer<unsigned char, 1> roundKeyBuffer(
        roundKey, range<1>(4 * NUM_BLOCKS));

    queue.submit([&](handler &handler) {
         auto stateAcc =
             stateBuffer.get_access<access::mode::read_write>(handler);
         auto roundKeyAcc =
             roundKeyBuffer.get_access<access::mode::read>(handler);

         handler.parallel_for<class AddRoundKey>(
             range<2>(4, NUM_BLOCKS), [=](id<2> id) {
                 size_t i = id[0];
                 size_t j = id[1];
                 stateAcc[i][j] ^= roundKeyAcc[i + 4 * j];
             });
     }).wait();
}

/**
 @brief Applies the SubWord transformation using SYCL.
 Substitutes bytes in a word using the S-box.
 @param a Array representing the word to be transformed.
 */
void subWord(unsigned char a[4])
{
    queue queue;
    buffer<unsigned char, 1> aBuffer(a, range<1>(4));
    buffer<unsigned char, 2> sBoxBuffer(sBox[0], range<2>(16, 16));

    queue.submit([&](handler &handler) {
         auto aAcc = aBuffer.get_access<access::mode::read_write>(handler);
         auto sBoxAcc = sBoxBuffer.get_access<access::mode::read>(handler);

         handler.parallel_for<class SubWord>(
             range<1>(4), [=](id<1> id) {
                 size_t i = id[0];
                 aAcc[i] = sBoxAcc[aAcc[i] / 16][aAcc[i] % 16];
             });
     }).wait();
}

/**
 @brief Rotates a word (4 bytes) using SYCL.
 Performs a circular rotation of a word.
 @param word Pointer to the word to be rotated.
 */
void rotWord(unsigned char *word)
{
    queue queue;
    buffer<unsigned char, 1> wordBuffer(word, range<1>(4));

    queue.submit([&](handler &handler) {
         auto wordAcc =
             wordBuffer.get_access<access::mode::read_write>(handler);

         handler.single_task<class RotWord>([=]() {
             unsigned char temp = wordAcc[0];
             wordAcc[0] = wordAcc[1];
             wordAcc[1] = wordAcc[2];
             wordAcc[2] = wordAcc[3];
             wordAcc[3] = temp;
         });
     }).wait();
}

/**
 @brief Applies the Rcon transformation using SYCL.
 Generates the Rcon value for key expansion based on the round number.
 @param a Array to store the Rcon value.
 @param n Round number.
 */
void rconWord(unsigned char a[4], unsigned int n)
{
    queue queue;
    unsigned char strong = 1;
    for (size_t i = 0; i < n - 1; i++)
        strong = xtime(strong);

    queue.parallel_for(range<1>(4), [=](id<1> idx) {
         size_t i = idx[0];
         if (i == 0) {
             a[i] = strong;
         }
         else {
             a[i] = 0;
         }
     }).wait();
}

/**
 @brief Expands the AES key for encryption/decryption using SYCL.
 Generates the round keys from the original key.
 @param key Original AES key.
 @param w Output array to store the expanded key.
 */
void keyExpansion(const unsigned char *key, unsigned char w[], AESKeyLength keyLength)
{
    queue queue;
    unsigned int numWordLocal = aesKeyLengthData[keyLength].numWord;

    unsigned int NUM_BLOCKSLocal = NUM_BLOCKS;
    unsigned int numRoundLocal = aesKeyLengthData[keyLength].numRound;
    // Copy the initial key to the output array
    queue.submit([&](handler &handler) {
         handler.parallel_for(range<1>(4 * numWordLocal),
                        [=](id<1> idx) { w[idx] = key[idx]; });
     }).wait();

    unsigned int i = 4 * numWordLocal;

    while (i < 4 * NUM_BLOCKSLocal * (numRoundLocal + 1)) {
        unsigned char temp[4];
        buffer<unsigned char, 1> temp_buf(temp, range<1>(4));

        queue.submit([&](handler &handler) {
            auto temp_acc =
                temp_buf.get_access<access::mode::write>(handler);
            handler.parallel_for(range<1>(4), [=](id<1> idx) {
                temp_acc[idx] = w[i - 4 + idx];
            });
        }).wait();

        if (i / 4 % numWordLocal == 0) {
        // Use SYCL to execute rotWord and subWord
            rotWord(temp);
            subWord(temp);
            unsigned char rcon[4];
            rconWord(rcon, i / (numWordLocal * 4));
            
            for (int k = 0; k < 4; k++) 
                temp[k] ^= rcon[k];
        }
        else if (numWordLocal > 6 && i / 4 % numWordLocal == 4) 
            subWord(temp);

        queue.submit([&](handler &handler) {
            auto temp_acc =
                temp_buf.get_access<access::mode::read>(handler);
            handler.parallel_for(range<1>(4), [=](id<1> idx) {
                w[i + idx] = w[i + idx - 4 * numWordLocal] ^ temp_acc[idx];
            });
        }).wait();

        i += 4;
    }
}

/**
 Applies the InvSubBytes transformation using the inverse S-box.
 This function replaces each byte in the state matrix with its
 corresponding byte from the inverse S-box.
 @param state 4xN matrix of state bytes to be transformed.
 */
void invSubBytes(unsigned char state[4][NUM_BLOCKS])
{
    queue queue;
    buffer<unsigned char, 2> stateBuffer(state[0],
                                               range<2>(4, NUM_BLOCKS));
    buffer<unsigned char, 2> invSBoxBuffer(invSBox[0],
                                                 range<2>(16, 16));

    queue.submit([&](handler &handler) {
         auto stateAcc =
             stateBuffer.get_access<access::mode::read_write>(handler);
         auto invSBoxAcc =
             invSBoxBuffer.get_access<access::mode::read>(handler);

         handler.parallel_for<class invSubBytes>(
             range<2>(4, NUM_BLOCKS), [=](id<2> id) {
                 size_t i = id[0];
                 size_t j = id[1];
                 stateAcc[i][j] =
                     invSBoxAcc[state[i][j] / 16][state[i][j] % 16];
             });
     }).wait();
}

/*Applies the InvMixColumns transformation*/
void invMixColumns(unsigned char state[4][NUM_BLOCKS])
{
    queue queue;
    queue.parallel_for(range<1>(NUM_BLOCKS), [=](id<1> idx) {
         size_t i = idx[0];
         unsigned char a[4];
         unsigned char b[4];

         for (size_t j = 0; j < 4; j++) {
             a[j] = state[j][i];
             b[j] = xtime(a[j]);
         }

         state[0][i] = multiply(0x0e, a[0]) ^ multiply(0x0b, a[1]) ^
                       multiply(0x0d, a[2]) ^ multiply(0x09, a[3]);
         state[1][i] = multiply(0x09, a[0]) ^ multiply(0x0e, a[1]) ^
                       multiply(0x0b, a[2]) ^ multiply(0x0d, a[3]);
         state[2][i] = multiply(0x0d, a[0]) ^ multiply(0x09, a[1]) ^
                       multiply(0x0e, a[2]) ^ multiply(0x0b, a[3]);
         state[3][i] = multiply(0x0b, a[0]) ^ multiply(0x0d, a[1]) ^
                       multiply(0x09, a[2]) ^ multiply(0x0e, a[3]);
     }).wait();
}

/**
 Applies the InvShiftRows transformation.
 This function performs a cyclic shift of the rows in the state matrix
 in the reverse direction for decryption.
 @param state 4xN matrix of state bytes to be transformed.
 */
void invShiftRows(unsigned char state[4][NUM_BLOCKS])
{
    queue queue;
    queue.submit([&](handler &handler) {
         handler.parallel_for(range<1>(4), [=](id<1> i) {
             unsigned char tmp[NUM_BLOCKS];
             for (size_t k = 0; k < NUM_BLOCKS; k++)
                 tmp[k] = state[i][(k - i + NUM_BLOCKS) % NUM_BLOCKS];
             for (size_t k = 0; k < NUM_BLOCKS; k++)
                 state[i][k] = tmp[k];
         });
     }).wait();
}

/** XORs two blocks of bytes a and b of length len, storing the result in c. */
void xorBlocks(const unsigned char *a, const unsigned char *b, unsigned char *c,
               unsigned int len)
{
    queue queue;

    buffer<unsigned char, 1> bufA(a, range<1>(len));
    buffer<unsigned char, 1> bufB(b, range<1>(len));
    buffer<unsigned char, 1> bufC(c, range<1>(len));

    queue.submit([&](handler &handler) {
         auto accA = bufA.get_access<access::mode::read>(handler);
         auto accB = bufB.get_access<access::mode::read>(handler);
         auto accC = bufC.get_access<access::mode::write>(handler);

         handler.parallel_for(range<1>(len),
                        [=](id<1> idx) { accC[idx] = accA[idx] ^ accB[idx]; });
     }).wait();
}

#else
/*
 Encrypts a single block of data.
 `in` - input block to be encrypted
 `out` - output block to store the encrypted data
 `roundKeys` - expanded key for encryption
*/
void encryptBlock(const unsigned char in[], unsigned char out[],
                  unsigned char *roundKeys, AESKeyLength keyLength)
{
    unsigned char state[4][NUM_BLOCKS];
    unsigned int i, j, round;

    // Initialize state array with input block
    for (i = 0; i < 4; i++)
        for (j = 0; j < NUM_BLOCKS; j++)
            state[i][j] = in[i + 4 * j];

    // Initial round key addition
    addRoundKey(state, roundKeys);

    // Main rounds
    for (round = 1; round < aesKeyLengthData[keyLength].numRound; round++) {
        subBytes(state);
        shiftRows(state);
        mixColumns(state);
        addRoundKey(state, roundKeys + round * 4 * NUM_BLOCKS);
    }

    // Final round
    subBytes(state);
    shiftRows(state);
    addRoundKey(state, roundKeys + aesKeyLengthData[keyLength].numRound * 4 * NUM_BLOCKS);

    // Copy state array to output block
    for (i = 0; i < 4; i++)
        for (j = 0; j < NUM_BLOCKS; j++)
            out[i + 4 * j] = state[i][j];
}

/*
 Decrypts a single block of data.
 `in` - input block to be decrypted
 `out` - output block to store the decrypted data
 `roundKeys` - expanded key for decryption
*/
void decryptBlock(const unsigned char in[], unsigned char out[],
                  unsigned char *roundKeys, AESKeyLength keyLength)
{
    unsigned char state[4][NUM_BLOCKS];
    unsigned int i, j, round;

    // Initialize state array with input block
    for (i = 0; i < 4; i++)
        for (j = 0; j < NUM_BLOCKS; j++)
            state[i][j] = in[i + 4 * j];

    // Initial round key addition
    addRoundKey(state, roundKeys + aesKeyLengthData[keyLength].numRound * 4 * NUM_BLOCKS);

    // Main rounds
    for (round = aesKeyLengthData[keyLength].numRound - 1; round >= 1; round--) {
        invSubBytes(state);
        invShiftRows(state);
        addRoundKey(state, roundKeys + round * 4 * NUM_BLOCKS);
        invMixColumns(state);
    }

    // Final round
    invSubBytes(state);
    invShiftRows(state);
    addRoundKey(state, roundKeys);

    // Copy state array to output block
    for (i = 0; i < 4; i++)
        for (j = 0; j < NUM_BLOCKS; j++)
            out[i + 4 * j] = state[i][j];
}

/*Multiplies a byte by x in GF(2^8)*/
unsigned char xtime(unsigned char b)
{
    return (b << 1) ^ (((b >> 7) & 1) * 0x1b);
}

/*Multiplies two bytes in GF(2^8)*/
unsigned char multiply(unsigned char x, unsigned char y)
{
    unsigned char result = 0;
    unsigned char temp = y;
    for (int i = 0; i < 8; i++) {
        if (x & 1)
            result ^= temp;
        bool carry = temp & 0x80;
        temp <<= 1;
        if (carry)
            temp ^= 0x1b;
        x >>= 1;
    }
    return result;
}

/*Apply SubBytes transformation using the S-box*/
void subBytes(unsigned char state[4][NUM_BLOCKS])
{
    for (size_t i = 0; i < 4; i++)
        for (size_t j = 0; j < NUM_BLOCKS; j++)
            state[i][j] = sBox[state[i][j] / 16][state[i][j] % 16];
}

/*ShiftRows transformation*/
void shiftRows(unsigned char state[4][NUM_BLOCKS])
{
    for (size_t i = 0; i < 4; i++) {
        unsigned char tmp[NUM_BLOCKS];
        for (size_t k = 0; k < NUM_BLOCKS; k++)
            tmp[k] = state[i][(k + i) % NUM_BLOCKS];
        memcpy(state[i], tmp, NUM_BLOCKS * sizeof(unsigned char));
    }
}

/*MixColumns transformation*/
void mixColumns(unsigned char state[4][NUM_BLOCKS])
{
    for (size_t i = 0; i < 4; i++) {
        unsigned char a[4];
        unsigned char b[4];
        for (size_t j = 0; j < 4; j++) {
            a[j] = state[j][i];
            b[j] = xtime(state[j][i]);
        }
        state[0][i] = b[0] ^ a[1] ^ b[1] ^ a[2] ^ a[3];
        state[1][i] = a[0] ^ b[1] ^ a[2] ^ b[2] ^ a[3];
        state[2][i] = a[0] ^ a[1] ^ b[2] ^ a[3] ^ b[3];
        state[3][i] = b[0] ^ a[0] ^ a[1] ^ a[2] ^ b[3];
    }
}

/*AddRoundKey transformation*/
void addRoundKey(unsigned char state[4][NUM_BLOCKS], unsigned char *key)
{
    unsigned int i, j;
    for (i = 0; i < 4; i++)
        for (j = 0; j < NUM_BLOCKS; j++)
            state[i][j] = state[i][j] ^ key[i + 4 * j];
}

/*Applies the SubWord transformation using the S-box*/
void subWord(unsigned char a[4])
{
    for (size_t i = 0; i < 4; i++)
        a[i] = sBox[a[i] / 16][a[i] % 16];
}

/*Rotates a word (4 bytes)*/
void rotWord(unsigned char a[4])
{
    unsigned char first = a[0];
    for (size_t i = 0; i < 3; i++)
        a[i] = a[i + 1];
    a[3] = first;
}

/*Applies the Rcon transformation*/
void rconWord(unsigned char a[4], unsigned int n)
{
    unsigned char strong = 1;
    for (size_t i = 0; i < n - 1; i++)
        strong = xtime(strong);

    a[0] = strong;
    a[1] = a[2] = a[3] = 0;
}

/*Expands the key for AES encryption/decryption*/
void keyExpansion(const unsigned char *key, unsigned char w[], AESKeyLength keyLength)
{
    unsigned char temp[4], rcon[4];
    unsigned int i = 0;

    //copy key to w array
    while (i < 4 * aesKeyLengthData[keyLength].numWord) {
        w[i] = key[i];
        i++;
    }

    i = 4 * aesKeyLengthData[keyLength].numWord;

    //main expansion loop
    while (i < 4 * NUM_BLOCKS * (aesKeyLengthData[keyLength].numRound + 1)) {
        for (size_t k = 4, j = 0; k > 0; --k, ++j)
            temp[j] = w[i - k];

        if (i / 4 % aesKeyLengthData[keyLength].numWord == 0) {
            rotWord(temp);
            subWord(temp);
            rconWord(rcon, i / (aesKeyLengthData[keyLength].numWord * 4));

            for (int i = 0; i < 4; i++)
                temp[i] = temp[i] ^ rcon[i];
        }
        else if (aesKeyLengthData[keyLength].numWord > 6 && i / 4 % aesKeyLengthData[keyLength].numWord == 4)
            subWord(temp);

        for (int k = 0; k < 4; k++)
            w[i + k] = w[i + k - 4 * aesKeyLengthData[keyLength].numWord] ^ temp[k];

        i += 4;
    }
}

/*Applies the InvSubBytes transformation using the inverse S-box*/
void invSubBytes(unsigned char state[4][NUM_BLOCKS])
{
    unsigned int i, j;
    unsigned char t;
    for (i = 0; i < 4; i++)
        for (j = 0; j < NUM_BLOCKS; j++) {
            t = state[i][j];
            state[i][j] = invSBox[t / 16][t % 16];
        }
}

/*Applies the InvMixColumns transformation*/
void invMixColumns(unsigned char state[4][NUM_BLOCKS])
{
    for (size_t i = 0; i < NUM_BLOCKS; i++) {
        unsigned char a[4];
        unsigned char b[4];
        for (size_t j = 0; j < 4; j++) {
            a[j] = state[j][i];
            b[j] = xtime(a[j]);
        }
        state[0][i] = multiply(0x0e, a[0]) ^ multiply(0x0b, a[1]) ^
                      multiply(0x0d, a[2]) ^ multiply(0x09, a[3]);
        state[1][i] = multiply(0x09, a[0]) ^ multiply(0x0e, a[1]) ^
                      multiply(0x0b, a[2]) ^ multiply(0x0d, a[3]);
        state[2][i] = multiply(0x0d, a[0]) ^ multiply(0x09, a[1]) ^
                      multiply(0x0e, a[2]) ^ multiply(0x0b, a[3]);
        state[3][i] = multiply(0x0b, a[0]) ^ multiply(0x0d, a[1]) ^
                      multiply(0x09, a[2]) ^ multiply(0x0e, a[3]);
    }
}

/*Applies the InvShiftRows transformation*/
void invShiftRows(unsigned char state[4][NUM_BLOCKS])
{
    for (size_t i = 0; i < 4; i++) {
        unsigned char tmp[NUM_BLOCKS];
        for (size_t k = 0; k < NUM_BLOCKS; k++)
            tmp[k] = state[i][(k - i + NUM_BLOCKS) % NUM_BLOCKS];
        memcpy(state[i], tmp, NUM_BLOCKS * sizeof(unsigned char));
    }
}

/** XORs two blocks of bytes a and b of length len, storing the result in c. */
void xorBlocks(const unsigned char *a, const unsigned char *b, unsigned char *c,
               unsigned int len)
{
    for (unsigned int i = 0; i < len; i++) {
        c[i] = a[i] ^ b[i];
    }
}
#endif
