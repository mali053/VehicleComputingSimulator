#ifndef FACTORY_MANAGER_H
#define FACTORY_MANAGER_H
#include "sha256.h"
#include "SHA3-512.h"
#include "IHash.h"
#include "return_codes.h"
#include <functional>
#include <map>
#include <memory>

class HashFactory
{
public:
    static HashFactory& getInstance();
    CK_RV create(const IHash::SHAAlgorithm& type, std::unique_ptr<IHash>& hashPtr) const;

private:
    std::map<IHash::SHAAlgorithm, std::function<std::unique_ptr<IHash>()>> factories;
    HashFactory(); //Private constructor for singleton pattern.
};

#endif // FACTORY_MANAGER_H