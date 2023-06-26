#include "statistics.hpp"
#include <climits>
#include <fstream>

using json = nlohmann::json;

namespace Platformer {
Statistics::Statistics(const std::string &levelFilepath) {
    levelName = utilityParsingLevelName(levelFilepath);
}

[[nodiscard]] std::string Statistics::utilityParsingLevelName(
    const std::string &levelFilepath
) {
    int filepathLength = static_cast<int>(levelFilepath.size());
    int secondDotPos = filepathLength - 1;
    for (int i = secondDotPos; levelFilepath[i] != '.' && i >= 0; i--) {
        secondDotPos--;
    }

    int firstDotPos = secondDotPos;
    for (int i = firstDotPos; levelFilepath[i] != '/' && i >= 0; i--) {
        firstDotPos--;
    }

    return levelFilepath.substr(
        firstDotPos + 1, secondDotPos - firstDotPos - 1
    );
}

std::string Statistics::utilityTimerToText(const int timer_) {
    int frameRate = 30;  // TODO need to bound with framerate const.
    int milli = ((timer_ % frameRate) * 1000) / 60;
    int sec = (timer_ % (60 * frameRate)) / frameRate;
    int min = timer_ / (60 * frameRate);

    std::string milliToText = std::to_string(milli);

    if (milli < 10) {
        milliToText = "00" + milliToText;
    } else if (milli < 100) {
        milliToText = "0" + milliToText;
    }

    return std::to_string(min) + ":" + std::to_string(sec) + "." + milliToText;
}

void Statistics::updateTimer() {
    timer++;
}

void Statistics::updateDeathCount() {
    deathCount++;
}

[[nodiscard]] int Statistics::getDeathCount() const {
    return deathCount;
}

[[nodiscard]] bool Statistics::getIsNewBestTime() const {
    return isNewBestTime;
}

[[nodiscard]] bool Statistics::getIsNewBestDeathCount() const {
    return isNewBestDeathCount;
}

[[nodiscard]] bool Statistics::getIsFirstTime() const {
    return isFirstTime;
}

std::string Statistics::getTextTimer() const {
    return utilityTimerToText(timer);
}

std::string Statistics::getTextLastTimer() const {
    return utilityTimerToText(lastTimer);
}

void Statistics::changeLevelFilepath(const std::string &newLevelFilepath) {
    levelName = utilityParsingLevelName(newLevelFilepath);
}

void Statistics::updateLocalStatistics() {
    std::ifstream i(statisticsFilepath);
    json statistics;
    statistics = json::parse(i);
    i.close();

    int oldBestTimer = statistics[levelName]["best time"];
    int oldBestDeathCount = statistics[levelName]["best death count"];
    bool firstTimeFlag = statistics[levelName]["first time flag"];
    int oldLastTimer = statistics[levelName]["last time"];
    lastTimer = oldLastTimer;

    if (oldBestTimer > timer) {
        isNewBestTime = true;
    }

    if (oldBestDeathCount > deathCount) {
        isNewBestDeathCount = true;
    }

    if (firstTimeFlag) {
        isFirstTime = true;
    } else {
        isFirstTime = false;
    }

    int m_timer = timer;
    int m_deathCount = deathCount;
    int newBestTimer = std::min(oldBestTimer, m_timer);
    int newBestDeathCount = std::min(oldBestDeathCount, m_deathCount);

    std::ofstream o(statisticsFilepath, std::ios::trunc);

    statistics[levelName]["best time"] = newBestTimer;
    statistics[levelName]["best death count"] = newBestDeathCount;
    statistics[levelName]["first time flag"] = false;
    statistics[levelName]["last time"] = timer;

    o << std::setw(4) << statistics << std::endl;
    o.close();
}

void Statistics::initializeLevelStatistics() {
    json statistics;
    std::ifstream i(statisticsFilepath);
    i >> statistics;

    statistics[levelName]["best time"] = INT_MAX;
    statistics[levelName]["best death count"] = INT_MAX;
    statistics[levelName]["first time flag"] = true;
    statistics[levelName]["last time"] = INT_MAX;

    std::ofstream o(statisticsFilepath);
    o << std::setw(4) << statistics << std::endl;
    o.close();
}

bool Statistics::checkForExistingLevelStatistics() {
    std::ifstream i(statisticsFilepath);
    json statistics;
    statistics = json::parse(i);
    i.close();
    return statistics.contains(levelName);
}
}  // namespace Platformer
