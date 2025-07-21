// gitee_api.h
#ifndef GITEE_API_H
#define GITEE_API_H

#include <string>
#include <vector>

class GiteeApi {
public:
    GiteeApi(const std::string& apiKey);

    bool createIssue(const std::string& owner, const std::string& repo,
                     const std::string& title, const std::string& description,
                     const std::string& labels = "");

    bool updateIssue(const std::string& owner, const std::string& repo,
                     const std::string& number, const std::string& title,
                     const std::string& description, const std::string& state,
                     const std::string& labels = "");

    bool listIssues(const std::string& owner, const std::string& repo);

private:
    std::string apiKey_;

    std::string httpPost(const std::string& url, const std::string& jsonData);
    std::string httpPatch(const std::string& url, const std::string& jsonData);
    std::string httpGet(const std::string& url);
};

#endif
