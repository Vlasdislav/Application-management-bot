#pragma once


struct UserContext {
    int64_t id = -1;
    std::string name;
    std::string serviceNumber;
    std::string currentCommand = "start";
};
