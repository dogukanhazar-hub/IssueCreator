// sqlcipher_wrapper.h
#ifndef SQLCIPHER_WRAPPER_H
#define SQLCIPHER_WRAPPER_H

#include <string>

class SqlCipherWrapper {
public:
    SqlCipherWrapper(const std::string& db_path, const std::string& db_password);
    ~SqlCipherWrapper();

    bool open();
    void close();

bool saveApiKey(const std::string& apiKey, const std::string& platform = "gitee");
bool getApiKey(std::string& apiKey, const std::string& platform = "gitee");


private:
    std::string db_path_;
    std::string db_password_;
    void* db_; // sqlite3*
};

#endif

