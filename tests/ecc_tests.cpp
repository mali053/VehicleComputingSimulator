#include "gtest/gtest.h"
#include "../include/ecc.h"

// This test case checks the encryption and decryption functionality of the ECC class.
TEST(ECCTest, EncryptDecrypt)
{
    mpz_class privateKey = generatePrivateKey();
    Point publicKey = generatePublicKey(privateKey);
    std::vector<uint8_t> messageBytes = {0b01101000, 0b01100101, 0b01101100, 0b01101100, 0b01101111};  // 'hello'

    // Encrypt the message
    auto cipher = encryptECC(messageBytes, publicKey);

    // Decrypt the message
   auto decryptedMessage = decryptECC(cipher, privateKey);

    // Check if the decrypted message matches the original message
    EXPECT_EQ(messageBytes, decryptedMessage);
}