cmake_minimum_required(VERSION 3.10)
project(issue_creator)

set(CMAKE_CXX_STANDARD 17)

find_package(CURL REQUIRED)
find_package(SQLite3 REQUIRED)
find_package(nlohmann_json REQUIRED)

include_directories(${CURL_INCLUDE_DIRS} /usr/include/sqlcipher)

add_executable(issue-creator main.cpp sqlcipher_wrapper.cpp gitee_api.cpp github_api.cpp)

target_link_libraries(issue-creator ${CURL_LIBRARIES} sqlite3 sqlcipher nlohmann_json::nlohmann_json)

