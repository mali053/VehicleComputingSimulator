#include "../include/aes.h"
#include <stdexcept>
#include <random>

/*
 Constructor for AES class
 Initializes the number of words (numWord) and number of rounds (numRound)
 based on the provided AES key length (128, 192, or 256 bits).
*/
AES::AES(const AESKeyLength keyLength) 
{
    switch (keyLength) {
     case AESKeyLength::AES_128:
       this->numWord = 4;
       this->numRound = 10;
       break;
     case AESKeyLength::AES_192:
       this->numWord = 6;
       this->numRound = 12;
       break;
     case AESKeyLength::AES_256:
       this->numWord = 8;
       this->numRound = 14;
       break;
  }
}

/*
 Generates a random AES key of the specified length
 `keyLength` - length of the key (128, 192, or 256 bits)
 Returns a pointer to the generated key.
*/
unsigned char* AES::generateKey(AESKeyLength keyLength)
{
    int keySize;
    switch (keyLength) {
    case AESKeyLength::AES_128:
        keySize = 16; 
        break;
    case AESKeyLength::AES_192:
        keySize = 24;
        break;
    case AESKeyLength::AES_256:
        keySize = 32; 
        break;
    default:
        throw std::invalid_argument("Invalid AES key length");
    }

    // Allocate memory for the key
    unsigned char* key = new unsigned char[keySize];

    // Initialize a random device and a random number generator
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<unsigned char> dis(0, 255);

    // Fill the key with random bytes
    for (int i = 0; i < keySize; i++) 
        key[i] = dis(gen);
    

    return key;
}

/*
 Encrypts the input data using the provided key.
 `in` - input data to be encrypted
 `inLen` - length of the input data
 `key` - encryption key
 Returns a pointer to the encrypted data.
*/
unsigned char* AES::encrypt(const unsigned char in[], unsigned int inLen, const unsigned char key[]) 
{
    checkLength(inLen);
    unsigned char* out = new unsigned char[inLen];
    unsigned char* roundKeys = new unsigned char[4 * numWord * (numRound + 1)];
    keyExpansion(key, roundKeys);
    for (unsigned int i = 0; i < inLen; i += blockBytesLen) 
        encryptBlock(in + i, out + i, roundKeys);
    
    delete[] roundKeys;
    return out;
}

/*
 Decrypts the input data using the provided key.
 `in` - input data to be decrypted
 `inLen` - length of the input data
 `key` - decryption key
 Returns a pointer to the decrypted data.
*/
unsigned char *AES::decrypt(const unsigned char in[], unsigned int inLen, const unsigned char key[]) 
{
    checkLength(inLen);
    unsigned char* out = new unsigned char[inLen];
    unsigned char* roundKeys = new unsigned char[4 * numBlocks * (numRound + 1)];
    keyExpansion(key, roundKeys);
    for (size_t i = 0; i < inLen; i += blockBytesLen) 
        decryptBlock(in + i, out+i, roundKeys);
    
    delete[] roundKeys;
    return out;
}

/*
 Checks if the input length is a multiple of the block length.
 Throws an exception if the length is invalid.
*/
void AES::checkLength(unsigned int len) 
{
  if (len % blockBytesLen != 0) 
    throw std::length_error("Plaintext length must be divisible by " +
                            std::to_string(blockBytesLen));
}

/*
 Encrypts a single block of data.
 `in` - input block to be encrypted
 `out` - output block to store the encrypted data
 `roundKeys` - expanded key for encryption
*/
void AES::encryptBlock(const unsigned char in[], unsigned char out[], unsigned char* roundKeys) 
{
    unsigned char state[4][numBlocks];
    unsigned int i, j, round;

    // Initialize state array with input block
    for (i = 0; i < 4; i++) 
        for (j = 0; j < numBlocks; j++) 
            state[i][j] = in[i + 4 * j];

    // Initial round key addition    
    addRoundKey(state, roundKeys);

    // Main rounds
    for (round = 1; round < numRound; round++) {
        subBytes(state);
        shiftRows(state);
        mixColumns(state);
        addRoundKey(state, roundKeys + round * 4 * numBlocks);
    }

    // Final round
    subBytes(state);
    shiftRows(state);
    addRoundKey(state, roundKeys + numRound * 4 * numBlocks);
    
    // Copy state array to output block
    for (i = 0; i < 4; i++) 
        for (j = 0; j < numBlocks; j++) 
            out[i + 4 * j] = state[i][j]; 
}

