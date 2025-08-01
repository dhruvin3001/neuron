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

    std::string join_args(int start_index) const;
    int handle_run(const std::string& command);
    int handle_tell(const std::string& command);
};

} // namespace neuron
