cmake_minimum_required(VERSION 3.8)

project(Application-management-bots)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=leak,address -Wall -Wpedantic") # -Wextra -Werror

find_library(TGBOT_LIBRARY TgBot HINTS /usr/local/lib)
find_path(TGBOT_INCLUDE_DIR TgBot/TgBot.h HINTS /usr/local/include)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/SQLiteCpp)

file(GLOB_RECURSE SRCS "${CMAKE_CURRENT_SOURCE_DIR}/src/*")
add_executable(main main.cpp ${SRCS})

target_include_directories(main PRIVATE ${TGBOT_INCLUDE_DIR})

target_link_libraries(main PRIVATE
    ${TGBOT_LIBRARY}
    boost_system
    ssl
    crypto
    pthread
    SQLiteCpp
    sqlite3
    dl
)
