#include "../include/ecc.h"
#include <gmpxx.h>
#include <gmp.h>
#include <iostream>
#include <string>
#include <bitset>

// Prime number for the elliptic curve 
std::string decimalString = "115792089237316195423570985008687907853269984665640564039457584007913129639936";
const mpz_class ECC::prime(decimalString);
const mpz_class ECC::a = 0;// Curve parameter a
const mpz_class ECC::b = 7;// Curve parameter b
// Curve basic point 
const Point ECC::basicPoint(mpz_class("55066263022277343669578718895168534326250603453777594175500187360389116729240"),
                     mpz_class("32670510020758816978083085130507043184471273380659243275938904335757337482424"));

/**
 * ECC constructor.
 */
ECC::ECC()  
{
    privateKey = generatePrivateKey();
    publicKey = generatePublicKey();
    k = generateK();
}

/**
 * Converts a message string to a point on the elliptic curve.
 * @param text The message to convert.
 * @return The point on the elliptic curve.
 */

Point ECC::convertMessageToPoint(const std::string& text)
{
    std::string binaryStr;
    for (char c : text)
        for (int i = 7; i >= 0; --i)
            binaryStr += ((c >> i) & 1) ? '1' : '0';

    mpz_class x;
    x.set_str(binaryStr, 2);
    for (;; added++) {
        mpz_class xAdded = x + mpz_class(added);
        mpz_class rhs = mod(xAdded * xAdded * xAdded + a * xAdded + b);
        mpz_class yAdded;
        if (modularSqrt(yAdded.get_mpz_t(), rhs.get_mpz_t(), prime.get_mpz_t()))
            return Point(xAdded, yAdded);
    }

    throw std::runtime_error(
        "Unable to convert message to a valid point on the curve.");
}

/**
 * Converts a point on the elliptic curve to a message string.
 * @param point The point to convert.
 * @return The message string.
 */
std::string ECC::convertPointToMessage(const Point& point)
{
    mpz_class x = point.x - mpz_class(added);
    std::string binaryStr = x.get_str(2);

    // Adding leading zeros to complete to a multiple of 8
    size_t paddingLength = 8 - (binaryStr.size() % 8);
    if (paddingLength != 8)  // If the string is not already a multiple of 8
        binaryStr = std::string(paddingLength, '0') + binaryStr;

    std::string text;
    for (size_t i = 0; i < binaryStr.size(); i += 8) {
        std::string byteStr = binaryStr.substr(i, 8);
        char c = static_cast<char>(std::bitset<8>(byteStr).to_ulong());
        text += c;
    }

    return text;
}

/**
 * Computes the modular square root using the Tonelli-Shanks algorithm.
 * @param result The computed square root.
 * @param a The value to find the square root of.
 * @param p The modulus.
 * @return True if the square root exists, false otherwise.
 */
bool ECC::modularSqrt(mpz_t result, const mpz_t a, const mpz_t p) 
{
    mpz_t q, s, z, m, c, t, r, b, temp;
    mpz_inits(q, s, z, m, c, t, r, b, temp, NULL);

    // Check if a is sqrt of 1 (x^2 ≡ a (mod p))
    if (mpz_legendre(a, p) != 1) {
        std::cout << "No solution exists." << std::endl;
        mpz_clears(q, s, z, m, c, t, r, b, temp, NULL);
        return false;
    }

    // If p ≡ 3 (mod 4), the solution is: a^((p+1)/4) mod p
    mpz_mod_ui(temp, p, 4);
    if (mpz_cmp_ui(temp, 3) == 0) {
        mpz_add_ui(q, p, 1);
        mpz_tdiv_q_ui(q, q, 4);
        mpz_powm(result, a, q, p);
        mpz_clears(q, s, z, m, c, t, r, b, temp, NULL);
        return true;
    }

    // For the general case, use the Tonley-Shanks method
    mpz_sub_ui(m, p, 1);
    mpz_tdiv_q_ui(m, m, 2);
    mpz_set_ui(s, 0);
    mpz_set_ui(z, 2);

    while (true) {
        mpz_powm(b, z, m, p);
        if (mpz_cmp_ui(b, 1) != 0)
            break;
        mpz_add_ui(s, s, 1);
        mpz_sub_ui(m, p, 1);
        mpz_tdiv_q_ui(m, m, 2);
        mpz_tdiv_q_ui(m, m, 2);
        mpz_add_ui(m, m, 1);
    }

    mpz_set_ui(c, 0);
    mpz_set(r, a);
    mpz_set_ui(t, 1);
    mpz_powm(t, t, m, p);

    while (true) {
        mpz_powm(b, t, c, p);
       if (mpz_cmp_ui(b, 1) == 0)
            break;
        mpz_set_ui(b, 0);
        mpz_powm(b, r, b, p);
        mpz_mul(r, r, b);
        mpz_mod(r, r, p);
        mpz_mul(t, t, b);
        mpz_mod(t, t, p);
        mpz_set_ui(c, 0);
        mpz_add_ui(c, c, 1);
    }

    mpz_set(result, r);

    // Clear memory
    mpz_clears(q, s, z, m, c, t, r, b, temp, NULL);
    return true;
}

