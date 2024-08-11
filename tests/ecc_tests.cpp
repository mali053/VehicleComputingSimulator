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

// Test for ECC signature and verification
TEST(ECCTest, SignVerify)
{
    mpz_class privateKey = generatePrivateKey();
    Point publicKey = generatePublicKey(privateKey);
    // Sign the message
    std::vector<uint8_t> messageBytes = {0b01110100, 0b01100101, 0b01110011, 0b01110100, 0b00100000, 0b01101101, 0b01100101, 0b01110011, 0b01110011, 0b01100001, 0b01100111, 0b01100101};  
    auto signature = signMessageECC(messageBytes, privateKey);
    // Verify the signature
    bool isValid = verifySignatureECC(messageBytes, signature, publicKey);
    // Check if the signature is valid
    EXPECT_TRUE(isValid);
}

// The main function for running all the tests
int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}