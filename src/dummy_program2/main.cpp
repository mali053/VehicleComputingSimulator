#include <iostream>
#include <thread>
#include <chrono>

int main() {
    for (int i = 0; i < 10; ++i) {
        std::cout << "Dummy Program 2 is running: " << i << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));  // Simulate work
    }
    return 0;
}
