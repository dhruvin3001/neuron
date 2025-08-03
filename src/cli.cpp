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
            std::cout << "\n\033[1;35m🧬 Neuron AI\033[0m - Your intelligent command-line assistant\n" << std::endl;
            std::cout << "\033[1;32mUsage:\033[0m" << std::endl;
            std::cout << "  \033[1;36mneuron run\033[0m \"find large files\"          \033[2;37m# Generate & execute commands\033[0m" << std::endl;
            std::cout << "  \033[1;36mneuron run\033[0m \"install docker\" \033[1;33m--yes\033[0m     \033[2;37m# Auto-execute without confirmation\033[0m" << std::endl;
            std::cout << "  \033[1;36mneuron tell\033[0m \"explain git rebase\"       \033[2;37m# Get explanations\033[0m" << std::endl;
            std::cout << std::endl;
            std::cout << "\033[1;32mLegacy flag syntax:\033[0m" << std::endl;
            std::cout << options.help() << std::endl;
            std::cout << "\n\033[1;33m💡 Tip:\033[0m Set your API key: \033[2;37mexport NEURON_API_KEY=your_key_here\033[0m" << std::endl;
            std::cout << "\n\033[1;32mQuick examples to try:\033[0m" << std::endl;
            std::cout << "  \033[2;37mneuron run \"show disk usage\"\033[0m" << std::endl;
            std::cout << "  \033[2;37mneuron run \"find files modified today\"\033[0m" << std::endl;
            std::cout << "  \033[2;37mneuron tell \"what is docker\"\033[0m\n" << std::endl;
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

    std::cout << "\n\033[1;35m🧬 Neuron AI\033[0m \033[2;37mis generating your command...\033[0m" << std::endl;

    auto result = client.run(prompt, neuron::Mode::RUN);
    if (!result) {
        std::cout << "\n\033[1;31m❌ Failed to get response from Neuron AI\033[0m" << std::endl;
        std::cout << "\033[2;37m💡 Possible issues:\033[0m" << std::endl;
        std::cout << "\033[2;37m   • Check your internet connection\033[0m" << std::endl;
        std::cout << "\033[2;37m   • Verify your API key is set: export NEURON_API_KEY=your_key\033[0m" << std::endl;
        std::cout << "\033[2;37m   • Try again in a moment (API might be busy)\033[0m" << std::endl;
        return 1;
    }

    std::string command = result.value();
    
    // Enhanced output with better formatting
    std::cout << "\n\033[1;35m🧬 Neuron AI\033[0m \033[1;32mgenerated command:\033[0m\n" << std::endl;
    std::cout << "\033[1;36m" << command << "\033[0m" << std::endl << std::endl;
    
    // Show command breakdown if it's complex
    if (command.find('|') != std::string::npos || command.find("&&") != std::string::npos) {
        std::cout << "\n\033[2;37m💡 This command chains multiple operations\033[0m" << std::endl;
    }
    
    // Check for potentially dangerous commands
    bool is_dangerous = is_potentially_dangerous(command);
    
    if (!auto_execute || is_dangerous) {
        if (is_dangerous) {
            std::cout << "\n\033[1;31m⚠️  WARNING:\033[0m This command might be destructive or require elevated privileges." << std::endl;
            std::cout << "\033[2;37m   Please review carefully before proceeding.\033[0m\n" << std::endl;
        }
        
        std::cout << "\033[1;33mExecute this command?\033[0m \033[2;37m[y/N/e(xplain)]\033[0m: ";
        std::string choice;
        std::getline(std::cin, choice);

        if (choice == "e" || choice == "explain") {
            std::cout << "\n\033[1;34m📚 Command Explanation:\033[0m" << std::endl;
            // Try to get explanation from AI
            neuron::AIClient explain_client(config);
            auto explanation = explain_client.run("Explain this command: " + command, neuron::Mode::TELL);
            if (explanation) {
                std::cout << explanation.value() << std::endl;
            } else {
                std::cout << "Unable to get explanation at this time." << std::endl;
            }
            std::cout << "\n\033[1;33mStill want to execute?\033[0m \033[2;37m[y/N]\033[0m: ";
            std::getline(std::cin, choice);
        }

        if (choice != "y" && choice != "yes" && choice != "Y" && choice != "YES") {
            std::cout << "\n\033[2;37m🚫 Command execution cancelled.\033[0m" << std::endl;
            return 0;
        }
    }

    std::cout << "\n\033[1;32m🚀 Executing...\033[0m" << std::endl;
    std::cout << "\033[2;37m" << std::string(50, '-') << "\033[0m" << std::endl;
    
    int exit_code = std::system(command.c_str());
    
    std::cout << "\033[2;37m" << std::string(50, '-') << "\033[0m" << std::endl;

    if (exit_code != 0) {
        std::cout << "\033[1;31m❌ Command failed\033[0m \033[2;37m(exit code: " << exit_code << ")\033[0m" << std::endl;
        std::cout << "\n\033[1;33m💡 Tip:\033[0m Try asking Neuron: \033[2;37m\"why did this command fail?\"\033[0m" << std::endl;
    } else {
        std::cout << "\033[1;32m✅ Command completed successfully!\033[0m" << std::endl;
    }

    return 0;
}

int CLI::handle_tell(const std::string& prompt) {
    neuron::Config config;
    neuron::AIClient client(config);

    std::cout << "\n\033[1;35m🧬 Neuron AI\033[0m \033[2;37mis thinking...\033[0m" << std::endl;
    
    auto result = client.run(prompt, neuron::Mode::TELL);
    if (result.has_value()) {
        std::cout << "\n\033[2;37m" << std::string(60, '-') << "\033[0m" << std::endl;
        std::cout << result.value() << std::endl;
        std::cout << "\033[2;37m" << std::string(60, '-') << "\033[0m\n" << std::endl;
        return 0;
    } else {
        std::cout << "\n\033[1;31m❌ Failed to get response from Neuron AI\033[0m" << std::endl;
        std::cout << "\033[2;37m💡 Check your internet connection and API key\033[0m" << std::endl;
        return 1;
    }
}

} // namespace neuron