/**
 * Generates a private key for ECC.
 * @return The generated private key.
 */
mpz_class ECC::generatePrivateKey()
{
    gmp_randclass rng(gmp_randinit_default);
    rng.seed(time(nullptr));
    return rng.get_z_range(prime - 1) + 1;
}

/**
 * Generates a random value k for ECC.
 * @return The generated value of k.
 */
mpz_class ECC::generateK()
{
    gmp_randclass rng(gmp_randinit_default);
    rng.seed(time(nullptr));
    return rng.get_z_range(prime - 1) + 1;
}

/**
 * Generates a public key for ECC.
 * @return The generated public key.
 */
Point ECC::generatePublicKey()
{
    return multiply(basicPoint, privateKey);
}

/**
 * Calculates the y-coordinate for a given x-coordinate on the elliptic curve.
 * @param x The x-coordinate.
 * @return The y-coordinate.
 */
mpz_class ECC::calculateY(mpz_class x)
{
    mpz_class rhs = mod(x * x * x + a * x + b);
    mpz_class y;
    if (modularSqrt(y.get_mpz_t(), rhs.get_mpz_t(), prime.get_mpz_t()))
        return y;
    else
        throw std::runtime_error("No solution for Y.");
}

/**
 * Checks if a point is on the elliptic curve.
 * @param P The point to check.
 * @return True if the point is on the curve, false otherwise.
 */
bool ECC::isOnCurve(Point P)
{
    return mod(P.y * P.y) == mod(P.x * P.x * P.x + a * P.x + b);
}

/**
 * Computes the modular reduction of a value by the curve's prime.
 * @param x The value to reduce.
 * @return The reduced value.
 */
mpz_class ECC::mod(mpz_class x)
{
    mpz_class result;
    mpz_mod(result.get_mpz_t(), x.get_mpz_t(), prime.get_mpz_t());
    return result;
}

/**
 * Computes the modular inverse of a value.
 * @param base The value to invert.
 * @return The modular inverse.
 */
mpz_class ECC::inverse(mpz_class base)
{
    mpz_class result;
    mpz_invert(result.get_mpz_t(), base.get_mpz_t(), prime.get_mpz_t());
    return result;
}

/**
 * Adds two points on the elliptic curve.
 * @param P The first point.
 * @param Q The second point.
 * @return The resulting point.
 */
Point ECC::add(Point P, Point Q)
{
    mpz_class incline;
    if (P.x == 0 && P.y == 0)
        return Q;

    if (Q.x == 0 && Q.y == 0)
        return P;

    if (P.x == Q.x && P.y == -Q.y)
        return Point(0, 0);

    if (P.x == Q.x && P.y == Q.y)
        incline = mod((3 * P.x * P.x + a) * inverse(2 * P.y));
    else
        incline = mod((Q.y - P.y) * inverse(Q.x - P.x));

    mpz_class x = mod(incline * incline - P.x - Q.x);
    mpz_class y = mod(incline * (P.x - x) - P.y);
    return Point(x, y);
}

/**
 * Multiplies a point by a scalar on the elliptic curve.
 * @param P The point to multiply.
 * @param times The scalar to multiply by.
 * @return The resulting point.
 */
Point ECC::multiply(Point P, mpz_class times)
{
    Point R(0, 0);
    Point N = P;
    while (times > 0) {
        if (times % 2 == 1)
            R = add(R, N);

        N = add(N, N);
        times /= 2;
    }

    return R;
}

/**
 * Encrypts a message using ECC.
 * @param message The message text to encrypt.
 * @return A pair of points representing the ciphertext.
 */
EncryptedMessage ECC::encrypt(std::string message)
{
    Point meesagePoint = convertMessageToPoint(message);
    Point C1 = multiply(basicPoint, k);
    Point C2 = add(meesagePoint, multiply(publicKey, k));
    return EncryptedMessage(C1.x, C1.y < 0, C2.x, C2.y < 0);
}

/**
 * Decrypts a ciphertext using ECC.
 * @param ciphertext The ciphertext to decrypt.
 * @return The decrypted message point.
 */
std::string ECC::decrypt(EncryptedMessage ciphertext)
{

    Point temp = multiply(Point(ciphertext.c1X, calculateY(ciphertext.c1X) *
                                                    (ciphertext.c1Y ? -1 : 1)),
                          privateKey);    Point negTemp = Point(temp.x, mod(-temp.y));
    Point decrypted = add(Point(ciphertext.c2X, calculateY(ciphertext.c2X) *
                                                    (ciphertext.c1Y ? -1 : 1)),
                          negTemp);
    return convertPointToMessage(decrypted);
}
