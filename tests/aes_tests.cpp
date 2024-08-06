#include <cstring> 
#include "gtest/gtest.h"
#include "../include/aes.h"
const unsigned int BLOCK_BYTES_LENGTH = 16 * sizeof(unsigned char);

// Tests for different key lengths
TEST(KeyLengths, KeyLength128)
{
    unsigned char plain[BLOCK_BYTES_LENGTH] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
    AES aes(AESKeyLength::AES_128);
    unsigned char* key = aes.generateKey(AESKeyLength::AES_128);
    unsigned char *encrypted = aes.encrypt(plain, BLOCK_BYTES_LENGTH, key);
    unsigned char *decrypted = aes.decrypt(encrypted, BLOCK_BYTES_LENGTH, key);
    ASSERT_FALSE(memcmp(plain, decrypted, BLOCK_BYTES_LENGTH));
    delete[] encrypted;
    delete[] decrypted;
}

TEST(KeyLengths, KeyLength192)
{
    unsigned char plain[BLOCK_BYTES_LENGTH] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                                               0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
    AES aes(AESKeyLength::AES_192);
    unsigned char* key = aes.generateKey(AESKeyLength::AES_192);
    unsigned char *encrypted = aes.encrypt(plain, BLOCK_BYTES_LENGTH, key);
    unsigned char *decrypted = aes.decrypt(encrypted, BLOCK_BYTES_LENGTH, key);
    ASSERT_FALSE(memcmp(plain, decrypted, BLOCK_BYTES_LENGTH));
    delete[] encrypted;
    delete[] decrypted;
}

TEST(KeyLengths, KeyLength256)
{
    unsigned char plain[BLOCK_BYTES_LENGTH] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                                               0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
    AES aes(AESKeyLength::AES_256);
    unsigned char* key = aes.generateKey(AESKeyLength::AES_256);
    unsigned char *encrypted = aes.encrypt(plain, BLOCK_BYTES_LENGTH, key);
    unsigned char *decrypted = aes.decrypt(encrypted, BLOCK_BYTES_LENGTH, key);
    ASSERT_FALSE(memcmp(plain, decrypted, BLOCK_BYTES_LENGTH));
    delete[] encrypted;
    delete[] decrypted;
}

// Tests for encryption and decryption of one block
TEST(ECB, EncryptDecryptOneBlock)
{
    unsigned char plain[BLOCK_BYTES_LENGTH] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                                               0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
    AES aes(AESKeyLength::AES_256);
    unsigned char* key = aes.generateKey(AESKeyLength::AES_256);
    unsigned char *encrypted = aes.encrypt(plain, BLOCK_BYTES_LENGTH, key);
    unsigned char *decrypted = aes.decrypt(encrypted, BLOCK_BYTES_LENGTH, key);
    ASSERT_FALSE(memcmp(plain, decrypted, BLOCK_BYTES_LENGTH));
    delete[] encrypted;
    delete[] decrypted;
}

// Tests for encryption of one block with missing or extra bytes
TEST(ECB, OneBlockWithoutByteEncrypt)
{
    unsigned char plain[15] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                               0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee}; // 15 bytes
    unsigned char key[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                             0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
    AES aes(AESKeyLength::AES_128);
    ASSERT_THROW(aes.encrypt(plain, 15, key), std::length_error); // Expecting invalid_argument exception
}

TEST(ECB, OneBlockExtraByteEncrypt)
{
    unsigned char plain[17] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                               0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, // 17 bytes
                               0x01};                                          // Extra byte
    unsigned char key[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                             0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
    AES aes(AESKeyLength::AES_128);
    ASSERT_THROW(aes.encrypt(plain, 17, key), std::length_error); // Expecting invalid_argument exception
}