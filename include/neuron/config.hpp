#pragma once

#include <string>
#include <optional>
#include <unordered_map>

namespace neuron {

class Config {
public:
    Config(); // Loads from .env or environment variables

    std::optional<std::string> getNeuronApiKey() const;

private:
    static constexpr const char* NEURON_API_KEY_ENV = "NEURON_API_KEY";

    std::unordered_map<std::string, std::string> _configMap;
    
    void load();
};

} // namespace neuron
