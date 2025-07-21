#include "github_api.h"
#include <curl/curl.h>
#include <iostream>
#include <sstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

GitHubApi::GitHubApi(const std::string& apiKey) : apiKey_(apiKey) {}

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

bool GitHubApi::listIssues(const std::string& owner, const std::string& repo) {
    std::string url = "https://api.github.com/repos/" + owner + "/" + repo + "/issues";

    CURL* curl = curl_easy_init();
    if (!curl) return false;

    std::string readBuffer;
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, ("Authorization: token " + apiKey_).c_str());
    headers = curl_slist_append(headers, "User-Agent: issue-creator");

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        std::cerr << "cURL hatası: " << curl_easy_strerror(res) << "\n";
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        return false;
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    try {
        auto json = nlohmann::json::parse(readBuffer);

        if (!json.is_array()) {
            std::cerr << "❌ Beklenmeyen JSON cevabı (dizi bekleniyordu).\n";
            return false;
        }

        if (json.empty()) {
            std::cout << "📭 Hiç issue bulunamadı.\n";
            return true;
        }

        std::cout << "📋 GitHub Issue Listesi:\n";
        for (const auto& issue : json) {
            std::cout << "✅ Issue Başarıyla Listelendi!\n";
            std::cout << "🆔 ID: " << issue["id"] << "\n";
            std::cout << "🔢 Number: " << issue["number"] << "\n";
            std::cout << "📝 Title: " << issue["title"] << "\n";
            std::cout << "📄 Body: " << (issue.contains("body") && !issue["body"].is_null() ? issue["body"].get<std::string>() : "") << "\n";
            std::cout << "📌 State: " << issue["state"] << "\n";
            std::cout << "🔗 URL: " << issue["html_url"] << "\n";
            std::cout << "---------------------------\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "❌ JSON parse hatası: " << e.what() << "\n";
        return false;
    }

    return true;
}


bool GitHubApi::createIssue(const std::string& owner, const std::string& repo,
                            const std::string& title, const std::string& description,
                            const std::string& labels) {
    std::string url = "https://api.github.com/repos/" + owner + "/" + repo + "/issues";

    json payload = {
        {"title", title},
        {"body", description}
    };

    if (!labels.empty()) {
        // GitHub API için labels bir dizi olmalı
        std::vector<std::string> labelList;
        std::stringstream ss(labels);
        std::string label;
        while (std::getline(ss, label, ',')) {
            labelList.push_back(label);
        }
        payload["labels"] = labelList;
    }

    std::string response = httpPost(url, payload.dump());
    if (response.empty()) {
        std::cerr << "API isteği başarısız.\n";
        return false;
    }

    try {
        auto result = json::parse(response);
        if (result.contains("id") && result.contains("html_url")) {
            std::cout << "✅ Issue başarıyla oluşturuldu!\n";
            std::cout << "🆔 ID: " << result["id"] << "\n";
            std::cout << "🔗 URL: " << result["html_url"] << "\n";
        } else if (result.contains("message")) {
            std::cerr << "❌ API Hatası: " << result["message"] << "\n";
            return false;
        } else {
            std::cerr << "⚠️ Beklenmeyen API cevabı.\n";
            return false;
        }
    } catch (const std::exception& e) {
        std::cerr << "JSON parse hatası: " << e.what() << "\n";
        return false;
    }

    return true;
}

bool GitHubApi::updateIssue(const std::string& owner, const std::string& repo,
                            const std::string& number, const std::string& title,
                            const std::string& description, const std::string& state,
                            const std::string& labels) {
    std::string url = "https://api.github.com/repos/" + owner + "/" + repo + "/issues/" + number;

    json payload;
    if (!title.empty()) payload["title"] = title;
    if (!description.empty()) payload["body"] = description;
    if (!state.empty()) payload["state"] = state;

    if (!labels.empty()) {
        std::vector<std::string> labelList;
        std::stringstream ss(labels);
        std::string label;
        while (std::getline(ss, label, ',')) {
            labelList.push_back(label);
        }
        payload["labels"] = labelList;
    }

    std::string response = httpPatch(url, payload.dump());
    if (response.empty()) {
        std::cerr << "Güncelleme isteği başarısız.\n";
        return false;
    }

    try {
        auto result = json::parse(response);
        if (result.contains("id") && result.contains("html_url")) {
            std::cout << "✅ Issue başarıyla güncellendi!\n";
            std::cout << "🆔 ID: " << result["id"] << "\n";
            std::cout << "🔗 URL: " << result["html_url"] << "\n";
        } else if (result.contains("message")) {
            std::cerr << "❌ API Hatası: " << result["message"] << "\n";
            return false;
        } else {
            std::cerr << "⚠️ Beklenmeyen API cevabı.\n";
            return false;
        }
    } catch (const std::exception& e) {
        std::cerr << "JSON parse hatası: " << e.what() << "\n";
        return false;
    }

    return true;
}

std::string GitHubApi::httpPost(const std::string& url, const std::string& jsonData) {
    CURL* curl = curl_easy_init();
    std::string responseString;

    if (!curl) return "";

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, ("Authorization: token " + apiKey_).c_str());
    headers = curl_slist_append(headers, "User-Agent: issue-creator");
    headers = curl_slist_append(headers, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonData.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseString);

    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        std::cerr << "cURL POST hatası: " << curl_easy_strerror(res) << "\n";
        return "";
    }

    return responseString;
}

std::string GitHubApi::httpPatch(const std::string& url, const std::string& jsonData) {
    CURL* curl = curl_easy_init();
    std::string responseString;

    if (!curl) return "";

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, ("Authorization: token " + apiKey_).c_str());
    headers = curl_slist_append(headers, "User-Agent: issue-creator");
    headers = curl_slist_append(headers, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonData.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseString);

    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        std::cerr << "cURL PATCH hatası: " << curl_easy_strerror(res) << "\n";
        return "";
    }

    return responseString;
}

