#include <cstring>
#include "gtest/gtest.h"
#include "../include/aes.h"
const unsigned int BLOCK_BYTES_LENGTH = 16 * sizeof(unsigned char);

TEST(KeyLengths, KeyLength128_CBC)
{
    unsigned char plain[BLOCK_BYTES_LENGTH] = {
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
        0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};

    unsigned char *key = generateKey(AESKeyLength::AES_128);
    unsigned char *encrypted;
    unsigned int outLenEncrypted;
    encryptAES(plain, BLOCK_BYTES_LENGTH, key, encrypted, outLenEncrypted, AESKeyLength::AES_128,AESChainingMode::CBC);
    unsigned char *decrypted;
    unsigned int outLenDecrypted;
    decryptAES(encrypted, outLenEncrypted, key, decrypted, outLenDecrypted, AESKeyLength::AES_128,AESChainingMode::CBC);
    ASSERT_FALSE(memcmp(plain, decrypted, BLOCK_BYTES_LENGTH));
    delete[] encrypted;
    delete[] decrypted;
}

TEST(KeyLengths, KeyLength192_CBC)
{
    unsigned char plain[BLOCK_BYTES_LENGTH] = {
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
        0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
   
    unsigned char *key = generateKey(AESKeyLength::AES_192);
    unsigned char *encrypted;
    unsigned int outLenEncrypted;
    encryptAES(plain, BLOCK_BYTES_LENGTH, key, encrypted, outLenEncrypted, AESKeyLength::AES_192,AESChainingMode::CBC);
    unsigned char *decrypted;
    unsigned int outLenDecrypted;
    decryptAES(encrypted, outLenEncrypted, key, decrypted, outLenDecrypted, AESKeyLength::AES_192,AESChainingMode::CBC);
    ASSERT_FALSE(memcmp(plain, decrypted, BLOCK_BYTES_LENGTH));
    delete[] encrypted;
    delete[] decrypted;
}

TEST(KeyLengths, KeyLength256_CBC)
{
    unsigned char plain[BLOCK_BYTES_LENGTH] = {
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
        0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};

    unsigned char *key = generateKey(AESKeyLength::AES_256);
    unsigned char *encrypted;
    unsigned int outLenEncrypted;
    encryptAES(plain, BLOCK_BYTES_LENGTH, key, encrypted, outLenEncrypted, AESKeyLength::AES_256,AESChainingMode::CBC);
    unsigned char *decrypted;
    unsigned int outLenDecrypted;
    decryptAES(encrypted, outLenEncrypted, key, decrypted, outLenDecrypted, AESKeyLength::AES_256,AESChainingMode::CBC);
    ASSERT_FALSE(memcmp(plain, decrypted, BLOCK_BYTES_LENGTH));
    delete[] encrypted;
    delete[] decrypted;
}

TEST(CBC, EncryptDecryptOneBlock_CBC)
{
    unsigned char plain[18] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55,
                               0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb,
                               0xcc, 0xdd, 0xee, 0xff, 0xef, 0x03};
   
    unsigned char *key = generateKey(AESKeyLength::AES_256);
    unsigned char *encrypted;
    unsigned int outLenEncrypted;
    encryptAES(plain, 17, key, encrypted, outLenEncrypted, AESKeyLength::AES_256,AESChainingMode::CBC);
    unsigned char *decrypted;
    unsigned int outLenDecrypted;
    decryptAES(encrypted, outLenEncrypted, key, decrypted, outLenDecrypted, AESKeyLength::AES_256,AESChainingMode::CBC);
    ASSERT_FALSE(memcmp(plain, decrypted, 17));
    delete[] encrypted;
    delete[] decrypted;
}

TEST(KeyLengths, KeyLength128_CTR)
{
    unsigned char plain[BLOCK_BYTES_LENGTH] = {
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
        0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};

    unsigned char *key = generateKey(AESKeyLength::AES_128);
    unsigned char *encrypted;
    unsigned int outLenEncrypted;
    encryptAES(plain, BLOCK_BYTES_LENGTH, key, encrypted, outLenEncrypted, AESKeyLength::AES_128, AESChainingMode::CTR);
    unsigned char *decrypted;
    unsigned int outLenDecrypted;
    decryptAES(encrypted, outLenEncrypted, key, decrypted, outLenDecrypted, AESKeyLength::AES_128, AESChainingMode::CTR);
    ASSERT_FALSE(memcmp(plain, decrypted, BLOCK_BYTES_LENGTH));
    delete[] encrypted;
    delete[] decrypted;
}

