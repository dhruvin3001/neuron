#include "neuron/config.hpp"
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <sstream>

namespace neuron {

Config::Config() {
    load();
}

void Config::load() {
    // Load from .env file first
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
    
    // Load from user config file
    std::ifstream configFile(getConfigFilePath());
    if (configFile.is_open()) {
        std::string line;
        while (std::getline(configFile, line)) {
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
    if (apiKey && strlen(apiKey) > 0) {
        return std::optional<std::string>(apiKey);
    }

    // If not found, check the config map
    auto it = _configMap.find(NEURON_API_KEY_ENV);
    if (it != _configMap.end() && !it->second.empty()) {
        return std::optional<std::string>(it->second);
    }

    return std::nullopt;
}

std::optional<std::string> Config::getNeuronModel() const {
    // Get the model from the environment variables first
    const char* model = std::getenv(NEURON_MODEL_ENV);
    if (model && strlen(model) > 0) {
        return std::optional<std::string>(model);
    }

    // If not found, check the config map
    auto it = _configMap.find(NEURON_MODEL_ENV);
    if (it != _configMap.end() && !it->second.empty()) {
        return std::optional<std::string>(it->second);
    }

    return std::nullopt;
}

bool Config::setApiKey(const std::string& api_key) {
    _configMap[NEURON_API_KEY_ENV] = api_key;
    return saveToFile();
}

bool Config::setModel(const std::string& model) {
    _configMap[NEURON_MODEL_ENV] = model;
    return saveToFile();
}

bool Config::saveToFile() {
    std::ofstream configFile(getConfigFilePath());
    if (!configFile.is_open()) {
        return false;
    }

    for (const auto& pair : _configMap) {
        configFile << pair.first << "=" << pair.second << std::endl;
    }

    return true;
}

std::vector<std::string> Config::getAvailableModels() {
    return {
        "openai/gpt-4.1",
    };
}

std::string Config::getConfigFilePath() const {
    const char* home = std::getenv("HOME");
    if (home) {
        return std::string(home) + "/" + CONFIG_FILE;
    }
    return CONFIG_FILE;
}

} // namespace neuron
