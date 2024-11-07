
#include "../include/aes_stream.h"

void AESOfb::encryptStart(unsigned char block[], unsigned int inLen,
                          unsigned char *&out, unsigned int &outLen,
                          unsigned char *key, AESKeyLength keyLength)
{
    unsigned char *lastData = new unsigned char[BLOCK_BYTES_LEN];
    generateRandomIV(iv);
    memcpy(lastData, iv, BLOCK_BYTES_LEN);
    encrypt(block, inLen, key, out, outLen, iv, lastData, keyLength);
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
    this->lastData = lastData;
    outLen += BLOCK_BYTES_LEN;
}

void AESOfb::encryptContinue(unsigned char block[], unsigned int inLen,
                             unsigned char *&out, unsigned int &outLen)
{
    encrypt(block, inLen, key, out, outLen, lastBlock, lastData, keyLength);
    memcpy(lastBlock, out + outLen - BLOCK_BYTES_LEN, BLOCK_BYTES_LEN);
}

void AESOfb::decryptStart(unsigned char block[], unsigned int inLen,
                          unsigned char *&out, unsigned int &outLen,
                          unsigned char *key, AESKeyLength keyLength)
{
    unsigned char *lastData = new unsigned char[BLOCK_BYTES_LEN];
    memcpy(lastData, iv, BLOCK_BYTES_LEN);
    this->iv = block + inLen - BLOCK_BYTES_LEN;
    decrypt(block, inLen - BLOCK_BYTES_LEN, key, out, outLen,
            block + inLen - BLOCK_BYTES_LEN, lastData, keyLength);
    memcpy(lastBlock, block + inLen - 2*BLOCK_BYTES_LEN, BLOCK_BYTES_LEN);
    this->lastData = lastData;
}

void AESOfb::decryptContinue(unsigned char block[], unsigned int inLen,
                             unsigned char *&out, unsigned int &outLen)
{
    decrypt(block, inLen, key, out, outLen, lastBlock, lastData, keyLength);
    memcpy(lastBlock, block + inLen - BLOCK_BYTES_LEN, BLOCK_BYTES_LEN);

}

void AESOfb::encrypt(unsigned char in[], unsigned int inLen, unsigned char *key,
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
    memcpy(feedback, lastData, BLOCK_BYTES_LEN);
    for (unsigned int i = 0; i < outLen; i += BLOCK_BYTES_LEN) {
        encryptBlock(feedback, block, roundKeys, keyLength);
        xorBlocks(in + i, block, out + i, BLOCK_BYTES_LEN);
        for (unsigned int j = 0; j < BLOCK_BYTES_LEN; ++j)
            out[i + j] = in[i + j] ^ block[j];
        memcpy(feedback, block, BLOCK_BYTES_LEN);
        memcpy(lastData, feedback, BLOCK_BYTES_LEN);
    }
    delete[] roundKeys;
}

void AESOfb::decrypt(unsigned char in[], unsigned int inLen, unsigned char *key,
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
    memcpy(feedback, lastData, BLOCK_BYTES_LEN);
    for (unsigned int i = 0; i < outLen; i += BLOCK_BYTES_LEN) {
        encryptBlock(feedback, block, roundKeys, keyLength);
        xorBlocks(in + i, block, out + i, BLOCK_BYTES_LEN);
        memcpy(feedback, block, BLOCK_BYTES_LEN);
        memcpy(lastData, feedback, BLOCK_BYTES_LEN);
    }
    unpadMessage(out, outLen);
    delete[] roundKeys;
}