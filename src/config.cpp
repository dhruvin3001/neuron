#include "neuron/config.hpp"
#include <cstdlib>
#include <fstream>
#include <sstream>

namespace neuron {

Config::Config() {
    load();
}

void Config::load() {
    std::ifstream envFile(".env");
    if (envFile.is_open()) {
        std::string line;
        while (std::getline(envFile, line)) {
            std::size_t delimiterPos = line.find('=');
            if (delimiterPos != std::string::npos) {
                std::string key = line.substr(0, delimiterPos);
                std::string value = line.substr(delimiterPos + 1);
                _configMap[key] = value;
            }
        }
    }
}

std::optional<std::string> Config::getNeuronApiKey() const {
    // Get the API key from the environment variables first
    const char* apiKey = std::getenv(NEURON_API_KEY_ENV);
    if (apiKey) {
        return std::optional<std::string>(apiKey);
    }

    // If not found, check the config map
    auto it = _configMap.find(NEURON_API_KEY_ENV);
    if (it != _configMap.end()) {
        return std::optional<std::string>(it->second);
    }

    return std::nullopt;
}

} // namespace neuron
