#include "../include/aes_stream.h"

void AESEcb::encryptStart(unsigned char block[], unsigned int inLen, unsigned char*& out, unsigned int& outLen,unsigned char* key, AESKeyLength keyLength) 
{
    encrypt(block, inLen, key, out, outLen, nullptr,nullptr, keyLength);
    this -> key = key;
    this -> keyLength = keyLength;
}

void AESEcb::encryptContinue(unsigned char block[], unsigned int inLen, unsigned char*& out, unsigned int &outLen)
{
    encrypt(block, inLen, key,out, outLen, nullptr, nullptr, keyLength);
}

void AESEcb::decryptStart(unsigned char block[], unsigned int inLen, unsigned char*& out, unsigned int &outLen,unsigned char* key, AESKeyLength keyLength)
{
  decrypt(block,  inLen , key, out, outLen, nullptr, nullptr, keyLength);
}

void AESEcb::decryptContinue(unsigned char block[], unsigned int inLen, unsigned char*& out, unsigned int& outLen)
{
  decrypt(block, inLen, key, out, outLen, nullptr, nullptr, keyLength);
}

void AESEcb::encrypt(unsigned char in[], unsigned int inLen, unsigned char *key,
                unsigned char *&out, unsigned int &outLen,const unsigned char *iv, unsigned char *lastData, AESKeyLength keyLength) 
{
    padMessage(in, inLen, outLen);
    unsigned char block[BLOCK_BYTES_LEN];
    out = new unsigned char[outLen];
    unsigned char *roundKeys = new unsigned char[(aesKeyLengthData[keyLength].numRound + 1) * NUM_BLOCKS * 4];
    keyExpansion(key, roundKeys, keyLength);
    for (unsigned int i = 0; i < outLen; i += BLOCK_BYTES_LEN) {
        memcpy(block, in + i, BLOCK_BYTES_LEN);
        encryptBlock(block, out + i, roundKeys, keyLength);
    }
    delete[] roundKeys;
}

void AESEcb::decrypt(unsigned char in[], unsigned int inLen, unsigned char *key,
                unsigned char *&out, unsigned int &outLen,const unsigned char *iv, unsigned char *lastData,AESKeyLength keyLength) 
{
    checkLength(inLen);
    outLen = inLen;
    out = new unsigned char[outLen];
    unsigned char *roundKeys = new unsigned char[(aesKeyLengthData[keyLength].numRound + 1) * NUM_BLOCKS * 4];
    keyExpansion(key, roundKeys, keyLength);
    for (unsigned int i = 0; i < outLen; i += BLOCK_BYTES_LEN) 
        decryptBlock(in + i, out + i, roundKeys, keyLength);
    unpadMessage(out, outLen);
    delete[] roundKeys;
}