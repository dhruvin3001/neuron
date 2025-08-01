#include "neuron/cli.hpp"
#include <cxxopts.hpp>
#include <iostream>

namespace neuron {

CLI::CLI(int argc, char** argv)
    : argc_(argc), argv_(argv) {}

int CLI::run() {
    if (argc_ >= 3 && std::string(argv_[1]) == "run") {
        // check for --yes or -y flag
        bool auto_execute = false;
        for (int i = 2; i < argc_; ++i) {
            if (std::string(argv_[i]) == "--yes" || std::string(argv_[i]) == "-y") {
                auto_execute = true;
                break;
            }
        }

        std::string command = join_args(2);
        return handle_run(command, auto_execute);
    }

    if (argc_ >= 3 && std::string(argv_[1]) == "tell") {
        std::string command = join_args(2);
        return handle_tell(command);
    }

    // fallback to original flag-based parsing
    try {
        cxxopts::Options options("neuron", "AI-powered developer CLI assistant");

        options.add_options()
            ("h,help", "Print help")
            ("m,mode", "Mode: run or tell", cxxopts::value<std::string>())
            ("p,prompt", "Prompt to send", cxxopts::value<std::string>())
            ("y,yes", "Auto execute the command without confirmation");

        auto result = options.parse(argc_, argv_);

        if (result.count("help") || !result.count("mode") || !result.count("prompt")) {
            std::cout << options.help() << std::endl;
            return 0;
        }

        std::string mode_str = result["mode"].as<std::string>();
        std::string prompt = result["prompt"].as<std::string>();
        bool auto_execute = result.count("yes") > 0;

        if (mode_str == "run") {
            return handle_run(prompt, auto_execute);
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

std::string CLI::join_args(int start_index) const {
    std::ostringstream ss;
    for (int i = start_index; i < argc_; ++i) {
        // skip --yes or -y flags
        if (std::string(argv_[i]) == "--yes" || std::string(argv_[i]) == "-y") {
            continue;
        }

        // add other arguments to the prompt
        if (i > start_index) {
            ss << " ";
        }
        ss << argv_[i];
    }
    return ss.str();
}

bool CLI::is_potentially_dangerous(const std::string& command) const {
    std::vector<std::string> dangerous_patterns = {
        "sudo", "rm -rf", "rm -r", "rm -f", "dd if=", "mkfs",
        "fdisk", "format", "del /", "rmdir /s", "> /dev/"
    };
    
    for (const auto& pattern : dangerous_patterns) {
        if (command.find(pattern) != std::string::npos) {
            return true;
        }
    }
    return false;
}

int CLI::handle_run(const std::string& prompt, const bool auto_execute) {
    neuron::Config config;
    neuron::AIClient client(config);

    auto result = client.run(prompt, neuron::Mode::RUN);
    if (!result) {
        std::cerr << "Failed to get a response from the Neuron." << std::endl;
        return 1;
    }

    std::string command = result.value();
    std::cout << "🧬 Neuron generated command:" << std::endl << std::endl;
    std::cout << "\033[1;36m" << command << "\033[0m" << std::endl << std::endl;
    
    // Check for potentially dangerous commands
    bool is_dangerous = is_potentially_dangerous(command);
    
    if (!auto_execute || is_dangerous) {
        if (is_dangerous) {
            std::cout << "\n⚠️  This command might be destructive or require elevated privileges.\n";
        }
        std::cout << "\nDo you want to execute this command? (y/n): ";
        std::string choice;
        std::getline(std::cin, choice);

        if (choice != "y" && choice != "yes" && choice != "Y" && choice != "YES") {
            std::cout << "Skipping command execution." << std::endl;
            return 0;
        }
    }

    std::cout << "🚀 Executing command..." << std::endl;
    int exit_code = std::system(command.c_str());

    if (exit_code != 0) {
        std::cerr << "Command execution failed with exit code: " << exit_code << std::endl;
    } else {
        std::cout << "Command executed successfully." << std::endl;
    }

    return 0;
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
