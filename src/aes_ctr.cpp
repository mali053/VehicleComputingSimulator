#include "../include/aes_stream.h"

void AESCtr::encryptStart(unsigned char block[], unsigned int inLen, unsigned char*& out, unsigned int& outLen,unsigned char* key, AESKeyLength keyLength)
{
    unsigned char* lastData = new unsigned char[BLOCK_BYTES_LEN];
    generateRandomIV(iv);
    memcpy(lastData, iv, BLOCK_BYTES_LEN);
    encrypt(block, inLen, key, out, outLen, iv, lastData, keyLength);
    unsigned char *newOut = new unsigned char[outLen + 16];
    memcpy(newOut, out, outLen);
    memcpy(newOut + outLen, iv, 16);
    out = newOut;
    this -> lastBlock  = out;
    this -> key = key;
    this -> keyLength = keyLength;
    this-> lastData = lastData;
    outLen += 16;
}

void AESCtr::encryptContinue(unsigned char block[], unsigned int inLen, unsigned char*& out, unsigned int &outLen)
{
    encrypt(block, inLen, key,out, outLen, lastBlock, lastData, keyLength);
}

void AESCtr::decryptStart(unsigned char block[], unsigned int inLen, unsigned char*& out, unsigned int &outLen,unsigned char* key, AESKeyLength keyLength)
{
  unsigned char* lastData = new unsigned char[BLOCK_BYTES_LEN];
  memcpy(lastData, iv, BLOCK_BYTES_LEN);
  this-> iv = block + inLen - 16;
  decrypt(block,  inLen - 16, key, out, outLen, block + inLen - 16, lastData, keyLength);
  this-> lastBlock = out;
  this->lastData = lastData;
}

void AESCtr::decryptContinue(unsigned char block[], unsigned int inLen, unsigned char*& out, unsigned int& outLen)
{
  decrypt(block,  inLen  , key, out, outLen, lastBlock, lastData, keyLength);
}

void AESCtr::encrypt(unsigned char in[], unsigned int inLen, unsigned char *key,
                unsigned char *&out, unsigned int &outLen, const unsigned char *iv, unsigned char *lastData, AESKeyLength keyLength) 
{
    padMessage(in, inLen, outLen);
    unsigned char block[BLOCK_BYTES_LEN];
    out = new unsigned char[outLen];
    unsigned char counter[BLOCK_BYTES_LEN];
    memcpy(counter, lastData, BLOCK_BYTES_LEN);
    unsigned char *roundKeys = new unsigned char[(aesKeyLengthData[keyLength].numRound + 1) * NUM_BLOCKS * 4];
    keyExpansion(key, roundKeys, keyLength);
    memcpy(counter, lastData, BLOCK_BYTES_LEN);
    for (unsigned int i = 0; i < outLen; i += BLOCK_BYTES_LEN) {
        encryptBlock(counter, block, roundKeys, keyLength);
        xorBlocks(in + i, block, out + i, BLOCK_BYTES_LEN);
        for (int j = BLOCK_BYTES_LEN - 1; j >= 0; --j) 
            if (++counter[j]) break; 
        memcpy(lastData, counter,BLOCK_BYTES_LEN);
    }
    delete[] roundKeys;
}

void AESCtr::decrypt(unsigned char in[], unsigned int inLen, unsigned char *key,
                unsigned char *&out, unsigned int &outLen, const unsigned char *iv, unsigned char *lastData, AESKeyLength keyLength) 
{
    checkLength(inLen);
    unsigned char block[BLOCK_BYTES_LEN];
    outLen = inLen;
    out = new unsigned char[outLen];
    unsigned char counter[BLOCK_BYTES_LEN];
    unsigned char *roundKeys = new unsigned char[(aesKeyLengthData[keyLength].numRound + 1) * NUM_BLOCKS * 4];
    keyExpansion(key, roundKeys, keyLength);
    memcpy(counter, lastData, BLOCK_BYTES_LEN);
    for (unsigned int i = 0; i < outLen; i += BLOCK_BYTES_LEN) {
        encryptBlock(counter, block, roundKeys, keyLength);         
        xorBlocks(in + i, block, out + i, BLOCK_BYTES_LEN);
        for (int j = BLOCK_BYTES_LEN - 1; j >= 0; --j) 
            if (++counter[j]) break;

        memcpy(lastData, counter,BLOCK_BYTES_LEN);
    }
    unpadMessage(out, outLen);
    delete[] roundKeys;
}