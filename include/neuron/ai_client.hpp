#pragma once

#include "neuron/config.hpp"
#include <string>
#include <optional>

namespace neuron {

enum class Mode {
    RUN, // for executing the CLI commands
    TELL, // for explanation mode
};

class AIClient {
public:
    explicit AIClient(const Config& config);

    std::optional<std::string> run(const std::string& user_input, Mode mode);

private:
    std::string api_key_;
    std::string model_;
    std::string build_prompt(const std::string& input, Mode mode) const;
};

} // namespace neuron