TEST(KeyLengths, KeyLength192_CTR)
{
    unsigned char plain[BLOCK_BYTES_LENGTH] = {
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
        0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};

    unsigned char *key = generateKey(AESKeyLength::AES_192);
    unsigned char *encrypted;
    unsigned int outLenEncrypted;
    encryptAES(plain, BLOCK_BYTES_LENGTH, key, encrypted, outLenEncrypted, AESKeyLength::AES_192, AESChainingMode::CTR);
    unsigned char *decrypted;
    unsigned int outLenDecrypted;
    decryptAES(encrypted, outLenEncrypted, key, decrypted, outLenDecrypted, AESKeyLength::AES_192, AESChainingMode::CTR);
    ASSERT_FALSE(memcmp(plain, decrypted, BLOCK_BYTES_LENGTH));
    delete[] encrypted;
    delete[] decrypted;
}

TEST(KeyLengths, KeyLength256_CTR)
{
    unsigned char plain[BLOCK_BYTES_LENGTH] = {
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
        0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};

    unsigned char *key = generateKey(AESKeyLength::AES_256);
    unsigned char *encrypted;
    unsigned int outLenEncrypted;
    encryptAES(plain, BLOCK_BYTES_LENGTH, key, encrypted, outLenEncrypted, AESKeyLength::AES_256, AESChainingMode::CTR);
    unsigned char *decrypted;
    unsigned int outLenDecrypted;
    decryptAES(encrypted, outLenEncrypted, key, decrypted, outLenDecrypted, AESKeyLength::AES_256, AESChainingMode::CTR);
    ASSERT_FALSE(memcmp(plain, decrypted, BLOCK_BYTES_LENGTH));
    delete[] encrypted;
    delete[] decrypted;
}

TEST(CTR, EncryptDecryptOneBlock_CTR)
{
    unsigned char plain[18] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55,
                               0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb,
                               0xcc, 0xdd, 0xee, 0xff, 0xef, 0x03};

    unsigned char *key = generateKey(AESKeyLength::AES_256);
    unsigned char *encrypted;
    unsigned int outLenEncrypted;
    encryptAES(plain, 17, key, encrypted, outLenEncrypted, AESKeyLength::AES_256, AESChainingMode::CTR);
    unsigned char *decrypted;
    unsigned int outLenDecrypted;
    decryptAES(encrypted, outLenEncrypted, key, decrypted, outLenDecrypted, AESKeyLength::AES_256, AESChainingMode::CTR);
    ASSERT_FALSE(memcmp(plain, decrypted, 17));
    delete[] encrypted;
    delete[] decrypted;
}

TEST(KeyLengths, KeyLength128_ECB)
{
    unsigned char plain[BLOCK_BYTES_LENGTH] = {
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
        0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};

    unsigned char *key = generateKey(AESKeyLength::AES_128);
    unsigned char *encrypted;
    unsigned int outLenEncrypted;
    encryptAES(plain, BLOCK_BYTES_LENGTH, key, encrypted, outLenEncrypted, AESKeyLength::AES_128, AESChainingMode::ECB);
    unsigned char *decrypted;
    unsigned int outLenDecrypted;
    decryptAES(encrypted, outLenEncrypted, key, decrypted, outLenDecrypted, AESKeyLength::AES_128, AESChainingMode::ECB);
    ASSERT_FALSE(memcmp(plain, decrypted, BLOCK_BYTES_LENGTH));
    delete[] encrypted;
    delete[] decrypted;
}

