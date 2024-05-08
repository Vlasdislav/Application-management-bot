#pragma once

#include <unordered_map>
#include "UserContext.hpp"


using ConversationMap = std::unordered_map<int64_t, UserContext>;

namespace table {
    inline std::string generate(const std::vector<std::vector<std::string>>& table) {
        std::stringstream result;
        for (int i = 1; i < table.size(); ++i) {
            for (int j = 0; j < table[0].size(); ++j) {
                result << table[0][j] << ": " << table[i][j] << std::endl;
            }
            result << std::endl;
        }
        return result.str();
    }
}
