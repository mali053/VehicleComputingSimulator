#include <cstring>
#include "gtest/gtest.h"
#include "../include/aes.h"
const unsigned int BLOCK_BYTES_LENGTH = 16 * sizeof(unsigned char);

/*Tests for different key lengths*/
TEST(KeyLengths, KeyLength128)
{
    unsigned char plain[BLOCK_BYTES_LENGTH] = {
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
        0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};

    unsigned char *key = generateKey(AESKeyLength::AES_128);
    unsigned char *encrypted;
    unsigned int outLenEncrypted;
    encryptAES(plain, BLOCK_BYTES_LENGTH, key, encrypted, outLenEncrypted, AESKeyLength::AES_128);
    unsigned char *decrypted;
    unsigned int outLenDecrypted;
    decryptAES(encrypted, outLenEncrypted, key, decrypted, outLenDecrypted, AESKeyLength::AES_128);
    ASSERT_FALSE(memcmp(plain, decrypted, BLOCK_BYTES_LENGTH));
    delete[] encrypted;
    delete[] decrypted;
}

TEST(KeyLengths, KeyLength192)
{
    unsigned char plain[BLOCK_BYTES_LENGTH] = {
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
        0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
   
    unsigned char *key = generateKey(AESKeyLength::AES_192);
    unsigned char *encrypted;
    unsigned int outLenEncrypted;
    encryptAES(plain, BLOCK_BYTES_LENGTH, key, encrypted, outLenEncrypted, AESKeyLength::AES_192);
    unsigned char *decrypted;
    unsigned int outLenDecrypted;
    decryptAES(encrypted, outLenEncrypted, key, decrypted, outLenDecrypted, AESKeyLength::AES_192);
    ASSERT_FALSE(memcmp(plain, decrypted, BLOCK_BYTES_LENGTH));
    delete[] encrypted;
    delete[] decrypted;
}

TEST(KeyLengths, KeyLength256)
{
    unsigned char plain[BLOCK_BYTES_LENGTH] = {
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
        0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};

    unsigned char *key = generateKey(AESKeyLength::AES_256);
    unsigned char *encrypted;
    unsigned int outLenEncrypted;
    encryptAES(plain, BLOCK_BYTES_LENGTH, key, encrypted, outLenEncrypted, AESKeyLength::AES_256);
    unsigned char *decrypted;
    unsigned int outLenDecrypted;
    decryptAES(encrypted, outLenEncrypted, key, decrypted, outLenDecrypted, AESKeyLength::AES_256);
    ASSERT_FALSE(memcmp(plain, decrypted, BLOCK_BYTES_LENGTH));
    delete[] encrypted;
    delete[] decrypted;
}
/*Tests for encryption and decryption with non 16 multiply length*/
TEST(CBC, EncryptDecryptOneBlock)
{
    unsigned char plain[18] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55,
                               0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb,
                               0xcc, 0xdd, 0xee, 0xff, 0xef, 0x03};
   
    unsigned char *key = generateKey(AESKeyLength::AES_256);
    unsigned char *encrypted;
    unsigned int outLenEncrypted;
    encryptAES(plain, 17, key, encrypted, outLenEncrypted, AESKeyLength::AES_256);
    unsigned char *decrypted;
    unsigned int outLenDecrypted;
    decryptAES(encrypted, outLenEncrypted, key, decrypted, outLenDecrypted, AESKeyLength::AES_256);
    ASSERT_FALSE(memcmp(plain, decrypted, 17));
    delete[] encrypted;
    delete[] decrypted;
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}