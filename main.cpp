#include <iostream>
#include <string>
#include <vector>
#include "sqlcipher_wrapper.h"
#include "gitee_api.h"
#include "github_api.h"

void printUsage() {
    std::cout << "Usage:\n";
std::cout << "  issue-creator register --apikey \"your_api_key\"\n";
std::cout << "  issue-creator update-key --apikey \"new_api_key\"\n"; // <-- burada update-key olarak yaz
std::cout << "  issue-creator create --owner \"owner\" --repo \"repo\" --title \"title\" --description \"desc\" [--labels \"bug,test\"]\n";
std::cout << "  issue-creator update --owner \"owner\" --repo \"repo\" --number \"issue_number\" --title \"title\" --description \"desc\" --state \"open|progressing|closed\" [--labels \"bug,test\"]\n";
std::cout << "  issue-creator list --owner \"owner\" --repo \"repo\"\n";
std::cout << "  issue-creator github register --apikey \"your_github_api_key\"\n";
std::cout << "  issue-creator github update-key --apikey \"new_github_api_key\"\n";
std::cout << "  issue-creator github create --owner \"owner\" --repo \"repo\" --title \"title\" --description \"desc\" [--labels \"bug,test\"]\n";
std::cout << "  issue-creator github update --owner \"owner\" --repo \"repo\" --number \"issue_number\" --title \"title\" --description \"desc\" --state \"open|closed\" [--labels \"bug,test\"]\n";
std::cout << "  issue-creator github list --owner \"owner\" --repo \"repo\"\n";


}


std::string getArgValue(const std::vector<std::string>& args, const std::string& key) {
    for (size_t i = 0; i < args.size(); ++i) {
        if (args[i] == key && i + 1 < args.size()) {
            return args[i + 1];
        }
    }
    return "";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage();
        return 1;
    }

    std::string command = argv[1];

    const std::string DB_PATH = "issue_creator.db";
    const std::string DB_PASSWORD = "your-secure-password"; // Şifreni buraya yaz

    SqlCipherWrapper db(DB_PATH, DB_PASSWORD);
    if (!db.open()) {
        std::cerr << "Veritabanı açılamadı\n";
        return 1;
    }

    if (command == "register" || command == "update-key") {
        std::vector<std::string> args(argv + 2, argv + argc);
        std::string apiKey = getArgValue(args, "--apikey");
        if (apiKey.empty()) {
            std::cerr << "--apikey parametresi gerekli.\n";
            return 1;
        }
        if (!db.saveApiKey(apiKey)) {
            std::cerr << "API key kaydedilemedi.\n";
            return 1;
        }
        std::cout << "API key başarıyla kaydedildi.\n";
    }
