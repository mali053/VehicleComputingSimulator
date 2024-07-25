#ifndef ECC_H_
#define ECC_H_
#include <string>
#include <gmpxx.h>

// Structure for representing a point
struct Point {
    mpz_class x;
    mpz_class y;
    Point() : x(0), y(0) {}
    Point(mpz_class x, mpz_class y) : x(x), y(y) {}
};

// Structure for representing an encrypted message
struct EncryptedMessage {
    mpz_class c1X;
    bool c1Y;
    mpz_class c2X;
    bool c2Y;
    EncryptedMessage(mpz_class c1X, bool c1Y, mpz_class c2X, bool c2Y)
        : c1X(c1X), c1Y(c1Y), c2X(c2X), c2Y(c2Y) {}
};

class ECC
{
  private:
    mpz_class privateKey;
    static const mpz_class prime;
    static const mpz_class a; 
    static const mpz_class b; 
    unsigned int added = 0;
    mpz_class generatePrivateKey(); 
    Point generatePublicKey();
  
  public:
    static const Point basicPoint;
    Point publicKey;
    mpz_class k;
    
    ECC();
    mpz_class calculateY(mpz_class x);
    Point convertMessageToPoint(const std::string& text) ;
    std::string convertPointToMessage(const Point& point);
    EncryptedMessage encrypt(std::string message);
    std::string decrypt(EncryptedMessage);
    mpz_class generateK(); 
    Point multiply(Point P, mpz_class times);
    Point add(Point P, Point Q);
    mpz_class mod(mpz_class x);
    bool isOnCurve(Point P);
    mpz_class inverse(mpz_class base);
    bool modularSqrt(mpz_t result, const mpz_t a, const mpz_t p);
};
#endif
