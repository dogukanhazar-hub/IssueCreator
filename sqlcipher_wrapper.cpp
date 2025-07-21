#include "sqlcipher_wrapper.h"
#include <sqlite3.h>
#include <iostream>

SqlCipherWrapper::SqlCipherWrapper(const std::string& db_path, const std::string& db_password)
    : db_path_(db_path), db_password_(db_password), db_(nullptr) {}

SqlCipherWrapper::~SqlCipherWrapper() {
    close();
}

bool SqlCipherWrapper::open() {
    if (sqlite3_open(db_path_.c_str(), reinterpret_cast<sqlite3**>(&db_)) != SQLITE_OK) {
        std::cerr << "DB açılamadı\n";
        return false;
    }

    std::string pragma_key = "PRAGMA key = '" + db_password_ + "';";
    if (sqlite3_exec(reinterpret_cast<sqlite3*>(db_), pragma_key.c_str(), nullptr, nullptr, nullptr) != SQLITE_OK) {
        std::cerr << "Şifreleme anahtarı ayarlanamadı\n";
        return false;
    }

    // ✅ GÜNCEL TABLO YAPISI
    std::string create_table = 
        "CREATE TABLE IF NOT EXISTS api_keys ("
        "id INTEGER PRIMARY KEY, "
        "key TEXT NOT NULL, "
        "platform TEXT NOT NULL"
        ");";

    if (sqlite3_exec(reinterpret_cast<sqlite3*>(db_), create_table.c_str(), nullptr, nullptr, nullptr) != SQLITE_OK) {
        std::cerr << "Tablo oluşturulamadı\n";
        return false;
    }

    return true;
}

void SqlCipherWrapper::close() {
    if (db_) {
        sqlite3_close(reinterpret_cast<sqlite3*>(db_));
        db_ = nullptr;
    }
}

bool SqlCipherWrapper::saveApiKey(const std::string& apiKey, const std::string& platform) {
    if (!db_) return false;

    std::string del = "DELETE FROM api_keys WHERE platform = '" + platform + "';";
    

    if (sqlite3_exec(reinterpret_cast<sqlite3*>(db_), del.c_str(), nullptr, nullptr, nullptr) != SQLITE_OK) {
        std::cerr << "Eski API key silinemedi\n";
        std::cerr << "[SQLite Hata] " << sqlite3_errmsg(reinterpret_cast<sqlite3*>(db_)) << "\n";
        return false;
    }

    std::string insert = "INSERT INTO api_keys (key, platform) VALUES ('" + apiKey + "', '" + platform + "');";
    if (sqlite3_exec(reinterpret_cast<sqlite3*>(db_), insert.c_str(), nullptr, nullptr, nullptr) != SQLITE_OK) {
        std::cerr << "Yeni API key eklenemedi\n";
        std::cerr << "[SQLite Hata] " << sqlite3_errmsg(reinterpret_cast<sqlite3*>(db_)) << "\n";
        return false;
    }

    return true;
}

bool SqlCipherWrapper::getApiKey(std::string& apiKey, const std::string& platform) {
    if (!db_) return false;
    sqlite3_stmt* stmt;

    std::string query = "SELECT key FROM api_keys WHERE platform = ? LIMIT 1;";
    if (sqlite3_prepare_v2(reinterpret_cast<sqlite3*>(db_), query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Sorgu hazırlanamadı\n";
        return false;
    }

    if (sqlite3_bind_text(stmt, 1, platform.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) {
        std::cerr << "Parametre bağlanamadı\n";
        sqlite3_finalize(stmt);
        return false;
    }

    int rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        const unsigned char* text = sqlite3_column_text(stmt, 0);
        apiKey = std::string(reinterpret_cast<const char*>(text));
        sqlite3_finalize(stmt);
        return true;
    }

    sqlite3_finalize(stmt);
    return false;
}




