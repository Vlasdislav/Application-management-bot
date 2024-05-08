#include "ManagmentBot.hpp"


ManagmentBot::ManagmentBot(const std::string& bot,
                            const std::string& db)
    : bot(bot), db(db, SQLite::OPEN_CREATE | SQLite::OPEN_READWRITE) {}

void ManagmentBot::eventStart(ConversationMap& conversations,
                            const TgBot::Message::Ptr& message) {
    const int64_t chatId = message->chat->id;
    conversations[chatId].currentCommand = "start";
    try {
        SQLite::Statement query(db, "SELECT service_number FROM employees WHERE chat_id = ?");
        query.bind(1, chatId);
        if (query.executeStep()) {
            conversations[chatId].serviceNumber = query.getColumn("service_number").getString();
            conversations[chatId].name = query.getColumn("name").getString();
            conversations[chatId].id = query.getColumn("id").getInt64();
        }
    } catch (std::exception& e) {
        std::cerr << "SQLite exception: " << e.what() << std::endl;
    }
    if (conversations[chatId].serviceNumber.empty()) {
        bot.getApi().sendMessage(chatId, "Привет! Я, Бот, давай знакомится!\nВведи свой табельный номер:");
    } else {
        bot.getApi().sendMessage(chatId, "Привет! Я, Бот, мы уже знакомы. Можешь ознакомится со списком команд в меню.");
    }
}

void ManagmentBot::eventView(ConversationMap& conversations,
                            const TgBot::Message::Ptr& message) {
    const int64_t chatId = message->chat->id;
    if (!conversations[chatId].serviceNumber.empty()) {
        conversations[chatId].currentCommand = "view";
        bot.getApi().sendMessage(chatId, "Введите табельный номер:");
    } else {
        conversations[chatId].currentCommand = "start";
        bot.getApi().sendMessage(chatId, "Не удалось посмотреть заявки. Для этого необходимо зарегистрироваться в системе.");
    }
}

void ManagmentBot::eventViewAll(ConversationMap& conversations,
                            const TgBot::Message::Ptr& message) {
    const int64_t chatId = message->chat->id;
    if (!conversations[chatId].serviceNumber.empty()) {
        activityViewAll(conversations, message);
    } else {
        bot.getApi().sendMessage(chatId, "Не удалось посмотреть заявки. Для этого необходимо зарегистрироваться в системе.");
    }
    conversations[chatId].currentCommand = "start";
}

void ManagmentBot::eventAdd(ConversationMap& conversations,
                            const TgBot::Message::Ptr& message) {
    const int64_t chatId = message->chat->id;
    if (!conversations[chatId].serviceNumber.empty()) {
        conversations[chatId].currentCommand = "add";
        bot.getApi().sendMessage(chatId, "Введите текст заявки:");
    } else {
        conversations[chatId].currentCommand = "start";
        bot.getApi().sendMessage(message->chat->id, "Не удалось добавить создать заявку. Для этого необходимо зарегистрироваться в системе.");
    }
}

void ManagmentBot::eventWork(ConversationMap& conversations,
                            const TgBot::Message::Ptr& message) {
    conversations[message->chat->id].currentCommand = "work";
}

void ManagmentBot::activityStart(ConversationMap& conversations,
                            const TgBot::Message::Ptr& message) {
    const int64_t chatId = message->chat->id;
    if (conversations[chatId].serviceNumber.empty()) {
        try {
            SQLite::Statement query(db, "SELECT service_number FROM employees WHERE service_number = ?");
            query.bind(1, message->text);
            if (query.executeStep()) {
                bot.getApi().sendMessage(chatId, "Пользователь с таким табельным номером уже существует, попробуй еще:");
            } else {
                conversations[chatId].serviceNumber = message->text;
                bot.getApi().sendMessage(chatId, "Супер, теперь введи свое ФИО:");
            }
        } catch (std::exception& e) {
            std::cerr << "SQLite exception: " << e.what() << std::endl;
        }
    } else {
        if (conversations[chatId].name.empty()) {
            conversations[chatId].name = message->text;
            SQLite::Statement query(db, "INSERT INTO employees (name, service_number, chat_id) VALUES (?, ?, ?)");
            query.bind(1, conversations[chatId].name);
            query.bind(2, conversations[chatId].serviceNumber);
            query.bind(3, chatId);
            query.exec();
            conversations[chatId].id = db.getLastInsertRowid();
            bot.getApi().sendMessage(chatId, "Отлично! Регистрация прошла успешно! Добро пожаловать!");
        }
    }
}

