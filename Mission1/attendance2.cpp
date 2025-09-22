#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>
#include <map>
#include "gmock/gmock.h"

#include "attendance.h"

using namespace std;

const string RESOURCE_FILE = "attendance_weekday_500.txt";
const int MAX_NUM_OF_RESOURCE_DATA = 500;

const string MONDAY = "monday";
const string TUESDAY = "tuesday";
const string WEDNESDAY = "wednesday";
const string THURSDAY = "thursday";
const string FRIDAY = "friday";
const string SATURDAY = "saturday";
const string SUNDAY = "sunday";

const string GOLD = "GOLD";
const string SILVER = "SILVER";
const string NORMAL = "NORMAL";

/* Basic Score per days */
const int WEDNESDAY_SCORE = 3;
const int WEEKEND_SCORE = 2;
const int NORMAL_SCORE = 1;

/* Bonus Score */
const int WEDNESDAY_ATTENDANCE_COUNT_FOR_BONUS = 10;
const int BONUS_WEDNESDAY_SCORE = 10;
const int WEEKEND_ATTENDANCE_COUNT_FOR_BONUS = 10;
const int BONUS_WEEKEND_SCORE = 10;

/* Grade Score*/
const int GOLD_SCORE = 50;
const int SILVER_SCORE = 30;

/* Grade */
const int GOLD_GRADE = 1;
const int SILVER_GRADE = 2;
const int NORMAL_GRADE = 0;

class Player {
public:
    int id;
    string name;
    vector<string> attendance;
    int score = 0;
    int grade;
    bool eliminationCandidate = false;

    Player() = default;

    Player(int i, const string& n, const vector<string>& att)
        : id(i), name(n), attendance(att) {}
};

map<string, Player> playerMap;

class IScorePolicy {
public:
    virtual int calculateScore(const vector<string>& attendance) const = 0;
    virtual ~IScorePolicy() = default;
};

class IGradePolicy {
public:
    virtual int determineGrade(int score) const = 0;
    virtual ~IGradePolicy() = default;
};

class DefaultScorePolicy : public IScorePolicy {
public:
    int calculateScore(const vector<string>& attendance) const override {
        int score = 0, wed = 0, weekend = 0;
        for (const auto& day : attendance) {
            if (day == WEDNESDAY) { score += WEDNESDAY_SCORE; wed++; }
            else if (day == SATURDAY || day == SUNDAY) { score += WEEKEND_SCORE; weekend++; }
            else score += NORMAL_SCORE;
        }
        if (wed >= WEDNESDAY_ATTENDANCE_COUNT_FOR_BONUS) score += BONUS_WEDNESDAY_SCORE;
        if (weekend >= WEEKEND_ATTENDANCE_COUNT_FOR_BONUS) score += BONUS_WEEKEND_SCORE;
        return score;
    }
};

class DefaultGradePolicy : public IGradePolicy {
public:
    int determineGrade(int score) const override {
        if (score >= 50) return GOLD_GRADE;
        if (score >= 30) return SILVER_GRADE;
        return NORMAL_GRADE;
    }
};

class AttendanceManager {
    vector<Player> players;
    unique_ptr<IScorePolicy> scorePolicy;
    unique_ptr<IGradePolicy> gradePolicy;

public:
    AttendanceManager(unique_ptr<IScorePolicy> sp, unique_ptr<IGradePolicy> gp)
        : scorePolicy(move(sp)), gradePolicy(move(gp)) {}

    void addPlayer(const Player& p) { players.push_back(p); }

    void process() {
        for (auto& p : players) {
            p.score = scorePolicy->calculateScore(p.attendance);
            p.grade = gradePolicy->determineGrade(p.score);
            p.eliminationCandidate = checkElimination(p);
        }
    }

    const vector<Player>& getPlayers() const { return players; }

private:
    bool checkElimination(const Player& p) {
        if (p.grade != NORMAL_GRADE) return false;
        bool wed = false, weekend = false;
        for (const auto& d : p.attendance) {
            if (d == WEDNESDAY) wed = true;
            if (d == SATURDAY || d == SUNDAY) weekend = true;
        }
        return !(wed || weekend);
    }
};

void printResult(const vector<Player>& players) {
    for (auto& p: players) {
        cout << "NAME : " << p.name << ", ";
        cout << "POINT : " << p.score << ", ";
        cout << "GRADE : ";

        if (p.grade == GOLD_GRADE) {
            cout << GOLD << "\n";
        }
        else if (p.grade == SILVER_GRADE) {
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
#if 0
    auto manager = AttendanceManager(
        make_unique<DefaultScorePolicy>(),
        make_unique<DefaultGradePolicy>()
    );

    ifstream fin{ RESOURCE_FILE };
    if (!fin.is_open()) {
        cerr << "Error: Unable to open file " << RESOURCE_FILE << endl;
        return 1;
    }

    int playerId = 1;
    for (int i = 0; i < MAX_NUM_OF_RESOURCE_DATA; i++) {
        string name, day;
        fin >> name >> day;

        if (fin.fail()) break; 

        if (playerMap.find(name) != playerMap.end()) {         
            playerMap[name].attendance.push_back(day);
        }
        else {        
            playerMap[name] = Player(playerId++, name, { day });
        }
    }

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