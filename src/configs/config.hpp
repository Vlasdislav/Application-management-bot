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
                    "create_time DATETIME DEFAULT CURRENT_TIMESTAMP,"
                    "employee_id INTEGER,"
                    "work_time DATETIME,"
                    "results TEXT,"
                    "end_time DATETIME,"
                    "FOREIGN KEY (employee_id) REFERENCES employees(id)"
                    ")");
        } catch (std::exception const& e) {
            std::cerr << "SQLite error: " << e.what() << std::endl;
        }
    }

    // DBG
    inline void showBD(SQLite::Database& db) {
        try {
            {
                SQLite::Statement select(db, "SELECT * FROM employees");
                while (select.executeStep()) {
                    int id = select.getColumn(0);
                    std::string name = select.getColumn(1);
                    std::string employee_id = select.getColumn(2);
                    std::cout << "ID: " << id << ", Name: " << name << ", " << "Employee_id: " << employee_id << std::endl;
                }
            }
            {
                SQLite::Statement select(db, "SELECT * FROM requests");
                while (select.executeStep()) {
                    int id = select.getColumn(0);
                    std::string request = select.getColumn(1);
                    std::string create_time = select.getColumn(2);
                    std::string employee_id = select.getColumn(3);
                    std::string work_time = select.getColumn(4);
                    std::string results = select.getColumn(5);
                    std::string end_time = select.getColumn(6);
                    std::cout << id << ' ' << request << ' ' << create_time << ' ' << employee_id << ' ' << work_time << ' ' << results << ' ' << end_time << std::endl;
                }
            }
        } catch (std::exception const& e) {
            std::cerr << "SQLite error: " << e.what() << std::endl;
        }
    }
}
