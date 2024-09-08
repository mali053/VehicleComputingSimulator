#include "../include/aes_stream.h"

void AESCbc::encryptStart(unsigned char block[], unsigned int inLen, unsigned char*& out, unsigned int& outLen,unsigned char* key, AESKeyLength keyLength)
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

void AESCbc::encryptContinue(unsigned char block[], unsigned int inLen, unsigned char*& out, unsigned int &outLen)
{
  encrypt(block, inLen, key,out, outLen, lastBlock, nullptr, keyLength);
}

void AESCbc::decryptStart(unsigned char block[], unsigned int inLen, unsigned char*& out, unsigned int &outLen,unsigned char* key, AESKeyLength keyLength)
{
  this-> iv = block + inLen - 16;
  decrypt(block,  inLen - 16, key, out, outLen, block + inLen - 16, nullptr, keyLength);
  this-> lastBlock = out;
}

void AESCbc::decryptContinue(unsigned char block[], unsigned int inLen, unsigned char*& out, unsigned int& outLen)
{
  decrypt(block,  inLen , key, out, outLen, lastBlock,nullptr, keyLength);
}

void AESCbc::encrypt(unsigned char in[], unsigned int inLen, unsigned char *key,
                    unsigned char *&out, unsigned int &outLen, const unsigned char *iv, unsigned char *lastData, AESKeyLength keyLength)
{
    padMessage(in, inLen, outLen);
    unsigned char block[BLOCK_BYTES_LEN];
    out = new unsigned char[outLen];
    unsigned char *roundKeys =
        new unsigned char[(aesKeyLengthData[keyLength].numRound + 1) * NUM_BLOCKS * 4];
    keyExpansion(key, roundKeys, keyLength);
    memcpy(block, iv, BLOCK_BYTES_LEN);
    for (unsigned int i = 0; i < outLen; i += BLOCK_BYTES_LEN) {
        xorBlocks(block, in + i, block, BLOCK_BYTES_LEN);
        encryptBlock(block, out + i, roundKeys, keyLength);
        memcpy(block, out + i, BLOCK_BYTES_LEN);
    }
    delete[] roundKeys;
}

void AESCbc::decrypt(unsigned char in[], unsigned int inLen, unsigned char *key,
             unsigned char *&out, unsigned int &outLen, const unsigned char *iv, unsigned char *lastData, AESKeyLength keyLength)
{
    checkLength(inLen);
    unsigned char block[BLOCK_BYTES_LEN];
    outLen = inLen;
    out = new unsigned char[outLen];
    unsigned char *roundKeys =
        new unsigned char[(aesKeyLengthData[keyLength].numRound + 1) * NUM_BLOCKS * 4];
    keyExpansion(key, roundKeys, keyLength);
    memcpy(block, iv, BLOCK_BYTES_LEN);
    for (unsigned int i = 0; i < outLen; i += BLOCK_BYTES_LEN) {
        decryptBlock(in + i, out + i, roundKeys, keyLength);
        xorBlocks(block, out + i, out + i, BLOCK_BYTES_LEN);
        memcpy(block, in + i, BLOCK_BYTES_LEN);
    }
    unpadMessage(out, outLen);
    delete[] roundKeys;
}