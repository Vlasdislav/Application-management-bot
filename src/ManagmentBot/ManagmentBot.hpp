#pragma once

#include "config.hpp"
#include "settings.hpp"


class ManagmentBot {
public:
    ManagmentBot() = default;

    ManagmentBot(const std::string&,
                    const std::string&);

    void eventStart(ConversationMap&,
                    const TgBot::Message::Ptr&);

    void eventView(ConversationMap&,
                    const TgBot::Message::Ptr&);

    void eventViewAll(ConversationMap&,
                    const TgBot::Message::Ptr&);

    void eventAdd(ConversationMap&,
                    const TgBot::Message::Ptr&);

    void eventWork(ConversationMap&,
                const TgBot::Message::Ptr&);

    void activityStart(ConversationMap&,
                    const TgBot::Message::Ptr&);

    void activityView(ConversationMap&,
                    const TgBot::Message::Ptr&);
    
    void activityViewAll(ConversationMap&,
                    const TgBot::Message::Ptr&);

    void activityAdd(ConversationMap&,
                    const TgBot::Message::Ptr&);

    void activityWork(ConversationMap&,
                const TgBot::Message::Ptr&);
                
public:
    TgBot::Bot bot;
    SQLite::Database db;
};
