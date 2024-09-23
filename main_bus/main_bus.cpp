#include "../communication/include/bus_manager.h"
int main()
{
    std::vector<uint32_t> ids;
    uint32_t limit;
    BusManager* manager = BusManager::getInstance(ids, limit);
    manager->startConnection();
    while(true);
    return 0;
}