#ifndef __RETURN_CODES_H__
#define __RETURN_CODES_H__

typedef unsigned long CK_RV;

/* Successful operation */
#define CKR_OK 0x00000000

/* General failure when a function could not complete its intended task */
#define CKR_FUNCTION_FAILED 0x00000006

/* Invalid arguments provided to the function (e.g., null pointers or invalid values) */
#define CKR_ARGUMENTS_BAD 0x00000007

/* Key size is out of the allowed range (e.g., key length is too short or too long) */
#define CKR_KEY_SIZE_RANGE 0x00000162

/* Buffer provided by the user is too small to hold the required data (e.g., during encryption or decryption) */
#define CKR_BUFFER_TOO_SMALL 0x00000150

/* The function attempted to generate a key, but key generation is not permitted or failed */
#define CKR_KEY_FUNCTION_NOT_PERMITTED 0x00000068

/* Decryption was attempted, but the decrypted data is invalid (e.g., data was corrupted) */
#define CKR_DECRYPTED_DATA_INVALID 0x00000064

/* Encrypted data has invalid padding (e.g., during decryption or when verifying padding) */
#define CKR_ENCRYPTED_DATA_INVALID 0x00000063

#endif  // __RETURN_CODES_H__