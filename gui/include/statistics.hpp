#ifndef PLATFORMERGAMECPP_STATISTICS_HPP
#define PLATFORMERGAMECPP_STATISTICS_HPP

#include "game.hpp"
#include "../../tools/json.hpp"

namespace Platformer {
class Statistics {
    int timer = 0;
    int deathCount = 0;
    int lastTimer = INT_MAX;
    bool isNewBestTime = false;
    bool isNewBestDeathCount = false;
    bool isFirstTime = true;
    std::string levelName;
    const std::string statisticsFilepath = "../model/statistics/statistics.json";

    [[nodiscard]] static std::string utilityParsingLevelName(const std::string &levelFilepath) ;
    [[nodiscard]] static std::string utilityTimerToText(int timer_) ;

public:

    explicit Statistics(const std::string &levelFilepath);

    void updateTimer();
    void updateDeathCount();

    [[nodiscard]] int getDeathCount() const;
    [[nodiscard]] bool getIsNewBestTime() const;
    [[nodiscard]] bool getIsNewBestDeathCount() const;
    [[nodiscard]] bool getIsFirstTime() const;
    [[nodiscard]] std::string getTextTimer() const;
    [[nodiscard]] std::string getTextLastTimer() const;

    void changeLevelFilepath(const std::string &newLevelFilepath);
    void updateLocalStatistics();
    void initializeLevelStatistics();

    [[nodiscard]] bool checkForExistingLevelStatistics();
};
}

#endif  // PLATFORMERGAMECPP_STATISTICS_HPP