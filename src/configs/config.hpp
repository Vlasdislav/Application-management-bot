#pragma once

#include <iostream>
#include <tgbot/tgbot.h>
#include <SQLiteCpp/SQLiteCpp.h>


namespace bot {
    const std::string token = "6671188985:AAHDSpRC7GZL3JOJiU-C9FAiIsqQWxu5mp0";
}

namespace database {
    inline void createDB(SQLite::Database& db) {
        try {
            db.exec("CREATE TABLE IF NOT EXISTS employees ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "name TEXT,"
                    "service_number TEXT,"
                    "chat_id INTEGER"
                    ")");

            db.exec("CREATE TABLE IF NOT EXISTS requests ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "request TEXT,"
                    "employee_created_id INTEGER,"
                    "create_time DATETIME DEFAULT CURRENT_TIMESTAMP,"
                    "employee_worker_id INTEGER,"
                    "work_time DATETIME,"
                    "results TEXT,"
                    "end_time DATETIME,"
                    "FOREIGN KEY (employee_created_id) REFERENCES employees(id)"
                    ")");
        } catch (std::exception const& e) {
            std::cerr << "SQLite error: " << e.what() << std::endl;
        }
    }
}
