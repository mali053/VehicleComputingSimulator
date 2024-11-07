#include "../include/hash_factory.h"
#include <string>

logger factory_logger("hsm");

/**
 * @brief Gets the singleton instance of HashFactory.
 * 
 * This method ensures that only one instance of HashFactory exists during
 * the program's lifetime (Singleton Pattern).
 *
 * @return The singleton instance of HashFactory.
 */
HashFactory& HashFactory::getInstance()
{
    factory_logger.logMessage(logger::LogLevel::INFO, "HashFactory::getInstance() called");
    static HashFactory instance;
    return instance;
}

/**
 * @brief Creates an IHash object based on the specified SHAAlgorithm.
 *
 * This method creates a hash algorithm object depending on the specified
 * type (e.g., SHA256, SHA3-512).
 *
 * @param type The SHA algorithm type to create.
 * @param hashPtr A reference to a unique_ptr where the newly created IHash object will be stored.
 * @return CK_RV The return code indicating success or failure.
 */
CK_RV HashFactory::create(const IHash::SHAAlgorithm& type, std::unique_ptr<IHash>& hashPtr) const
{
    factory_logger.logMessage(logger::LogLevel::INFO, "HashFactory::create() called with SHAAlgorithm: " + std::to_string(static_cast<int>(type)));

    try {
        const auto it = factories.find(type);
        if (it != factories.end()) {
            hashPtr = it->second();
            return CKR_OK;  // Success
        } else {
            factory_logger.logMessage(logger::LogLevel::ERROR, "Error: Algorithm type not found in HashFactory.");
            return CKR_FUNCTION_FAILED;  // Error: Algorithm type not found
        }
    }
    catch (const std::exception& e) {
        factory_logger.logMessage(logger::LogLevel::ERROR, std::string("Exception caught in HashFactory::create: ") + e.what());
        return CKR_FUNCTION_FAILED;  // Error: Exception occurred
    }
}

/**
 * @brief Private constructor that initializes the hash algorithm factories.
 *
 * This constructor registers hash algorithms to the factory map. It is private
 * to ensure that only one instance of HashFactory can exist (Singleton Pattern).
 */
HashFactory::HashFactory()
    : factories({
        {IHash::SHAAlgorithm::SHA256, []() -> std::unique_ptr<IHash> { return std::make_unique<SHA256>(); }},
        {IHash::SHAAlgorithm::SHA3_512, []() -> std::unique_ptr<IHash> { return std::make_unique<SHA3_512>(); }}
    })
{
    factory_logger.logMessage(logger::LogLevel::INFO, "HashFactory constructor called, initializing hash algorithm factories.");
}