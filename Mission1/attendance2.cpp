#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <algorithm>
#include "gmock/gmock.h"
#include "../Mission2/attendance.h"

using namespace std;

int DefaultScorePolicy::calculateScore(const vector<string>& attendance) const {
    int score = 0, wed = 0, weekend = 0;
    for (const auto& day : attendance) {
        if (day == WEDNESDAY) { 
            score += WEDNESDAY_SCORE; 
            wed++; 
        }
        else if (day == SATURDAY || day == SUNDAY) { 
            score += WEEKEND_SCORE; 
            weekend++; 
        }
        else score += NORMAL_SCORE;
    }
    if (wed >= WEDNESDAY_ATTENDANCE_COUNT_FOR_BONUS) score += BONUS_WEDNESDAY_SCORE;
    if (weekend >= WEEKEND_ATTENDANCE_COUNT_FOR_BONUS) score += BONUS_WEEKEND_SCORE;
    return score;
}

int DefaultGradePolicy::determineGrade(int score) const {
    if (score >= GOLD_SCORE) return GOLD_GRADE;
    if (score >= SILVER_SCORE) return SILVER_GRADE;
    return NORMAL_GRADE;
}

AttendanceManager::AttendanceManager(unique_ptr<IScorePolicy> sp, unique_ptr<IGradePolicy> gp)
    : scorePolicy(move(sp)), gradePolicy(move(gp)) {}

void AttendanceManager::addPlayer(const Player& p) {
    players.push_back(p);
}

void AttendanceManager::process() {
    for (auto& p : players) {
        p.score = scorePolicy->calculateScore(p.attendance);
        p.grade = gradePolicy->determineGrade(p.score);
        p.eliminationCandidate = checkElimination(p);
    }
}

const vector<Player>& AttendanceManager::getPlayers() const {
    return players;
}

bool AttendanceManager::checkElimination(const Player& p) {
    if (p.grade != NORMAL_GRADE) return false;
    bool wed = false, weekend = false;
    for (const auto& d : p.attendance) {
        if (d == WEDNESDAY) wed = true;
        if (d == SATURDAY || d == SUNDAY) weekend = true;
    }
    return !(wed || weekend);
}

void createPlayerMap(std::map<std::string, Player>& playerMap) {
    ifstream fin{ RESOURCE_FILE };
    int playerId = 1;
    for (int i = 0; i < MAX_NUM_OF_RESOURCE_DATA; i++) {
        string name, day;
        fin >> name >> day;

        if (playerMap.find(name) != playerMap.end()) {
            playerMap[name].attendance.push_back(day);
        }
        else {
            playerMap[name] = Player(playerId++, name, { day });
        }
    }
}

void printResult(const vector<Player>& players) {
    vector<const Player*> playerPtrs;
    for (const auto& p : players) {
        playerPtrs.push_back(&p);
    }

    sort(playerPtrs.begin(), playerPtrs.end(), [](const Player* a, const Player* b) {
        return a->id < b->id; 
        });

    for (const auto* p : playerPtrs) {
        cout << "NAME : " << p->name << ", ";
        cout << "POINT : " << p->score << ", ";
        cout << "GRADE : ";

        if (p->grade == GOLD_GRADE) {
            cout << GOLD << "\n";
        }
        else if (p->grade == SILVER_GRADE) {
            cout << SILVER << "\n";
        }
        else {
            cout << NORMAL << "\n";
        }
    }
}

void printRemovedPlayer(const vector<Player>& players) {
    std::cout << "\n";
    std::cout << "Removed player\n";
    std::cout << "==============\n";
    for (auto& p : players) {
        if (p.eliminationCandidate) {
            std::cout << p.name << "\n";
        }
    }
}

int main() {
#ifndef _DEBUG
    auto manager = AttendanceManager(
        make_unique<DefaultScorePolicy>(),
        make_unique<DefaultGradePolicy>()
    );

    map<string, Player> playerMap;
    createPlayerMap(playerMap);
    for (const auto& pair : playerMap) {
        manager.addPlayer(pair.second);
    }
    manager.process();

    printResult(manager.getPlayers());
    printRemovedPlayer(manager.getPlayers());
#else
    ::testing::InitGoogleMock();
    return RUN_ALL_TESTS();
#endif
}


