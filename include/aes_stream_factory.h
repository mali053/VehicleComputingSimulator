#include "aes.h"
#include "aes_stream.h"
#include <map>
#include <string>

/**
 * @brief Abstract factory class for creating StreamAES objects.
 */
class StreamAESFactory 
{
   public:
     /**
      * @brief Creates a new StreamAES object.
      * 
      * @return A pointer to the newly created StreamAES object.
      */
     virtual StreamAES* create() const = 0;
};

/**
 * @brief Factory class for creating AESEcb objects.
 */
class AESEcbFactory : public StreamAESFactory 
{
   public:
     /**
      * @brief Creates a new AESEcb object.
      * 
      * @return A pointer to the newly created AESEcb object.
      */
     StreamAES* create() const override 
     {
        return new AESEcb();
     }
};

/**
 * @brief Factory class for creating AESCbc objects.
 */
class AESCbcFactory : public StreamAESFactory 
{
   public:
      /**
       * @brief Creates a new AESCbc object.
       * 
       * @return A pointer to the newly created AESCbc object.
       */
      StreamAES* create() const override 
      {
        return new AESCbc();
      }
};

/**
 * @brief Factory class for creating AESCfb objects.
 */
class AESCfbFactory : public StreamAESFactory 
{
   public:
      /**
       * @brief Creates a new AESCfb object.
       * 
       * @return A pointer to the newly created AESCfb object.
       */
      StreamAES* create() const override 
      {
        return new AESCfb();
      }
};

/**
 * @brief Factory class for creating AESOfb objects.
 */
class AESOfbFactory : public StreamAESFactory 
{
   public:
      /**
       * @brief Creates a new AESOfb object.
       * 
       * @return A pointer to the newly created AESOfb object.
       */
      StreamAES* create() const override 
      {
        return new AESOfb();
      }
};

/**
 * @brief Factory class for creating AESCtr objects.
 */
class AESCtrFactory : public StreamAESFactory 
{
   public:
      /**
       * @brief Creates a new AESCtr object.
       * 
       * @return A pointer to the newly created AESCtr object.
       */
      StreamAES* create() const override 
      {
        return new AESCtr();
      }
};

/**
 * @brief Singleton class for managing StreamAESFactory instances.
 */
class FactoryManager 
{
   public:
    /**
     * @brief Gets the singleton instance of FactoryManager.
     * 
     * @return The singleton instance of FactoryManager.
     */
    static FactoryManager& getInstance() 
    {
        static FactoryManager instance;
        return instance;
    }

    /**
     * @brief Creates a StreamAES object based on the specified AESChainingMode.
     * 
     * @param type The AES chaining mode.
     * @return A pointer to the newly created StreamAES object.
     */
    StreamAES* create(const AESChainingMode& type) const 
    {
        auto it = factories.find(type);
        if (it != factories.end()) 
            return it->second->create();
        
        return nullptr;
    }

   private:
    std::map<AESChainingMode, StreamAESFactory*> factories = 
    {
      {AESChainingMode::ECB, new AESEcbFactory()},
      {AESChainingMode::CBC, new AESCbcFactory()},
      {AESChainingMode::CFB, new AESCfbFactory()},
      {AESChainingMode::OFB, new AESOfbFactory()},
      {AESChainingMode::CTR, new AESCtrFactory()}
    };

    /**
     * @brief Private constructor for singleton pattern.
     */
    FactoryManager() {}
};
