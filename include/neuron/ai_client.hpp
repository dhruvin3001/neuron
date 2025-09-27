#pragma once

#include "neuron/config.hpp"
#include <string>
#include <optional>

namespace neuron {

enum class Mode {
    RUN, // for executing the CLI commands
    TELL, // for explanation mode
};

struct Prompt {
    std::string system_message;
    std::string user_template;
};

class AIClient {
public:
    explicit AIClient(const Config& config);

    std::optional<std::string> run(const std::string& user_input, Mode mode);

private:
    std::string api_key_;
    std::string model_;
    std::string os_;

    Prompt build_prompt(const std::string& input, Mode mode) const;
};

} // namespace neuron
