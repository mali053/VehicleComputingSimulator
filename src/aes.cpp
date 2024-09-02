#include "../include/aes.h"
#include <stdexcept>
#include <random>

unsigned int numWord;
unsigned int numRound;
unsigned int keySize;
AESKeyLength keyLength;

/*
 Constructor for AES class
 Initializes the number of words (numWord) and number of rounds (numRound)
 based on the provided AES key length (128, 192, or 256 bits).
*/
void init(const AESKeyLength keyLengthUser) 
{
    keyLength = keyLengthUser;
    switch (keyLength) {
        case AESKeyLength::AES_128:
            numWord = 4;
            numRound = 10;
            break;
        case AESKeyLength::AES_192:
            numWord = 6;
            numRound = 12;
            break;
        case AESKeyLength::AES_256:
            numWord = 8;
            numRound = 14;
            break;
    }
}

/*
Generates a random AES key of the specified length
`keyLength` - length of the key (128, 192, or 256 bits)
Returns a pointer to the generated key.
 */
unsigned char* generateKey(AESKeyLength keyLength)
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
    unsigned char *key = new unsigned char[keySize];

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
void encrypt(const unsigned char in[], unsigned int inLen, unsigned char* key, unsigned char* &out, unsigned int &outLen, const unsigned char *iv)
{
    checkLength(inLen);
    unsigned char block[BLOCK_BYTES_LEN];
    outLen = inLen;
    out = new unsigned char[outLen];
    unsigned char* roundKeys = new unsigned char[(numRound + 1) * NUM_BLOCKS * 4];
    keyExpansion(key, roundKeys);
    memcpy(block, iv, BLOCK_BYTES_LEN);
    for (unsigned int i = 0; i < outLen; i += BLOCK_BYTES_LEN){
       xorBlocks(block, in + i, block, BLOCK_BYTES_LEN);
       encryptBlock(block, out + i, roundKeys);
       memcpy(block, out + i, BLOCK_BYTES_LEN);
    }
    
    delete[] roundKeys;
}

/*
 Decrypts the input data using the provided key.
 `in` - input data to be decrypted
 `inLen` - length of the input data
`key` - decryption key
*/
void decrypt(const unsigned char in[], unsigned int inLen, unsigned char* key, unsigned char* &out, unsigned int &outLen, const unsigned char *iv)
{
    checkLength(inLen);
    unsigned char block[BLOCK_BYTES_LEN];
    outLen = inLen;
    out = new unsigned char[outLen];
    unsigned char* roundKeys = new unsigned char[(numRound + 1) * NUM_BLOCKS * 4];
    keyExpansion(key, roundKeys);
    memcpy(block, iv, BLOCK_BYTES_LEN);

    for (unsigned int i = 0; i < outLen; i += BLOCK_BYTES_LEN){
        decryptBlock(in + i, out + i, roundKeys);
        xorBlocks(block, out + i, out + i, BLOCK_BYTES_LEN);
        memcpy(block, in + i, BLOCK_BYTES_LEN);
    }

    delete[] roundKeys;
}

/*
 Checks if the input length is a multiple of the block length.
 Throws an exception if the length is invalid.
*/
void checkLength(unsigned int len) 
{
  if (len % BLOCK_BYTES_LEN != 0) 
    throw std::length_error("Plaintext length must be divisible by " +
                            std::to_string(BLOCK_BYTES_LEN));
}

/*
 Encrypts a single block of data.
 `in` - input block to be encrypted
 `out` - output block to store the encrypted data
 `roundKeys` - expanded key for encryption
*/
void encryptBlock(const unsigned char in[],unsigned char out[], unsigned char* roundKeys) 
{
    unsigned char state[4][NUM_BLOCKS];
    unsigned int i, j, round;

    // Initialize state array with input block
    for (i = 0; i < 4; i++) 
        for (j = 0; j < NUM_BLOCKS; j++) 
            state[i][j] = in[i + 4 * j];

    // Initial round key addition
    addRoundKey(state, roundKeys);

    // Main rounds
    for (round = 1; round < numRound; round++) {
        subBytes(state);
        shiftRows(state);
        mixColumns(state);
        addRoundKey(state, roundKeys + round * 4 * NUM_BLOCKS);
    }

    // Final round
    subBytes(state);
    shiftRows(state);
    addRoundKey(state, roundKeys + numRound * 4 * NUM_BLOCKS);
    
    // Copy state array to output block
    for (i = 0; i < 4; i++) 
        for (j = 0; j < NUM_BLOCKS; j++) 
            out[i + 4 * j] = state[i][j]; 
}

/*
 Decrypts a single block of data.
 `in` - input block to be decrypted
 `out` - output block to store the decrypted data
 `roundKeys` - expanded key for decryption
*/
void decryptBlock(const unsigned char in[], unsigned char out[], unsigned char* roundKeys)
{
    unsigned char state[4][NUM_BLOCKS];
    unsigned int i, j, round;

    // Initialize state array with input block
    for (i = 0; i < 4; i++)
        for (j = 0; j < NUM_BLOCKS; j++)
            state[i][j] = in[i + 4 * j];

    // Initial round key addition
    addRoundKey(state, roundKeys + numRound * 4 * NUM_BLOCKS);

    // Main rounds
    for (round = numRound - 1; round >= 1; round--) {
        invSubBytes(state);
        invShiftRows(state);
        addRoundKey(state, roundKeys + round * 4 * NUM_BLOCKS);
        invMixColumns(state);
    }

    // Final round
    invSubBytes(state);
    invShiftRows(state);
    addRoundKey(state, roundKeys);

    // Copy state array to output block
    for (i = 0; i < 4; i++)
        for (j = 0; j < NUM_BLOCKS; j++)
            out[i + 4 * j] = state[i][j];
}

