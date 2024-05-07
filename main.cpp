#include <stdio.h>
#include <iostream>
#include <tgbot/tgbot.h>
#include <SQLiteCpp/SQLiteCpp.h>

int main() {
//  DB
    try {
        SQLite::Database db("example.db3", SQLite::OPEN_CREATE | SQLite::OPEN_READWRITE);

        db.exec("CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY, name TEXT)");

        SQLite::Statement query(db, "INSERT INTO users (name) VALUES (?)");
        query.bind(1, "Alice");
        query.exec();
        query.reset();

        query.bind(1, "Bob");
        query.exec();
        query.reset();

        SQLite::Statement select(db, "SELECT * FROM users");
        while (select.executeStep()) {
            int id = select.getColumn(0);
            std::string name = select.getColumn(1);
            std::cout << "ID: " << id << ", Name: " << name << std::endl;
        }
    } catch (std::exception const& e) {
        std::cerr << "SQLite error: " << e.what() << std::endl;
    }

//  Bot
    TgBot::Bot bot("6671188985:AAHDSpRC7GZL3JOJiU-C9FAiIsqQWxu5mp0");
    
    // Обработчик команды /start
    bot.getEvents().onCommand("start", [&bot](TgBot::Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id, "Привет! Я бот Службы поддержки.");
    });

    // Обработчик команды /view для просмотра заявок по табельному номеру
    bot.getEvents().onCommand("view", [&bot](TgBot::Message::Ptr message) {
        // Реализация просмотра заявок по табельному номеру из базы данных
        // И отправка результатов пользователю
    });

    // Обработчик команды /add для добавления новой заявки
    bot.getEvents().onCommand("add", [&bot](TgBot::Message::Ptr message) {
        // Реализация добавления новой заявки в базу данных
        // И отправка подтверждения пользователю
    });

    // Обработчик команды /register для регистрации нового работника
    bot.getEvents().onCommand("register", [&bot](TgBot::Message::Ptr message) {
        // Реализация добавления нового работника в базу данных
    });

    // Обработчик команды /enter для входа для работника под своим табельным
    bot.getEvents().onCommand("enter", [&bot](TgBot::Message::Ptr message) {
        // Реализация авторизации работника по табельному
    });

    // Обработчик для изменения статуса заявки
    bot.getEvents().onNonCommandMessage([&bot](TgBot::Message::Ptr message) {
        // Реализация изменения статуса заявки в базе данных
        // И отправка сообщения пользователю о изменении статуса
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