void ManagmentBot::activityView(ConversationMap& conversations,
                            const TgBot::Message::Ptr& message) {
    try {
        int64_t employee_created_id = -1;
        {
            SQLite::Statement select(db, "SELECT * FROM employees WHERE service_number = ?");
            select.bind(1, message->text);
            if (select.executeStep()) {
                employee_created_id = select.getColumn(0).getInt64();
            }
        }
        {
            SQLite::Statement select(db, "SELECT * FROM requests WHERE employee_created_id = ?");
            select.bind(1, employee_created_id);
            std::vector<std::vector<std::string>> table = {
                { "id", "Текст заявки", "Создана работником", "Создана в", "Взята в работу",
                    "Начало работы", "Результаты", "Конец работы" }
            };
            while (select.executeStep()) {
                std::string id = select.getColumn(0).getString();
                std::string request = select.getColumn(1).getString();
                std::string employee_created_id = select.getColumn(2).getString();
                std::string create_time = select.getColumn(3).getString();
                std::string employee_worker_id = select.getColumn(4).getString();
                std::string work_time = select.getColumn(5).getString();
                std::string results = select.getColumn(6).getString();
                std::string end_time = select.getColumn(7).getString();
                table.emplace_back(std::vector<std::string>{
                    id, request, employee_created_id, create_time,
                    employee_worker_id, work_time, results, end_time
                });
            }
            std::string answer = table::generate(table);
            bot.getApi().sendMessage(message->chat->id, answer);
        }
    } catch (std::exception const& e) {
        std::cerr << "SQLite error: " << e.what() << std::endl;
    }
}

void ManagmentBot::activityViewAll(ConversationMap& conversations,
                            const TgBot::Message::Ptr& message) {
    SQLite::Statement select(db, "SELECT * FROM requests");
    std::vector<std::vector<std::string>> table = {
        { "id", "Текст заявки", "Создана работником", "Создана в", "Взята в работу",
            "Начало работы", "Результаты", "Конец работы" }
    };
    while (select.executeStep()) {
        std::string id = select.getColumn(0).getString();
        std::string request = select.getColumn(1).getString();
        std::string employee_created_id = select.getColumn(2).getString();
        std::string create_time = select.getColumn(3).getString();
        std::string employee_worker_id = select.getColumn(4).getString();
        std::string work_time = select.getColumn(5).getString();
        std::string results = select.getColumn(6).getString();
        std::string end_time = select.getColumn(7).getString();
        table.emplace_back(std::vector<std::string>{
            id, request, employee_created_id, create_time,
            employee_worker_id, work_time, results, end_time
        });
    }
    std::string answer = table::generate(table);
    bot.getApi().sendMessage(message->chat->id, answer);
}

void ManagmentBot::activityAdd(ConversationMap& conversations,
                            const TgBot::Message::Ptr& message) {
    const int64_t chatId = message->chat->id;
    std::string requestText = message->text;
    int64_t employeeCreatedId = conversations[chatId].id;
    SQLite::Statement insert(db, "INSERT INTO requests (request, employee_created_id) VALUES (?, ?)");
    insert.bind(1, requestText);
    insert.bind(2, employeeCreatedId);
    insert.exec();
    bot.getApi().sendMessage(chatId, "Новая заявка успешно создана!");
    database::showBD(db);
    conversations[chatId].currentCommand = "start";
}

void ManagmentBot::activityWork(ConversationMap& conversations,
                            const TgBot::Message::Ptr& message) {
    // TODO
}