TEST(KeyLengths, KeyLength192_ECB)
{
    unsigned char plain[BLOCK_BYTES_LENGTH] = {
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
        0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
   
    unsigned char *key = generateKey(AESKeyLength::AES_192);
    unsigned char *encrypted;
    unsigned int outLenEncrypted;
    encryptAES(plain, BLOCK_BYTES_LENGTH, key, encrypted, outLenEncrypted, AESKeyLength::AES_192, AESChainingMode::ECB);
    unsigned char *decrypted;
    unsigned int outLenDecrypted;
    decryptAES(encrypted, outLenEncrypted, key, decrypted, outLenDecrypted, AESKeyLength::AES_192, AESChainingMode::ECB);
    ASSERT_FALSE(memcmp(plain, decrypted, BLOCK_BYTES_LENGTH));
    delete[] encrypted;
    delete[] decrypted;
}

TEST(KeyLengths, KeyLength256_ECB)
{
    unsigned char plain[BLOCK_BYTES_LENGTH] = {
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
        0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};

    unsigned char *key = generateKey(AESKeyLength::AES_256);
    unsigned char *encrypted;
    unsigned int outLenEncrypted;
    encryptAES(plain, BLOCK_BYTES_LENGTH, key, encrypted, outLenEncrypted, AESKeyLength::AES_256, AESChainingMode::ECB);
    unsigned char *decrypted;
    unsigned int outLenDecrypted;
    decryptAES(encrypted, outLenEncrypted, key, decrypted, outLenDecrypted, AESKeyLength::AES_256, AESChainingMode::ECB);
    ASSERT_FALSE(memcmp(plain, decrypted, BLOCK_BYTES_LENGTH));
    delete[] encrypted;
    delete[] decrypted;
}

TEST(ECB, EncryptDecryptOneBlock_ECB)
{
    unsigned char plain[18] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55,
                               0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb,
                               0xcc, 0xdd, 0xee, 0xff, 0xef, 0x03};
   
    unsigned char *key = generateKey(AESKeyLength::AES_256);
    unsigned char *encrypted;
    unsigned int outLenEncrypted;
    encryptAES(plain, 17, key, encrypted, outLenEncrypted, AESKeyLength::AES_256, AESChainingMode::ECB);
    unsigned char *decrypted;
    unsigned int outLenDecrypted;
    decryptAES(encrypted, outLenEncrypted, key, decrypted, outLenDecrypted, AESKeyLength::AES_256, AESChainingMode::ECB);
    ASSERT_FALSE(memcmp(plain, decrypted, 17));
    delete[] encrypted;
    delete[] decrypted;
}

TEST(KeyLengths, KeyLength128_CFB) {
    unsigned char plain[BLOCK_BYTES_LENGTH] = {
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
        0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};

    unsigned char* key = generateKey(AESKeyLength::AES_128);
    unsigned char* encrypted;
    unsigned int outLenEncrypted;
    encryptAES(plain, BLOCK_BYTES_LENGTH, key, encrypted, outLenEncrypted, AESKeyLength::AES_128, AESChainingMode::CFB);
    unsigned char* decrypted;
    unsigned int outLenDecrypted;
    decryptAES(encrypted, outLenEncrypted, key, decrypted, outLenDecrypted, AESKeyLength::AES_128, AESChainingMode::CFB);
    ASSERT_FALSE(memcmp(plain, decrypted, BLOCK_BYTES_LENGTH));
    delete[] encrypted;
    delete[] decrypted;
    delete[] key;
}

TEST(KeyLengths, KeyLength192_CFB) {
    unsigned char plain[BLOCK_BYTES_LENGTH] = {
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
        0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
   
    unsigned char* key = generateKey(AESKeyLength::AES_192);
    unsigned char* encrypted;
    unsigned int outLenEncrypted;
    encryptAES(plain, BLOCK_BYTES_LENGTH, key, encrypted, outLenEncrypted, AESKeyLength::AES_192, AESChainingMode::CFB);
    unsigned char* decrypted;
    unsigned int outLenDecrypted;
    decryptAES(encrypted, outLenEncrypted, key, decrypted, outLenDecrypted, AESKeyLength::AES_192, AESChainingMode::CFB);
    ASSERT_FALSE(memcmp(plain, decrypted, BLOCK_BYTES_LENGTH));
    delete[] encrypted;
    delete[] decrypted;
    delete[] key;
}

TEST(KeyLengths, KeyLength256_CFB) {
    unsigned char plain[BLOCK_BYTES_LENGTH] = {
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
        0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};

    unsigned char* key = generateKey(AESKeyLength::AES_256);
    unsigned char* encrypted;
    unsigned int outLenEncrypted;
    encryptAES(plain, BLOCK_BYTES_LENGTH, key, encrypted, outLenEncrypted, AESKeyLength::AES_256, AESChainingMode::CFB);
    unsigned char* decrypted;
    unsigned int outLenDecrypted;
    decryptAES(encrypted, outLenEncrypted, key, decrypted, outLenDecrypted, AESKeyLength::AES_256, AESChainingMode::CFB);
    ASSERT_FALSE(memcmp(plain, decrypted, BLOCK_BYTES_LENGTH));
    delete[] encrypted;
    delete[] decrypted;
    delete[] key;
}

TEST(CFB, EncryptDecryptOneBlock) {
    unsigned char plain[18] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55,
                               0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb,
                               0xcc, 0xdd, 0xee, 0xff, 0xef, 0x03};
   
    unsigned char* key = generateKey(AESKeyLength::AES_256);
    unsigned char* encrypted;
    unsigned int outLenEncrypted;
    encryptAES(plain, 17, key, encrypted, outLenEncrypted, AESKeyLength::AES_256, AESChainingMode::CFB);
    unsigned char* decrypted;
    unsigned int outLenDecrypted;
    decryptAES(encrypted, outLenEncrypted, key, decrypted, outLenDecrypted, AESKeyLength::AES_256, AESChainingMode::CFB);
    ASSERT_FALSE(memcmp(plain, decrypted, 17));
    delete[] encrypted;
    delete[] decrypted;
    delete[] key;
}

