#include "neuron/ai_client.hpp"

#include <curl/curl.h>
#include <sstream>
#include <iostream>
#include <nlohmann/json.hpp>

namespace neuron {

using json = nlohmann::json;

AIClient::AIClient(const Config& config) {
    auto key = config.getNeuronApiKey();
    
    if (!key) {
        throw std::runtime_error("API key not found in configuration.");
    }

    api_key_ = *key;

    // Get configured model or use default
    auto configured_model = config.getNeuronModel();
    model_ = configured_model ? *configured_model : "openai/gpt-4";
}

Prompt AIClient::build_prompt(const std::string& input, Mode mode) const {
    Prompt prompt;
    
    switch (mode) {
        case Mode::RUN:
            prompt.system_message = R"(You are an expert system administrator and command-line specialist with deep knowledge of Unix/Linux and macOS systems.
                                    ROLE: Generate safe, efficient shell commands based on user requests.
                                    CONSTRAINTS:
                                        - Only output the command itself, no explanations unless requested
                                        - Prioritize safety: avoid destructive operations without explicit confirmation
                                        - Use modern, cross-platform commands when possible
                                        - For macOS, prefer built-in tools or common package managers (brew, port)
                                        - For complex operations, break into multiple safe commands
                                        - Always use proper quoting and escaping
                                        - If the request is ambiguous, choose the safest interpretation
                                    OUTPUT FORMAT: Return only the command(s), one per line. No markdown, no explanations.
                                    SAFETY RULES:
                                        - Never suggest: rm -rf /, dd commands on system disks, chmod 777 on system files
                                        - For file operations, use relative paths unless absolute paths are explicitly requested
                                        - When modifying system files, suggest backup commands first
                                        - For network operations, prefer secure protocols (https, ssh, etc.)
                                    EXAMPLES:
                                        User: "list files in current directory" → "ls -la"
                                        User: "find large files" → "find . -type f -size +100M -exec ls -lh {} \;"
                                        User: "install node" → "brew install node" (macOS) or "curl -fsSL https://deb.nodesource.com/setup_lts.x | sudo -E bash - && sudo apt-get install -y nodejs" (Linux))";
            prompt.user_template = "Generate a shell command for: " + input;
            break;
        case Mode::TELL:
            prompt.system_message = R"(You are a knowledgeable technical assistant with expertise across software development, system administration, and general computing topics.
                                    ROLE: Provide clear, accurate, and helpful explanations tailored to the user's apparent technical level.
                                    RESPONSE STYLE:
                                        - Start with a concise direct answer
                                        - Follow with relevant details and context
                                        - Use examples when helpful
                                        - Structure information logically (overview → details → examples)
                                        - Adjust technical depth based on the question complexity
                                    AREAS OF EXPERTISE:
                                        - Programming languages and frameworks
                                        - System administration and DevOps
                                        - Command-line tools and scripting
                                        - Software architecture and design patterns
                                        - Development workflows and best practices
                                        - Troubleshooting and debugging
                                    FORMAT:
                                        - Use bullet points for lists
                                        - Include code examples in backticks when relevant
                                        - Highlight important concepts
                                        - Provide actionable information when possible
                                    TONE: Professional but approachable, like a senior colleague explaining something to a peer.)";
            prompt.user_template = "Please explain: " + input;
            break;
    }

    return prompt;
}

static size_t curl_write_callback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t total_size = size * nmemb;
    output->append(static_cast<char*>(contents), total_size);
    return total_size;
}

std::optional<std::string> AIClient::run(const std::string& user_input, Mode mode) {
    Prompt prompt = build_prompt(user_input, mode);
    std::string response_string;

    json request_body = {
        {"model", model_},
        {"messages", {
            {{"role", "system"}, {"content", prompt.system_message}},
            {{"role", "user"}, {"content", prompt.user_template}}
        }},
        {"max_tokens", mode == Mode::RUN ? 150 : 500},  // Shorter for commands, longer for explanations
        {"temperature", mode == Mode::RUN ? 0.1 : 0.3}  // Lower temperature for commands (more deterministic)
    };

    CURL* curl = curl_easy_init();
    if (!curl) return std::nullopt;

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, ("Authorization: Bearer " + api_key_).c_str());
    headers = curl_slist_append(headers, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, "https://models.github.ai/inference/chat/completions");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    std::string body_str = request_body.dump();
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body_str.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L); // Set timeout to 10 seconds

    CURLcode res = curl_easy_perform(curl);

    // Get HTTP response code
    long response_code;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
    
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        std::cerr << "CURL error: " << curl_easy_strerror(res) << std::endl;
        return std::nullopt;
    }

    // Check for HTTP errors before parsing JSON
    if (response_code == 401) {
        std::cerr << "Authentication failed. Please check your API key." << std::endl;
        return std::nullopt;
    } else if (response_code != 200) {
        std::cerr << "HTTP Error " << response_code << ": " << response_string << std::endl;
        return std::nullopt;
    }

    try {
        json response_json = json::parse(response_string);
        if (response_json.contains("choices") && !response_json["choices"].empty()) {
            return response_json["choices"][0]["message"]["content"].get<std::string>();
        } else {
            std::cerr << "Unexpected response format: " << response_string << std::endl;
            return std::nullopt;
        }
    } catch (const json::parse_error& e) {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
        return std::nullopt;
    }
}

} // namespace neuron
