#include <iostream>
#include <tgbot/tgbot.h>
#include <SQLiteCpp/SQLiteCpp.h>

void createDB(SQLite::Database& db) {
    try {
        // Таблица работников
        db.exec("CREATE TABLE IF NOT EXISTS employees ("
                "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                "name TEXT,"
                "employee_id TEXT"
                ")");

        // Таблица заявок в службе поддержки
        db.exec("CREATE TABLE IF NOT EXISTS requests ("
                "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                "request TEXT,"
                "create_time DATETIME DEFAULT CURRENT_TIMESTAMP,"
                "employee_id TEXT,"
                "work_time DATETIME,"
                "results TEXT,"
                "end_time DATETIME,"
                "FOREIGN KEY (employee_id) REFERENCES employees(id)"
                ")");
    } catch (std::exception const& e) {
        std::cerr << "SQLite error: " << e.what() << std::endl;
    }
}

void showBD(SQLite::Database& db) {
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

struct UserContext {
    std::string employeeId;
    bool waitingForName = false;
    std::string currentCommand = "start";
};

int main() {
    SQLite::Database db("service_desk.db3", SQLite::OPEN_CREATE | SQLite::OPEN_READWRITE);
    createDB(db);

    std::unordered_map<int64_t, UserContext> conversations;

//  Bot
    TgBot::Bot bot("6671188985:AAHDSpRC7GZL3JOJiU-C9FAiIsqQWxu5mp0");
    
    // Обработчик команды /start
    bot.getEvents().onCommand("start", [&bot](TgBot::Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id, "Привет! Я бот Службы поддержки.");
    });

    // Обработчик команды /view для просмотра заявок по табельному номеру
    bot.getEvents().onCommand("view", [&bot, &db, &conversations](TgBot::Message::Ptr message) {
        conversations[message->chat->id].currentCommand = "view";
        if (!conversations[message->chat->id].employeeId.empty()) {
            bot.getApi().sendMessage(message->chat->id, "Ваши заявки:");
            try {
                SQLite::Statement select(db, "SELECT * FROM requests WHERE employee_id = ?");
                select.bind(1, conversations[message->chat->id].employeeId);
                std::string answer;
                while (select.executeStep()) {
                    int id = select.getColumn(0);
                    std::string request = select.getColumn(1);
                    std::string create_time = select.getColumn(2);
                    std::string employee_id = select.getColumn(3);
                    std::string work_time = select.getColumn(4);
                    std::string results = select.getColumn(5);
                    std::string end_time = select.getColumn(6);
                    answer += std::to_string(id) + " " + request + " " + create_time + " " + work_time + " " + results + " " + end_time + "\n\n";
                }
                bot.getApi().sendMessage(message->chat->id, answer);
            } catch (std::exception const& e) {
                std::cerr << "SQLite error: " << e.what() << std::endl;
            }
        } else {
            bot.getApi().sendMessage(message->chat->id, "Не удалось посмотреть заявки. Для этого необходимо зарегистрироваться в системе.");
        }
    });

    // Обработчик команды /add для добавления новой заявки
    bot.getEvents().onCommand("add", [&bot, &db, &conversations](TgBot::Message::Ptr message) {
        conversations[message->chat->id].currentCommand = "add";
        if (!conversations[message->chat->id].employeeId.empty()) {
            bot.getApi().sendMessage(message->chat->id, "Введите текст заявки:");
        } else {
            bot.getApi().sendMessage(message->chat->id, "Не удалось добавить заявку. Для этого необходимо зарегистрироваться в системе.");
        }
    });


    // Обработчик команды /work для взятия в работу заявки
    bot.getEvents().onCommand("work", [&bot, &conversations](TgBot::Message::Ptr message) {
        conversations[message->chat->id].currentCommand = "work";
    });
 
    // Обработчик команды /register для начала процесса регистрации
    bot.getEvents().onCommand("register", [&bot, &conversations](TgBot::Message::Ptr message) {
        conversations[message->chat->id].currentCommand = "register";
        bot.getApi().sendMessage(message->chat->id, "Введите ваш табельный номер:");
        conversations[message->chat->id].employeeId.clear();
        conversations[message->chat->id].waitingForName = false;
    });

    bot.getEvents().onNonCommandMessage([&bot, &conversations, &db](TgBot::Message::Ptr message) {
        if (conversations[message->chat->id].currentCommand == "add") {
            if (!conversations[message->chat->id].employeeId.empty()) {
                std::string requestText = message->text;
                std::string employeeId = conversations[message->chat->id].employeeId;
                SQLite::Statement insert(db, "INSERT INTO requests (request, employee_id) VALUES (?, ?)");
                insert.bind(1, requestText);
                insert.bind(2, employeeId);
                insert.exec();
                bot.getApi().sendMessage(message->chat->id, "Новая заявка успешно добавлена!");
            } else {
                bot.getApi().sendMessage(message->chat->id, "Не удалось добавить заявку. Для этого необходимо зарегистрироваться в системе.");
            }
            showBD(db);
        }
        if (conversations[message->chat->id].currentCommand == "register") {
            if (!conversations[message->chat->id].waitingForName) {
                if (message->text != "") {
                    std::string employeeId = message->text;
                    SQLite::Statement query(db, "SELECT COUNT(*) FROM employees WHERE employee_id = ?");
                    query.bind(1, employeeId);
                    if (query.executeStep()) {
                        int count = query.getColumn(0);
                        if (count > 0) {
                            bot.getApi().sendMessage(message->chat->id, "Вы уже зарегистрированы.");
                        } else {
                            bot.getApi().sendMessage(message->chat->id, "Введите ваше имя:");
                            conversations[message->chat->id].employeeId = employeeId;
                            conversations[message->chat->id].waitingForName = true;
                        }
                    } else {
                        bot.getApi().sendMessage(message->chat->id, "Произошла ошибка. Пожалуйста, попробуйте позже.");
                    }
                }
            } else {
                std::string name = message->text;
                std::string employeeId = conversations[message->chat->id].employeeId;
                SQLite::Statement insert(db, "INSERT INTO employees (name, employee_id) VALUES (?, ?)");
                insert.bind(1, name);
                insert.bind(2, employeeId);
                insert.exec();
                bot.getApi().sendMessage(message->chat->id, "Регистрация завершена. Добро пожаловать!");
                conversations[message->chat->id].waitingForName = false;
                showBD(db);
            }
        }
    });

    try {
        std::cout << "Bot username: " << bot.getApi().getMe()->username.data() << std::endl;
        bot.getApi().deleteWebhook();
        TgBot::TgLongPoll longPoll(bot);
        while (true) {
            std::cout << "Long poll started" << std::endl;
            longPoll.start();
        }
    } catch (TgBot::TgException& e) {
        std::cerr << "error: " << e.what() << std::endl;
    }
    return 0;
}
