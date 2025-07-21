// gitee_api.cpp
#include "gitee_api.h"
#include "json.hpp"
#include <curl/curl.h>
#include <iostream>

using json = nlohmann::json;

GiteeApi::GiteeApi(const std::string& apiKey) : apiKey_(apiKey) {}

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t totalSize = size * nmemb;
    std::string* str = static_cast<std::string*>(userp);
    str->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}
bool GiteeApi::listIssues(const std::string& owner, const std::string& repo) {
    std::string url = "https://gitee.com/api/v5/repos/" + owner + "/" + repo + "/issues";
    url += "?access_token=" + apiKey_;
    url += "&state=all&sort=created&direction=desc&page=1&per_page=20";

    CURL* curl = curl_easy_init();
    std::string responseString;

    if (!curl) return false;

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json;charset=UTF-8");

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseString);

    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        std::cerr << "❌ cURL hatası: " << curl_easy_strerror(res) << "\n";
        return false;
    }
std::cout << "[DEBUG] Gitee response: " << responseString << "\n";

    try {
        auto jsonArr = json::parse(responseString);

        if (!jsonArr.is_array()) {
            std::cerr << "❌ Beklenmeyen JSON cevabı (dizi bekleniyordu).\n";
            return false;
        }

        if (jsonArr.empty()) {
            std::cout << "📭 Hiç issue bulunamadı.\n";
            return true;
        }

        std::cout << "📋 Issue Listesi:\n";
        for (const auto& item : jsonArr) {
            std::cout << "🔢 Number: " << item["number"] << "\n";
            std::cout << "📝 Title: " << item["title"] << "\n";
            std::cout << "📄 Body: " << item["body"] << "\n";
            std::cout << "📌 State: " << item["state"] << "\n";
            std::cout << "🔗 URL: " << item["html_url"] << "\n";
            std::cout << "---------------------------\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "❌ JSON parse hatası: " << e.what() << "\n";
        return false;
    }

    return true;
}

bool GiteeApi::createIssue(const std::string& owner, const std::string& repo,
                           const std::string& title, const std::string& description,
                           const std::string& labels) {
    std::string url = "https://gitee.com/api/v5/repos/" + owner + "/issues";

    json payload = {
        {"access_token", apiKey_},
        {"title", title},
        {"body", description}
    };

    if (!labels.empty()) {
        payload["labels"] = labels;
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
    std::cout << "🔢 Number: " << result["number"] << "\n";
    std::cout << "🔗 URL: " << result["html_url"] << "\n";

        } else if (result.contains("message")) {
            std::cerr << "❌ API Hatası: " << result["message"] << "\n";
            return false;
        } else {
            std::cerr << "⚠️ Beklenmeyen API cevabı.\n";
            return false;
        }
    } catch (const std::exception& e) {
        std::cerr << "❌ JSON parse hatası: " << e.what() << "\n";
        return false;
    }

    return true;
}

bool GiteeApi::updateIssue(const std::string& owner, const std::string& repo,
                           const std::string& number, const std::string& title,
                           const std::string& description, const std::string& state,
                           const std::string& labels) {
    std::string url = "https://gitee.com/api/v5/repos/" + owner + "/issues/" + number;

json payload = {
    {"access_token", apiKey_},
    {"repo", repo},
    {"title", title},
    {"body", description},
    {"state", state}
};


    if (!labels.empty()) {
        payload["labels"] = labels;
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
        std::cerr << "❌ JSON parse hatası: " << e.what() << "\n";
        return false;
    }

    return true;
}

std::string GiteeApi::httpPost(const std::string& url, const std::string& jsonData) {
    CURL* curl = curl_easy_init();
    std::string responseString;

    if (!curl) return "";

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json;charset=UTF-8");

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonData.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseString);

    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        std::cerr << "cURL hatası: " << curl_easy_strerror(res) << "\n";
        return "";
    }

    return responseString;
}

std::string GiteeApi::httpPatch(const std::string& url, const std::string& jsonData) {
    CURL* curl = curl_easy_init();
    std::string responseString;

    if (!curl) return "";

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json;charset=UTF-8");

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonData.c_str());
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