/* Multiplies a byte by x in GF(2^8) */
unsigned char xtime(unsigned char b)  
{
    return (b << 1) ^ (((b >> 7) & 1) * 0x1b);
}

/* Multiplies two bytes in GF(2^8) */
unsigned char multiply(unsigned char x, unsigned char y) 
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
void subBytes(unsigned char state[4][NUM_BLOCKS])
{
    for (size_t i = 0; i < 4; i++)
        for (size_t j = 0; j < NUM_BLOCKS; j++)
            state[i][j] = sBox[state[i][j] / 16][state[i][j] % 16];
}

/* ShiftRows transformation */
void shiftRows(unsigned char state[4][NUM_BLOCKS]) 
{
  for (size_t i = 0; i < 4; i++) {
        unsigned char tmp[NUM_BLOCKS];
        for (size_t k = 0; k < NUM_BLOCKS; k++)
            tmp[k] = state[i][(k + i) % NUM_BLOCKS];
        memcpy(state[i], tmp, NUM_BLOCKS * sizeof(unsigned char));
    }
}

/* MixColumns transformation */
void mixColumns(unsigned char state[4][NUM_BLOCKS]) 
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
void addRoundKey(unsigned char state[4][NUM_BLOCKS], unsigned char *key) 
{
    unsigned int i, j;
    for (i = 0; i < 4; i++)
        for (j = 0; j < NUM_BLOCKS; j++)
            state[i][j] = state[i][j] ^ key[i + 4 * j];
}

/* Applies the SubWord transformation using the S-box */
void subWord(unsigned char a[4]) 
{
    for (size_t i = 0; i < 4; i++)
        a[i] = sBox[a[i] / 16][a[i] % 16];
}

/* Rotates a word (4 bytes) */
void rotWord(unsigned char a[4]) 
{
    unsigned char first = a[0];
    for (size_t i = 0; i < 3; i++)
        a[i] = a[i + 1];
    a[3] = first;
}

/* Applies the Rcon transformation */
void rconWord(unsigned char a[4], unsigned int n) 
{
    unsigned char strong = 1;
    for (size_t i = 0; i < n - 1; i++)
        strong = xtime(strong);

    a[0] = strong;
    a[1] = a[2] = a[3] = 0;
}

/* Expands the key for AES encryption/decryption */
void keyExpansion(const unsigned char* key, unsigned char w[])
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
    while (i < 4 * NUM_BLOCKS * (numRound + 1)) {
        
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
void invSubBytes(unsigned char state[4][NUM_BLOCKS]) 
{
  unsigned int i, j;
  unsigned char t;
  for (i = 0; i < 4; i++) 
    for (j = 0; j < NUM_BLOCKS; j++) {
      t = state[i][j];
      state[i][j] = invSBox[t / 16][t % 16];
    }
}

/* Applies the InvMixColumns transformation */
void invMixColumns(unsigned char state[4][NUM_BLOCKS])
{
    for (size_t i = 0; i < NUM_BLOCKS; i++) {
        unsigned char a[4];
        unsigned char b[4];
        for (size_t j = 0; j < 4; j++) {
            a[j] = state[j][i];
            b[j] = xtime(a[j]);
        }
        state[0][i] = multiply(0x0e, a[0]) ^ multiply(0x0b, a[1]) ^
                      multiply(0x0d, a[2]) ^ multiply(0x09, a[3]);
        state[1][i] = multiply(0x09, a[0]) ^ multiply(0x0e, a[1]) ^
                      multiply(0x0b, a[2]) ^ multiply(0x0d, a[3]);
        state[2][i] = multiply(0x0d, a[0]) ^ multiply(0x09, a[1]) ^
                      multiply(0x0e, a[2]) ^ multiply(0x0b, a[3]);
        state[3][i] = multiply(0x0b, a[0]) ^ multiply(0x0d, a[1]) ^
                      multiply(0x09, a[2]) ^ multiply(0x0e, a[3]);
    }
}

/* Applies the InvShiftRows transformation */
void invShiftRows(unsigned char state[4][NUM_BLOCKS])
{
   for (size_t i = 0; i < 4; i++) {
        unsigned char tmp[NUM_BLOCKS];
        for (size_t k = 0; k < NUM_BLOCKS; k++)
            tmp[k] = state[i][(k - i + NUM_BLOCKS) % NUM_BLOCKS];
        memcpy(state[i], tmp, NUM_BLOCKS * sizeof(unsigned char));
    }
}

/** XORs two blocks of bytes a and b of length len, storing the result in c. */
void xorBlocks(const unsigned char *a, const unsigned char *b,
                    unsigned char *c, unsigned int len) 
                    {
  for (unsigned int i = 0; i < len; i++) {
    c[i] = a[i] ^ b[i];
  }
}