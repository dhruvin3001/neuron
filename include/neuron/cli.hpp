#pragma once

#include "neuron/ai_client.hpp"
#include "neuron/config.hpp"
#include <string>

namespace neuron {

class CLI {
public:
    CLI(int argc, char** argv);

    int run();

private:
    int argc_;
    char** argv_;

    // Helper methods
    std::string join_args(int start_index) const;
    bool is_potentially_dangerous(const std::string& command) const;

    // Command handlers
    int handle_run(const std::string& command, const bool auto_execute = false);
    int handle_tell(const std::string& command);
};

} // namespace neuron