/*
 Decrypts a single block of data.
 `in` - input block to be decrypted
 `out` - output block to store the decrypted data
 `roundKeys` - expanded key for decryption
*/
void AES::decryptBlock(const unsigned char in[], unsigned char out[], unsigned char* roundKeys)
{
    unsigned char state[4][numBlocks];
    unsigned int i, j, round;

    // Initialize state array with input block
    for (i = 0; i < 4; i++)
        for (j = 0; j < numBlocks; j++)
            state[i][j] = in[i + 4 * j];
    
    // Initial round key addition
    addRoundKey(state, roundKeys + numRound * 4 * numBlocks);

    // Main rounds
    for (round = numRound - 1; round >= 1; round--) {
        invSubBytes(state);
        invShiftRows(state);
        addRoundKey(state, roundKeys + round * 4 * numBlocks);
        invMixColumns(state);
    }

    // Final round
    invSubBytes(state);
    invShiftRows(state);
    addRoundKey(state, roundKeys);

    // Copy state array to output block
    for (i = 0; i < 4; i++)
        for (j = 0; j < numBlocks; j++)
            out[i + 4 * j] = state[i][j];
}

/* Multiplies a byte by x in GF(2^8) */
unsigned char AES::xtime(unsigned char b)  
{
    return (b << 1) ^ (((b >> 7) & 1) * 0x1b);
}

/* Multiplies two bytes in GF(2^8) */
unsigned char AES::multiply(unsigned char x, unsigned char y) 
{
    unsigned char result = 0;
    unsigned char temp = y;
    for (int i = 0; i < 8; i++) {
        if (x & 1)
            result ^= temp;
        bool carry = temp & 0x80;
        temp <<= 1;
        if (carry)
            temp ^= 0x1b;
        x >>= 1;
    }
    return result;
}

/* Apply SubBytes transformation using the S-box */ 
void AES::subBytes(unsigned char state[4][numBlocks])
{
    for (size_t i = 0; i < 4; i++)
        for (size_t j = 0; j < numBlocks; j++)
            state[i][j] = sBox[state[i][j] / 16][state[i][j] % 16];
}


/* ShiftRows transformation */
void AES::shiftRows(unsigned char state[4][numBlocks]) 
{
  for (size_t i = 0; i < 4; i++) {
        unsigned char tmp[numBlocks];
        for (size_t k = 0; k < numBlocks; k++)
            tmp[k] = state[i][(k + i) % numBlocks];
        memcpy(state[i], tmp, numBlocks * sizeof(unsigned char));
    }
}

/* MixColumns transformation */
void AES::mixColumns(unsigned char state[4][numBlocks]) 
{
   for (size_t i = 0; i < 4; i++) {
        unsigned char a[4];
        unsigned char b[4];
        for (size_t j = 0; j < 4; j++) {
            a[j] = state[j][i];
            b[j] = xtime(state[j][i]);
        }
        state[0][i] = b[0] ^ a[1] ^ b[1] ^ a[2] ^ a[3];
        state[1][i] = a[0] ^ b[1] ^ a[2] ^ b[2] ^ a[3];
        state[2][i] = a[0] ^ a[1] ^ b[2] ^ a[3] ^ b[3];
        state[3][i] = b[0] ^ a[0] ^ a[1] ^ a[2] ^ b[3];
    }
}

