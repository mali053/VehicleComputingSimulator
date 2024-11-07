#include <cstring>
#include "gtest/gtest.h"
#include "../include/aes.h"
#include "../include/aes_stream_factory.h"  // Assuming this is where your FactoryManager is defined
void printBufferHexa(const uint8_t *buffer, size_t len, std::string message)
{
    std::cout << message << std::endl;
    for (size_t i = 0; i < len; ++i)
    {
        std::cout << std::hex << std::setw(2) << std::setfill('0')
                  << static_cast<int>(buffer[i]) << " ";
        // Print a new line every 16 bytes for better readability
        if ((i + 1) % 16 == 0)
            std::cout << std::endl;
    }
    std::cout << std::endl;
    // Reset the stream format back to decimal
    std::cout << std::dec;
}
/* Helper function to setup encryption and decryption */
void testEncryptionDecryption(AESChainingMode mode, AESKeyLength keyLength) {
    unsigned char plain[64] = {
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
        0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
           0x00, 0x10, 0x21, 0x33, 0x44, 0x55, 0x66, 0x77,
        0x78, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
         0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
        0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
           0x00, 0x10, 0x21, 0x33, 0x44, 0x55, 0x66, 0x77,
        0x78, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
    };
   
    unsigned char plain2[32] = {
        0x00, 0x10, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
        0x78, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
           0x00, 0x10, 0x21, 0x33, 0x44, 0x55, 0x66, 0x77,
        0x78, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff
    };
   
     unsigned char plain3[13] = {
        0x48, 0x65, 0x6c ,0x6c ,0x6f ,0x2c ,0x20 ,0x57 ,0x6f ,0x72 ,0x6c ,0x64 ,0x21
    };
    

    // Create a factory instance
    StreamAES* streamAES = FactoryManager::getInstance().create(mode);
    ASSERT_NE(streamAES, nullptr); 

    unsigned char *key = new unsigned char[aesKeyLengthData[keyLength].keySize];
    generateKey(key, keyLength);
    unsigned char* encrypted = new unsigned char[calculatEncryptedLenAES(64, true)];
    unsigned char* encrypted2 = new unsigned char[calculatEncryptedLenAES(32, false)];
    unsigned char* encrypted3 = new unsigned char[calculatEncryptedLenAES(32, false)];
    printBufferHexa(encrypted3, 13, "OFB hello world");
    unsigned int outLenEncrypted = 0;
    unsigned int outLenEncrypted2 = 0;
    unsigned int outLenEncrypted3 = 0;

    streamAES->encryptStart(plain, 64, encrypted, outLenEncrypted, key, keyLength);
    streamAES->encryptContinue(plain2, 32, encrypted2, outLenEncrypted2);
    streamAES->encryptContinue(plain3, 32, encrypted3, outLenEncrypted3);
   
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


    ASSERT_FALSE(memcmp(plain, decrypted, 64));
    ASSERT_FALSE(memcmp(plain2, decrypted2, 32));
    ASSERT_FALSE(memcmp(plain3, decrypted3, 32));

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

int main() 
{
    ::testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
