#include "gtest/gtest.h"
#include "../include/ecc.h"

// This test case checks the encryption and decryption functionality of the ECC class.
TEST(ECCTest, EncryptDecrypt) {
    // Create an ECC object
    ECC ecc;

    // The message to be encrypted
    std::string message = "hello";

    // Encrypt the message
    auto cipher = ecc.encrypt(message);

    // Decrypt the message
    auto decryptedMessage = ecc.decrypt(cipher);

    // Check if the decrypted message matches the original message
    EXPECT_EQ(message, decryptedMessage);
}

// The main function for running all the tests
int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
