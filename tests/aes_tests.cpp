#include <cstring>
#include "gtest/gtest.h"
#include "../include/aes.h"
#include "../include/aes_stream_factory.h"  // Assuming this is where your FactoryManager is defined

/* Helper function to setup encryption and decryption */
void testEncryptionDecryption(AESChainingMode mode, AESKeyLength keyLength) {
    unsigned char plain[BLOCK_BYTES_LEN] = {
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
        0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff
    };
    unsigned char plain2[BLOCK_BYTES_LEN] = {
        0x00, 0x10, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
        0x78, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff
    };
     unsigned char plain3[BLOCK_BYTES_LEN] = {
        0x00, 0x10, 0x21, 0x33, 0x44, 0x55, 0x66, 0x77,
        0x78, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff
    };

    // Create a factory instance
    StreamAES* streamAES = FactoryManager::getInstance().create(mode);
    ASSERT_NE(streamAES, nullptr); 

    unsigned char *key = new unsigned char[aesKeyLengthData[keyLength].keySize];
    generateKey(key, keyLength);
    unsigned char* encrypted = new unsigned char[calculatEncryptedLenAES(BLOCK_BYTES_LEN, true)];
    unsigned char* encrypted2 = new unsigned char[calculatEncryptedLenAES(BLOCK_BYTES_LEN, false)];
    unsigned char* encrypted3 = new unsigned char[calculatEncryptedLenAES(BLOCK_BYTES_LEN, false)];
    
    unsigned int outLenEncrypted = 0;
    unsigned int outLenEncrypted2 = 0;
    unsigned int outLenEncrypted3 = 0;

    streamAES->encryptStart(plain, BLOCK_BYTES_LEN, encrypted, outLenEncrypted, key, keyLength);
    streamAES->encryptContinue(plain2, BLOCK_BYTES_LEN, encrypted2, outLenEncrypted2);
    streamAES->encryptContinue(plain3, BLOCK_BYTES_LEN, encrypted3, outLenEncrypted3);
   
    unsigned char* decrypted;
    // new unsigned char[calculatDecryptedLenAES(BLOCK_BYTES_LEN, true)];
    unsigned char* decrypted2;
    // new unsigned char[calculatDecryptedLenAES(BLOCK_BYTES_LEN, false)];
    unsigned char* decrypted3; 
    // new unsigned char[calculatDecryptedLenAES(BLOCK_BYTES_LEN, false)];
    unsigned int outLenDecrypted = 0;
    unsigned int outLenDecrypted2 = 0;
    unsigned int outLenDecrypted3 = 0;

    streamAES->decryptStart(encrypted, outLenEncrypted, decrypted, outLenDecrypted, key, keyLength);
    streamAES->decryptContinue(encrypted2, outLenEncrypted2, decrypted2, outLenDecrypted2);
    streamAES->decryptContinue(encrypted3, outLenEncrypted3, decrypted3, outLenDecrypted3);


    ASSERT_FALSE(memcmp(plain, decrypted, BLOCK_BYTES_LEN));
    ASSERT_FALSE(memcmp(plain2, decrypted2, BLOCK_BYTES_LEN));
    ASSERT_FALSE(memcmp(plain3, decrypted3, BLOCK_BYTES_LEN));

    delete [] encrypted;
    delete [] encrypted2;
    delete [] encrypted3;
    delete [] decrypted;
    delete [] decrypted2;
    delete [] decrypted3;
    delete [] key;
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
    ::testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}