/* AddRoundKey transformation */
void AES::addRoundKey(unsigned char state[4][numBlocks], unsigned char *key) 
{
    unsigned int i, j;
    for (i = 0; i < 4; i++)
        for (j = 0; j < numBlocks; j++)
            state[i][j] = state[i][j] ^ key[i + 4 * j];
}

/* Applies the SubWord transformation using the S-box */
void AES::subWord(unsigned char a[4]) 
{
   for (size_t  i = 0; i < 4; i++)
        a[i] = sBox[a[i]/16][a[i]%16];
}

/* Rotates a word (4 bytes) */
void AES::rotWord(unsigned char a[4]) 
{
  unsigned char first = a[0];
    for (size_t i = 0; i < 3; i++)
        a[i] = a[i + 1];
    a[3] = first;
}

/* Applies the Rcon transformation */
void AES::rconWord(unsigned char a[4], unsigned int n) 
{
    unsigned char strong = 1;
    for (size_t i = 0; i < n-1; i++)
        strong = xtime(strong);
    
    a[0] = strong;
    a[1] = a[2] = a[3] = 0;
}

/* Expands the key for AES encryption/decryption */
void AES::keyExpansion(const unsigned char* key, unsigned char w[])
{
    unsigned char temp[4], rcon[4];
    unsigned int i = 0;
    
    //copy key to w array
    while (i < 4 * numWord) {
      w[i] = key[i];
      i++;
    }

    i = 4 * numWord;

    //main expansion loop
    while (i < 4 * numBlocks * (numRound + 1)) {
        
        for (size_t k =4 , j = 0; k > 0; --k, ++j)
            temp[j] = w[i - k];

        if (i / 4 % numWord == 0) {
           rotWord(temp);
           subWord(temp);
           rconWord(rcon, i / (numWord * 4));
        
        for (int i = 0; i < 4; i++)
            temp[i] = temp[i] ^ rcon[i];   
        
        } 
        else if (numWord > 6 && i / 4 % numWord == 4) 
            subWord(temp);

        for (int k = 0; k < 4; k++) 
           w[i + k] = w[i + k - 4 * numWord] ^ temp[k];

        i += 4;
    }
}

/* Applies the InvSubBytes transformation using the inverse S-box */
void AES::invSubBytes(unsigned char state[4][numBlocks]) 
{
  unsigned int i, j;
  unsigned char t;
  for (i = 0; i < 4; i++) 
    for (j = 0; j < numBlocks; j++) {
      t = state[i][j];
      state[i][j] = invSBox[t / 16][t % 16];
    }
}

/* Applies the InvMixColumns transformation */
void AES::invMixColumns(unsigned char state[4][numBlocks])
{
    for (size_t i = 0; i < numBlocks; i++) {
        unsigned char a[4];
        unsigned char b[4];
        for (size_t j = 0; j < 4; j++) {
            a[j] = state[j][i];
            b[j] = xtime(a[j]);
        }
        state[0][i] = multiply(0x0e, a[0]) ^ multiply(0x0b, a[1]) ^ multiply(0x0d, a[2]) ^ multiply(0x09, a[3]);
        state[1][i] = multiply(0x09, a[0]) ^ multiply(0x0e, a[1]) ^ multiply(0x0b, a[2]) ^ multiply(0x0d, a[3]);
        state[2][i] = multiply(0x0d, a[0]) ^ multiply(0x09, a[1]) ^ multiply(0x0e, a[2]) ^ multiply(0x0b, a[3]);
        state[3][i] = multiply(0x0b, a[0]) ^ multiply(0x0d, a[1]) ^ multiply(0x09, a[2]) ^ multiply(0x0e, a[3]);
    }
}

/* Applies the InvShiftRows transformation */
void AES::invShiftRows(unsigned char state[4][numBlocks])
{
   for (size_t i = 0; i < 4; i++) {
        unsigned char tmp[numBlocks];
        for (size_t k = 0; k < numBlocks; k++)
            tmp[k] = state[i][(k - i + numBlocks) % numBlocks];
        memcpy(state[i], tmp, numBlocks * sizeof(unsigned char));
    }
}