TEST( KeyLengths, KeyLength128_OFB) {
    unsigned char plain[BLOCK_BYTES_LENGTH] = {
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
        0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};

    unsigned char* key = generateKey(AESKeyLength::AES_128);
    unsigned char* encrypted;
    unsigned int outLenEncrypted;
    encryptAES(plain, BLOCK_BYTES_LENGTH, key, encrypted, outLenEncrypted, AESKeyLength::AES_128, AESChainingMode::OFB);
    unsigned char* decrypted;
    unsigned int outLenDecrypted;
    decryptAES(encrypted, outLenEncrypted, key, decrypted, outLenDecrypted, AESKeyLength::AES_128, AESChainingMode::OFB);
    ASSERT_FALSE(memcmp(plain, decrypted, BLOCK_BYTES_LENGTH));
    delete[] encrypted;
    delete[] decrypted;
    delete[] key;
}

TEST(KeyLengths, KeyLength192_OFB) {
    unsigned char plain[BLOCK_BYTES_LENGTH] = {
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
        0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
   
    unsigned char* key = generateKey(AESKeyLength::AES_192);
    unsigned char* encrypted;
    unsigned int outLenEncrypted;
    encryptAES(plain, BLOCK_BYTES_LENGTH, key, encrypted, outLenEncrypted, AESKeyLength::AES_192, AESChainingMode::OFB);
    unsigned char* decrypted;
    unsigned int outLenDecrypted;
    decryptAES(encrypted, outLenEncrypted, key, decrypted, outLenDecrypted, AESKeyLength::AES_192, AESChainingMode::OFB);
    ASSERT_FALSE(memcmp(plain, decrypted, BLOCK_BYTES_LENGTH));
    delete[] encrypted;
    delete[] decrypted;
    delete[] key;
}

TEST( KeyLengths, KeyLength256_OFB) {
    unsigned char plain[BLOCK_BYTES_LENGTH] = {
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
        0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};

    unsigned char* key = generateKey(AESKeyLength::AES_256);
    unsigned char* encrypted;
    unsigned int outLenEncrypted;
    encryptAES(plain, BLOCK_BYTES_LENGTH, key, encrypted, outLenEncrypted, AESKeyLength::AES_256, AESChainingMode::OFB);
    unsigned char* decrypted;
    unsigned int outLenDecrypted;
    decryptAES(encrypted, outLenEncrypted, key, decrypted, outLenDecrypted, AESKeyLength::AES_256, AESChainingMode::OFB);
    ASSERT_FALSE(memcmp(plain, decrypted, BLOCK_BYTES_LENGTH));
    delete[] encrypted;
    delete[] decrypted;
    delete[] key;
}

TEST( OFB, EncryptDecryptOneBlock) {
    unsigned char plain[18] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55,
                               0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb,
                               0xcc, 0xdd, 0xee, 0xff, 0xef, 0x03};
   
    unsigned char* key = generateKey(AESKeyLength::AES_256);
    unsigned char* encrypted;
    unsigned int outLenEncrypted;
    encryptAES(plain, 17, key, encrypted, outLenEncrypted, AESKeyLength::AES_256, AESChainingMode::OFB);
    unsigned char* decrypted;
    unsigned int outLenDecrypted;
    decryptAES(encrypted, outLenEncrypted, key, decrypted, outLenDecrypted, AESKeyLength::AES_256, AESChainingMode::OFB);
    ASSERT_FALSE(memcmp(plain, decrypted, 17));
    delete[] encrypted;
    delete[] decrypted;
    delete[] key;
}
int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}