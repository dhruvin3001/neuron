#pragma once

#include <string>
#include <optional>
#include <unordered_map>
#include <vector>

namespace neuron {

class Config {
public:
    Config(); // Loads from .env or environment variables

    std::optional<std::string> getNeuronApiKey() const;
    std::optional<std::string> getNeuronModel() const;
    
    // Setup methods
    bool setApiKey(const std::string& api_key);
    bool setModel(const std::string& model);
    bool saveToFile();
    
    // Available models
    static std::vector<std::string> getAvailableModels();

private:
    static constexpr const char* NEURON_API_KEY_ENV = "NEURON_API_KEY";
    static constexpr const char* NEURON_MODEL_ENV = "NEURON_MODEL";
    static constexpr const char* CONFIG_FILE = ".neuron_config";

    std::unordered_map<std::string, std::string> _configMap;
    
    void load();
    std::string getConfigFilePath() const;
};

} // namespace neuron
