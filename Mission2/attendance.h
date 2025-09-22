#include <string>
#include <vector>
#include <map>
#include <memory>

using namespace std;

/* Constants */
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

/* Grade Score */
const int GOLD_SCORE = 50;
const int SILVER_SCORE = 30;

/* Grade */
const int GOLD_GRADE = 1;
const int SILVER_GRADE = 2;
const int NORMAL_GRADE = 0;

/* Player Class */
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

/* Global Player Map */
extern map<string, Player> playerMap;

/* Score Policy Interface */
class IScorePolicy {
public:
    virtual int calculateScore(const vector<string>& attendance) const = 0;
    virtual ~IScorePolicy() = default;
};

/* Grade Policy Interface */
class IGradePolicy {
public:
    virtual int determineGrade(int score) const = 0;
    virtual ~IGradePolicy() = default;
};

/* Default Score Policy */
class DefaultScorePolicy : public IScorePolicy {
public:
    int calculateScore(const vector<string>& attendance) const override;
};

/* Default Grade Policy */
class DefaultGradePolicy : public IGradePolicy {
public:
    int determineGrade(int score) const override;
};

/* Attendance Manager */
class AttendanceManager {
    vector<Player> players;
    unique_ptr<IScorePolicy> scorePolicy;
    unique_ptr<IGradePolicy> gradePolicy;

public:
    AttendanceManager(unique_ptr<IScorePolicy> sp, unique_ptr<IGradePolicy> gp);
    void addPlayer(const Player& p);
    void process();
    const vector<Player>& getPlayers() const;

private:
    bool checkElimination(const Player& p);
};

/* Utility Functions */
void printResult(const vector<Player>& players);
void printRemovedPlayer(const vector<Player>& players);
