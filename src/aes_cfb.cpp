#include "../include/aes_stream.h"

void AESCfb::encryptStart(unsigned char block[], unsigned int inLen, unsigned char*& out, unsigned int& outLen,unsigned char* key, AESKeyLength keyLength)
{
    generateRandomIV(iv);
    encrypt(block, inLen, key, out, outLen, iv,nullptr, keyLength);
    unsigned char *newOut = new unsigned char[outLen + 16];
    memcpy(newOut, out, outLen);
    memcpy(newOut + outLen, iv, 16);
    out = newOut;
    this -> lastBlock  = out;
    this -> key = key;
    this -> keyLength = keyLength;

    outLen += 16;
}

void AESCfb::encryptContinue(unsigned char block[], unsigned int inLen, unsigned char*& out, unsigned int &outLen)
{
  encrypt(block, inLen, key,out, outLen, lastBlock, nullptr, keyLength);
}

void AESCfb::decryptStart(unsigned char block[], unsigned int inLen, unsigned char*& out, unsigned int &outLen,unsigned char* key, AESKeyLength keyLength)
{
  this-> iv = block + inLen - 16;
  decrypt(block,  inLen - 16, key, out, outLen, block + inLen - 16, nullptr, keyLength);
  this-> lastBlock = out;
}

void AESCfb::decryptContinue(unsigned char block[], unsigned int inLen, unsigned char*& out, unsigned int& outLen)
{
  decrypt(block,  inLen , key, out, outLen, lastBlock,nullptr, keyLength);
}

void AESCfb::encrypt(unsigned char in[], unsigned int inLen, unsigned char *key,
                unsigned char *&out, unsigned int &outLen, const unsigned char *iv, unsigned char *lastData, AESKeyLength keyLength) 
{
    padMessage(in, inLen, outLen);
    unsigned char block[BLOCK_BYTES_LEN];
    out = new unsigned char[outLen];
    unsigned char feedback[BLOCK_BYTES_LEN];
    unsigned char *roundKeys = new unsigned char[(aesKeyLengthData[keyLength].numRound + 1) * NUM_BLOCKS * 4];
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
                unsigned char *&out, unsigned int &outLen, const unsigned char *iv, unsigned char *lastData, AESKeyLength keyLength) 
{
    checkLength(inLen);
    unsigned char block[BLOCK_BYTES_LEN];
    outLen = inLen;
    out = new unsigned char[outLen];
    unsigned char feedback[BLOCK_BYTES_LEN];
    unsigned char *roundKeys = new unsigned char[(aesKeyLengthData[keyLength].numRound + 1) * NUM_BLOCKS * 4];
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