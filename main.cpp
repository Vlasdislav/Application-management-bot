#include <iostream>
#include "config.hpp"
#include "settings.hpp"
#include "UserContext.hpp"
#include "ManagmentBot.hpp"


int main() {
    ManagmentBot mBot(bot::token, "service_desk.db3");
    database::createDB(mBot.db);
    ConversationMap conversations;
    
    // Обработчик команды /start
    mBot.bot.getEvents().onCommand("start", [&mBot, &conversations](TgBot::Message::Ptr message) {
        mBot.eventStart(conversations, message);
    });

    // Обработчик команды /view для просмотра заявок по табельному номеру
    mBot.bot.getEvents().onCommand("view", [&mBot, &conversations](TgBot::Message::Ptr message) {
        mBot.eventView(conversations, message);
    });

    // Обработчик команды /view_all для просмотра всех заявок
    mBot.bot.getEvents().onCommand("view_all", [&mBot, &conversations](TgBot::Message::Ptr message) {
        mBot.eventViewAll(conversations, message);
    });

    // Обработчик команды /add для добавления новой заявки
    mBot.bot.getEvents().onCommand("add", [&mBot, &conversations](TgBot::Message::Ptr message) {
        mBot.eventAdd(conversations, message);
    });

    // Обработчик команды /work для взятия в работу заявки
    mBot.bot.getEvents().onCommand("work", [&mBot, &conversations](TgBot::Message::Ptr message) {
        mBot.eventWork(conversations, message);
    });

    mBot.bot.getEvents().onNonCommandMessage([&mBot, &conversations](TgBot::Message::Ptr message) {
        if (conversations[message->chat->id].currentCommand == "start") {
            mBot.activityStart(conversations, message);
        }
        if (conversations[message->chat->id].currentCommand == "view") {
            mBot.activityView(conversations, message);
        }
        if (conversations[message->chat->id].currentCommand == "add") {
            mBot.activityAdd(conversations, message);
        }
    });

    try {
        std::cout << "Bot username: " << mBot.bot.getApi().getMe()->username.data() << std::endl;
        mBot.bot.getApi().deleteWebhook();
        TgBot::TgLongPoll longPoll(mBot.bot);
        while (true) {
            std::cout << "Long poll started" << std::endl;
            longPoll.start();
        }
    } catch (TgBot::TgException& e) {
        std::cerr << "error: " << e.what() << std::endl;
    }
    return 0;
}
