#include "neuron/ai_client.hpp"

#include <iostream>

int main() {
    neuron::Config config;
    neuron::AIClient client(config);

    auto result = client.run("how to create a zip file", neuron::Mode::RUN);

    if (result) {
        std::cout << "Neuron: " << *result << std::endl;
    } else {
        std::cerr << "Failed to get a response from the Neuron." << std::endl;
    }

    return 0;
}
