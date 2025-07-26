#include "neuron/config.hpp"

#include <iostream>

int main() {
    neuron::Config config;

    // Attempt to retrieve the NEURON_API_KEY
    auto apiKey = config.getNeuronApiKey();
    if (apiKey) {
        std::cout << "NEURON_API_KEY: " << *apiKey << std::endl;
    } else {
        std::cout << "NEURON_API_KEY not found." << std::endl;
    }

    return 0;
}
