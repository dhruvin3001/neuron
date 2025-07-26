#include "neuron/config.hpp"

#include <iostream>

int main() {
    neuron::Config config;

    // Attempt to retrieve the api key
    auto apiKey = config.getNeuronApiKey();
    if (apiKey) {
        std::cout << "key: " << *apiKey << std::endl;
    } else {
        std::cout << "key not found." << std::endl;
    }

    return 0;
}
