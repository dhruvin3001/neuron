#include "neuron/cli.hpp"
#include <cxxopts.hpp>
#include <iostream>

namespace neuron {

CLI::CLI(int argc, char** argv)
    : argc_(argc), argv_(argv) {}

int CLI::run() {
    try {
        cxxopts::Options options("neuron", "AI-powered developer CLI assistant");

        options.add_options()
            ("h,help", "Print help")
            ("m,mode", "Mode: run or tell", cxxopts::value<std::string>())
            ("p,prompt", "Prompt to send", cxxopts::value<std::string>());

        auto result = options.parse(argc_, argv_);

        if (result.count("help") || !result.count("mode") || !result.count("prompt")) {
            std::cout << options.help() << std::endl;
            return 0;
        }

        std::string mode_str = result["mode"].as<std::string>();
        std::string prompt = result["prompt"].as<std::string>();

        if (mode_str == "run") {
            return handle_run(prompt);
        } else if (mode_str == "tell") {
            return handle_tell(prompt);
        } else {
            std::cerr << "Invalid mode: " << mode_str << std::endl;
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}

int CLI::handle_run(const std::string& prompt) {
    neuron::Config config;
    neuron::AIClient client(config);

    auto result = client.run(prompt, neuron::Mode::RUN);
    if (result.has_value()) {
        std::cout << result.value() << std::endl;
        return 0;
    } else {
        std::cerr << "Failed to get a response from the Neuron." << std::endl;
        return 1;
    }
}

int CLI::handle_tell(const std::string& prompt) {
    neuron::Config config;
    neuron::AIClient client(config);

    auto result = client.run(prompt, neuron::Mode::TELL);
    if (result.has_value()) {
        std::cout << result.value() << std::endl;
        return 0;
    } else {
        std::cerr << "Failed to get a response from the Neuron." << std::endl;
        return 1;
    }
}

} // namespace neuron
