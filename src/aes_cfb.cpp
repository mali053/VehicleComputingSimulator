#include "../include/aes_stream.h"
#include <cstring>
void AESCfb::encryptStart(unsigned char block[], unsigned int inLen,
                          unsigned char *&out, unsigned int &outLen,
                          unsigned char *key, AESKeyLength keyLength)
{
    generateRandomIV(iv);
    encrypt(block, inLen, key, out, outLen, iv, nullptr, keyLength);
    unsigned char *newOut = new unsigned char[outLen + BLOCK_BYTES_LEN];
    memcpy(newOut, out, outLen);
    memcpy(newOut + outLen, iv, BLOCK_BYTES_LEN);
    memcpy(lastBlock, out + outLen - BLOCK_BYTES_LEN, BLOCK_BYTES_LEN);
    out = newOut;
    this->key = new unsigned char[aesKeyLengthData[keyLength].keySize];
    // Copy the data from the provided key
    memcpy(this->key, key, aesKeyLengthData[keyLength].keySize);

    // Set the key length
    this->keyLength = keyLength;

    outLen += BLOCK_BYTES_LEN;
}

void AESCfb::encryptContinue(unsigned char block[], unsigned int inLen,
                             unsigned char *&out, unsigned int &outLen)
{
    encrypt(block, inLen, key, out, outLen, lastBlock, nullptr, keyLength);
    memcpy(lastBlock, out + outLen - BLOCK_BYTES_LEN, BLOCK_BYTES_LEN);
}

void AESCfb::decryptStart(unsigned char block[], unsigned int inLen,
                          unsigned char *&out, unsigned int &outLen,
                          unsigned char *key, AESKeyLength keyLength)
{
    this->iv = block + inLen - BLOCK_BYTES_LEN;
    decrypt(block, inLen - BLOCK_BYTES_LEN, key, out, outLen,
            block + inLen - BLOCK_BYTES_LEN, nullptr, keyLength);
    memcpy(lastBlock, block + inLen - 2*BLOCK_BYTES_LEN, BLOCK_BYTES_LEN);    
}

void AESCfb::decryptContinue(unsigned char block[], unsigned int inLen,
                             unsigned char *&out, unsigned int &outLen)
{
    decrypt(block, inLen, key, out, outLen, lastBlock, nullptr, keyLength);
    memcpy(lastBlock, block + inLen - BLOCK_BYTES_LEN, BLOCK_BYTES_LEN);

}
void AESCfb::encrypt(unsigned char in[], unsigned int inLen, unsigned char *key,
                     unsigned char *&out, unsigned int &outLen,
                     const unsigned char *iv, unsigned char *lastData,
                     AESKeyLength keyLength)
{
    padMessage(in, inLen, outLen);
    out = new unsigned char[outLen];
    unsigned char block[BLOCK_BYTES_LEN];
    unsigned char feedback[BLOCK_BYTES_LEN];
    unsigned char *roundKeys =
        new unsigned char[(aesKeyLengthData[keyLength].numRound + 1) *
                          NUM_BLOCKS * 4];
    keyExpansion(key, roundKeys, keyLength);
    memcpy(feedback, iv, BLOCK_BYTES_LEN);
    for (unsigned int i = 0; i < outLen; i += BLOCK_BYTES_LEN) {
        encryptBlock(feedback, block, roundKeys, keyLength);
        xorBlocks(in + i, block, out + i, BLOCK_BYTES_LEN);
        memcpy(feedback, out + i, BLOCK_BYTES_LEN);
    }
    delete[] roundKeys;
}

/**
 Decrypts data using AES in CFB mode. 
 @param in Encrypted input data.
 @param inLen Length of input data.
 @param key Decryption key.
 @param[out] out Decrypted output data.
 @param[out] outLen Length of decrypted data.
 @param iv Initialization vector.
 @param keyLength AES key length (128, 192, 256 bits).
 */
void AESCfb::decrypt(unsigned char in[], unsigned int inLen, unsigned char *key,
                     unsigned char *&out, unsigned int &outLen,
                     const unsigned char *iv, unsigned char *lastData,
                     AESKeyLength keyLength)
{
    checkLength(inLen);
    unsigned char block[BLOCK_BYTES_LEN];
    outLen = inLen;
    out = new unsigned char[outLen];
    unsigned char feedback[BLOCK_BYTES_LEN];
    unsigned char *roundKeys =
        new unsigned char[(aesKeyLengthData[keyLength].numRound + 1) *
                          NUM_BLOCKS * 4];
    keyExpansion(key, roundKeys, keyLength);
    memcpy(feedback, iv, BLOCK_BYTES_LEN);
    for (unsigned int i = 0; i < outLen; i += BLOCK_BYTES_LEN) {
        encryptBlock(feedback, block, roundKeys, keyLength);
        xorBlocks(in + i, block, out + i, BLOCK_BYTES_LEN);
        memcpy(feedback, in + i, BLOCK_BYTES_LEN);
    }
    unpadMessage(out, outLen);
    delete[] roundKeys;
}