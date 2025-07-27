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
    model_ = "openai/gpt-4.1"; // Default model
}

std::string AIClient::build_prompt(const std::string& input, Mode mode) const {
    switch(mode) {
        case Mode::RUN:
            return "Understand the request and give macOS terminal command which can be directly executed (give only terminal command): " + input;
        case Mode::TELL:
        default:
            return "Explain the task in concise terms: " + input;
    }
}

static size_t curl_write_callback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t total_size = size * nmemb;
    output->append(static_cast<char*>(contents), total_size);
    return total_size;
}

std::optional<std::string> AIClient::run(const std::string& user_input, Mode mode) {
    std::string prompt = build_prompt(user_input, mode);
    std::string response_string;

    json request_body = {
        {"model", model_},
        {"messages", {
            {{"role", "user"}, {"content", prompt}}
        }},
        {"max_tokens", 100},
        {"temperature", 0.3}
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