else if (command == "github") {
    if (argc < 3) {
        std::cerr << "github komutu için alt komut gerekli (register, update-key, create, update, list)\n";
        return 1;
    }

    std::string subCommand = argv[2];
    std::vector<std::string> args(argv + 3, argv + argc);

    if (subCommand == "register" || subCommand == "update-key") {
        std::string apiKey = getArgValue(args, "--apikey");
        if (apiKey.empty()) {
            std::cerr << "--apikey parametresi gerekli.\n";
            return 1;
        }
        if (!db.saveApiKey(apiKey, "github")) {
            std::cerr << "GitHub API key kaydedilemedi.\n";
            return 1;
        }
        std::cout << "GitHub API key başarıyla kaydedildi.\n";
    }

    else if (subCommand == "list") {
        std::string owner = getArgValue(args, "--owner");
        std::string repo = getArgValue(args, "--repo");

        if (owner.empty() || repo.empty()) {
            std::cerr << "list için --owner ve --repo gerekli.\n";
            return 1;
        }

        std::string apiKey;
        if (!db.getApiKey(apiKey, "github")) {
            std::cerr << "GitHub API key bulunamadı. Önce register yapın.\n";
            return 1;
        }

        GitHubApi github(apiKey);
        if (!github.listIssues(owner, repo)) {
            std::cerr << "GitHub issue listesi alınamadı.\n";
            return 1;
        }
    }

    else if (subCommand == "create") {
        std::string owner = getArgValue(args, "--owner");
        std::string repo = getArgValue(args, "--repo");
        std::string title = getArgValue(args, "--title");
        std::string description = getArgValue(args, "--description");
        std::string labels = getArgValue(args, "--labels");

        if (owner.empty() || repo.empty() || title.empty()) {
            std::cerr << "create için --owner, --repo ve --title parametreleri zorunludur.\n";
            return 1;
        }

        std::string apiKey;
        if (!db.getApiKey(apiKey, "github")) {
            std::cerr << "GitHub API key bulunamadı. Önce register yapın.\n";
            return 1;
        }

        GitHubApi github(apiKey);
        if (!github.createIssue(owner, repo, title, description, labels)) {
            std::cerr << "GitHub issue oluşturulamadı.\n";
            return 1;
        }
    }

    else if (subCommand == "update") {
        std::string owner = getArgValue(args, "--owner");
        std::string repo = getArgValue(args, "--repo");
        std::string number = getArgValue(args, "--number");
        std::string title = getArgValue(args, "--title");
        std::string description = getArgValue(args, "--description");
        std::string state = getArgValue(args, "--state");
        std::string labels = getArgValue(args, "--labels");

        if (owner.empty() || repo.empty() || number.empty()) {
            std::cerr << "update için --owner, --repo ve --number parametreleri zorunludur.\n";
            return 1;
        }

        std::string apiKey;
        if (!db.getApiKey(apiKey, "github")) {
            std::cerr << "GitHub API key bulunamadı. Önce register yapın.\n";
            return 1;
        }

        GitHubApi github(apiKey);
        if (!github.updateIssue(owner, repo, number, title, description, state, labels)) {
            std::cerr << "GitHub issue güncellenemedi.\n";
            return 1;
        }
    }

    else {
        std::cerr << "Bilinmeyen github alt komutu.\n";
        printUsage();
        return 1;
    }
}



    else if (command == "create") {
        std::vector<std::string> args(argv + 2, argv + argc);
        std::string owner = getArgValue(args, "--owner");
        std::string repo = getArgValue(args, "--repo");
        std::string title = getArgValue(args, "--title");
        std::string description = getArgValue(args, "--description");
        std::string labels = getArgValue(args, "--labels");

        if (owner.empty() || title.empty()) {
    std::cerr << "create için --owner ve --title parametreleri zorunludur\n";
    return 1;
}


        std::string apiKey;
        if (!db.getApiKey(apiKey)) {
            std::cerr << "API key bulunamadı. Lütfen önce register yapın.\n";
            return 1;
        }

        GiteeApi gitee(apiKey);
        if (!gitee.createIssue(owner, repo, title, description, labels)) {
            std::cerr << "Issue oluşturulamadı.\n";
            return 1;
        }
    }
    else if (command == "list") {
    std::vector<std::string> args(argv + 2, argv + argc);
    std::string owner = getArgValue(args, "--owner");
    std::string repo = getArgValue(args, "--repo");

    if (owner.empty() || repo.empty()) {
        std::cerr << "list için --owner ve --repo parametreleri zorunludur\n";
        return 1;
    }

    std::string apiKey;
    if (!db.getApiKey(apiKey)) {
        std::cerr << "API key bulunamadı. Lütfen önce register yapın.\n";
        return 1;
    }

    GiteeApi gitee(apiKey);
    if (!gitee.listIssues(owner, repo)) {
        std::cerr << "Issue listesi alınamadı.\n";
        return 1;
    }
}


    else if (command == "update") {
        std::vector<std::string> args(argv + 2, argv + argc);
        std::string owner = getArgValue(args, "--owner");
        std::string repo = getArgValue(args, "--repo");
        std::string number = getArgValue(args, "--number");
        std::string title = getArgValue(args, "--title");
        std::string description = getArgValue(args, "--description");
        std::string state = getArgValue(args, "--state");
        std::string labels = getArgValue(args, "--labels");

if (owner.empty() || number.empty()) {
    std::cerr << "update için --owner ve --number parametreleri zorunludur\n";
    return 1;
}


        std::string apiKey;
        if (!db.getApiKey(apiKey)) {
            std::cerr << "API key bulunamadı. Lütfen önce register yapın.\n";
            return 1;
        }

        GiteeApi gitee(apiKey);
        if (!gitee.updateIssue(owner, repo, number, title, description, state, labels)) {
            std::cerr << "Issue güncellenemedi.\n";
            return 1;
        }
    }

    else if (command == "help") {
        printUsage();
    }

    else {
        std::cerr << "Bilinmeyen komut.\n";
        printUsage();
        return 1;
    }

    db.close();
    return 0;
}

