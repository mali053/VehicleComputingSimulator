#include <cstring>
#include "gtest/gtest.h"
#include "../include/aes.h"
#include "../include/aes_stream_factory.h"  // Assuming this is where your FactoryManager is defined

const unsigned int BLOCK_BYTES_LENGTH = 16 * sizeof(unsigned char);

/* Helper function to setup encryption and decryption */
void testEncryptionDecryption(AESChainingMode mode, AESKeyLength keyLength) {
    unsigned char plain[BLOCK_BYTES_LENGTH] = {
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
        0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff
    };
    unsigned char plain2[BLOCK_BYTES_LENGTH] = {
        0x00, 0x10, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
        0x78, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff
    };

    // Create a factory instance
    StreamAES* streamAES = FactoryManager::getInstance().create(mode);
    ASSERT_NE(streamAES, nullptr); 

    unsigned char* key = generateKey(keyLength);
    unsigned char* encrypted = nullptr;
    unsigned char* encrypted2 = nullptr;
    unsigned int outLenEncrypted = 0;
    unsigned int outLenEncrypted2 = 0;

    streamAES->encryptStart(plain, BLOCK_BYTES_LENGTH, encrypted, outLenEncrypted, key, keyLength);
    streamAES->encryptContinue(plain2, BLOCK_BYTES_LENGTH, encrypted2, outLenEncrypted2);

    unsigned char* decrypted = nullptr;
    unsigned char* decrypted2 = nullptr;
    unsigned int outLenDecrypted = 0;
    unsigned int outLenDecrypted2 = 0;

    streamAES->decryptStart(encrypted, outLenEncrypted, decrypted, outLenDecrypted, key, keyLength);
    streamAES->decryptContinue(encrypted2, outLenEncrypted2, decrypted2, outLenDecrypted2);

    ASSERT_FALSE(memcmp(plain, decrypted, BLOCK_BYTES_LENGTH));
    delete[] encrypted;
    delete[] encrypted2;
    delete[] decrypted;
    delete[] decrypted2;
   // delete streamAES;
}
TEST(KeyLengths, KeyLength128_ECB) 
{
    testEncryptionDecryption(AESChainingMode::ECB, AESKeyLength::AES_128);
}

TEST(KeyLengths, KeyLength128_CBC) 
{
    testEncryptionDecryption(AESChainingMode::CBC, AESKeyLength::AES_128);
}

TEST(KeyLengths, KeyLength128_CFB) 
{
    testEncryptionDecryption(AESChainingMode::CFB, AESKeyLength::AES_128);
}

TEST(KeyLengths, KeyLength128_OFB) 
{
    testEncryptionDecryption(AESChainingMode::OFB, AESKeyLength::AES_128);
}

TEST(KeyLengths, KeyLength128_CTR) 
{
    testEncryptionDecryption(AESChainingMode::CTR, AESKeyLength::AES_128);
}

TEST(KeyLengths, KeyLength192_ECB) 
{
    testEncryptionDecryption(AESChainingMode::ECB, AESKeyLength::AES_192);
}

TEST(KeyLengths, KeyLength192_CBC) 
{
    testEncryptionDecryption(AESChainingMode::CBC, AESKeyLength::AES_192);
}

TEST(KeyLengths, KeyLength192_CFB) 
{
    testEncryptionDecryption(AESChainingMode::CFB, AESKeyLength::AES_192);
}

TEST(KeyLengths, KeyLength192_OFB) 
{
    testEncryptionDecryption(AESChainingMode::OFB, AESKeyLength::AES_192);
}

TEST(KeyLengths, KeyLength192_CTR) 
{
    testEncryptionDecryption(AESChainingMode::CTR, AESKeyLength::AES_192);
}

TEST(KeyLengths, KeyLength256_ECB) 
{
    testEncryptionDecryption(AESChainingMode::ECB, AESKeyLength::AES_256);
}

TEST(KeyLengths, KeyLength256_CBC) 
{
    testEncryptionDecryption(AESChainingMode::CBC, AESKeyLength::AES_256);
}

TEST(KeyLengths, KeyLength256_CFB) 
{
    testEncryptionDecryption(AESChainingMode::CFB, AESKeyLength::AES_256);
}

TEST(KeyLengths, KeyLength256_OFB) 
{
    testEncryptionDecryption(AESChainingMode::OFB, AESKeyLength::AES_256);
}

TEST(KeyLengths, KeyLength256_CTR) 
{
    testEncryptionDecryption(AESChainingMode::CTR, AESKeyLength::AES_256);
}

int main() {
    // Register factories
    FactoryManager& manager = FactoryManager::getInstance();

    // Now run your tests
    ::testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
