   #include "../include/aes_stream_factory.h"
  FactoryManager FactoryManager::instance;
  
  FactoryManager& FactoryManager::getInstance() 
  {
        return instance;
  }

  StreamAES* FactoryManager::create(const AESChainingMode& type) const 
  {
    auto it = factories.find(type);
        if (it != factories.end()) 
            return it->second;
        
        return nullptr;
    }