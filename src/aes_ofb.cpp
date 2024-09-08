#include "../include/aes_stream.h"

void AESOfb::encryptStart(unsigned char block[], unsigned int inLen, unsigned char*& out, unsigned int& outLen,unsigned char* key, AESKeyLength keyLength) 
{
    unsigned char* lastData = new unsigned char[BLOCK_BYTES_LEN];
    generateRandomIV(iv);
    memcpy(lastData, iv, BLOCK_BYTES_LEN);
    encrypt(block, inLen, key, out, outLen, iv,lastData, keyLength);
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

void AESOfb::encryptContinue(unsigned char block[], unsigned int inLen, unsigned char*& out, unsigned int &outLen)
{
    encrypt(block, inLen, key,out, outLen, lastBlock, lastData, keyLength);
}

void AESOfb::decryptStart(unsigned char block[], unsigned int inLen, unsigned char*& out, unsigned int &outLen,unsigned char* key, AESKeyLength keyLength)
{
  unsigned char* lastData = new unsigned char[BLOCK_BYTES_LEN];
  memcpy(lastData, iv, BLOCK_BYTES_LEN);
  this-> iv = block + inLen - 16;
  decrypt(block,  inLen - 16, key, out, outLen, block + inLen - 16, lastData, keyLength);
  this-> lastBlock = out;
  this->lastData = lastData;
}

void AESOfb::decryptContinue(unsigned char block[], unsigned int inLen, unsigned char*& out, unsigned int& outLen)
{
  decrypt(block,  inLen , key, out, outLen, lastBlock, lastData, keyLength);
}

void AESOfb::encrypt(unsigned char in[], unsigned int inLen, unsigned char *key,
                unsigned char *&out, unsigned int &outLen, const unsigned char *iv, unsigned char *lastData, AESKeyLength keyLength) 
{
    padMessage(in, inLen, outLen);
    unsigned char block[BLOCK_BYTES_LEN];
    out = new unsigned char[outLen];
    unsigned char feedback[BLOCK_BYTES_LEN];
    unsigned char *roundKeys = new unsigned char[(aesKeyLengthData[keyLength].numRound + 1) * NUM_BLOCKS * 4];
    keyExpansion(key, roundKeys, keyLength);
    if(lastData)
        memcpy(feedback, lastData, BLOCK_BYTES_LEN);
    else
       memcpy(feedback, iv, BLOCK_BYTES_LEN);
    for (unsigned int i = 0; i < outLen; i += BLOCK_BYTES_LEN) {
        encryptBlock(feedback, block, roundKeys, keyLength);
        for (unsigned int j = 0; j < BLOCK_BYTES_LEN; ++j) 
            out[i + j] = in[i + j] ^ block[j];
        memcpy(feedback, block, BLOCK_BYTES_LEN);
        memcpy(lastData, feedback, BLOCK_BYTES_LEN);
    }
    delete[] roundKeys;
}

void AESOfb::decrypt(unsigned char in[], unsigned int inLen, unsigned char *key,
                unsigned char *&out, unsigned int &outLen, const unsigned char *iv, unsigned char *lastData, AESKeyLength keyLength) 
{
    checkLength(inLen);
    unsigned char block[BLOCK_BYTES_LEN];
    outLen = inLen;
    out = new unsigned char[outLen];
    unsigned char feedback[BLOCK_BYTES_LEN];
    unsigned char *roundKeys = new unsigned char[(aesKeyLengthData[keyLength].numRound + 1) * NUM_BLOCKS * 4];
    keyExpansion(key, roundKeys, keyLength);
    if(lastData)
        memcpy(feedback, lastData, BLOCK_BYTES_LEN);
    else
       memcpy(feedback, iv, BLOCK_BYTES_LEN);
    for (unsigned int i = 0; i < outLen; i += BLOCK_BYTES_LEN) {
        encryptBlock(feedback, block, roundKeys, keyLength);
        // Copy only the amount of data needed for the current block
        unsigned int blockLen = (i + BLOCK_BYTES_LEN <= outLen) ? BLOCK_BYTES_LEN : (outLen - i);
        for (unsigned int j = 0; j < blockLen; ++j) 
            out[i + j] = in[i + j] ^ block[j];
        // Update feedback to be used for the next block
        memcpy(feedback, block, BLOCK_BYTES_LEN);
        memcpy(lastData, feedback, BLOCK_BYTES_LEN);

    }
    unpadMessage(out, outLen);
    delete[] roundKeys;
}