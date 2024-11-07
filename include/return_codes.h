#ifndef __RETURN_CODES_H__
#define __RETURN_CODES_H__
typedef unsigned long CK_RV;

/* Successful operation */
constexpr CK_RV CKR_OK = 0x00000000;
/* General failure when a function could not complete its intended task */
constexpr CK_RV CKR_FUNCTION_FAILED = 0x00000006;
/* Invalid arguments provided to the function (e.g., null pointers or invalid values) */
constexpr CK_RV CKR_ARGUMENTS_BAD = 0x00000007;
/* Key size is out of the allowed range (e.g., key length is too short or too long) */
constexpr CK_RV CKR_KEY_SIZE_RANGE = 0x00000162;
/* Buffer provided by the user is too small to hold the required data (e.g., during encryption or decryption) */
constexpr CK_RV CKR_BUFFER_TOO_SMALL = 0x00000150;
/* The function attempted to generate a key, but key generation is not permitted or failed */
constexpr CK_RV CKR_KEY_FUNCTION_NOT_PERMITTED = 0x00000068;
/* Decryption was attempted, but the decrypted data is invalid (e.g., data was corrupted) */
constexpr CK_RV CKR_DECRYPTED_DATA_INVALID = 0x00000064;
/* Encrypted data has invalid padding (e.g., during decryption or when verifying padding) */
constexpr CK_RV CKR_ENCRYPTED_DATA_INVALID = 0x00000063;
/* Data provided for encryption is too large for the RSA key */
constexpr CK_RV CKR_DATA_TOO_LARGE = 0x00000080;

#endif  // __RETURN_CODES_H__