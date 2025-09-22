#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <gmock/gmock.h>

using namespace std;

const string RESOURCE_FILE = "attendance_weekday_500.txt";

const int MAX_NUM_OF_RESOURCE_DATA = 500;
const int MAX_NUM_OF_PLAYERS = 20;
const int MAX_NUM_OF_DAYS = 7;

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

struct Player {
public:
	int id;
	string name;
	vector<string> attendanceDays;
	bool candidateForRemoval;
	int score;
	int grade;
};

//std::vector<Player> players;
std::map<std::string, Player> players;

enum class Day {
	Monday,
	Tuesday,
	Wednesday,
	Thursday,
	Friday,
	Saturday,
	Sunday
};

std::map<std::string, Day> stringToDay = {
	{MONDAY, Day::Monday},
	{TUESDAY, Day::Tuesday},
	{WEDNESDAY, Day::Wednesday},
	{THURSDAY, Day::Thursday},
	{FRIDAY, Day::Friday},
	{SATURDAY, Day::Saturday},
	{SUNDAY, Day::Sunday}
};

std::map<std::string, int> stringToScore = {
	{MONDAY, NORMAL_SCORE},
	{TUESDAY, NORMAL_SCORE},
	{WEDNESDAY, WEDNESDAY_SCORE},
	{THURSDAY, NORMAL_SCORE},
	{FRIDAY, NORMAL_SCORE},
	{SATURDAY, WEEKEND_SCORE},
	{SUNDAY, WEEKEND_SCORE}
};

map<string, int> MapOfplayerNameAndId;
int NUM_OF_USERS = 0;

//dat[사용자ID][요일]
int countOfDay[MAX_NUM_OF_PLAYERS][MAX_NUM_OF_DAYS];
int points[MAX_NUM_OF_PLAYERS];
int grade[MAX_NUM_OF_PLAYERS];
string names[MAX_NUM_OF_PLAYERS];

void createPlayerId(std::string& name) {
	bool isFirstAppearance = (MapOfplayerNameAndId.count(name) == 0);
	if (isFirstAppearance) {
		MapOfplayerNameAndId.insert({ name, ++NUM_OF_USERS });
		if (name == "Daisy") {
			int debug = 1;
		}
		names[NUM_OF_USERS] = name;
	}
}

void increaseDaysForPlayer(int playerId, string& day) {
	countOfDay[playerId][static_cast<int>(stringToDay[day])] += 1;
}

void increaseScorePerDay(int playerId, std::string& day) {
	points[playerId] += stringToScore[day];
}

void calculatePoint(string name, string day) {
	// 디버깅용
	if (name == "Daisy") {
		int debug = 1;
	}
	int playerId = MapOfplayerNameAndId[name];
	increaseDaysForPlayer(playerId, day);
	increaseScorePerDay(playerId, day);
}

void inputDataFromFile() {
	ifstream fin{ RESOURCE_FILE };
	for (int i = 0; i < MAX_NUM_OF_RESOURCE_DATA; i++) {
		string name, day;
		fin >> name >> day;
		createPlayerId(name);
		calculatePoint(name, day);
	}
}

void input() {
	ifstream fin{ RESOURCE_FILE };
	for (int i = 0; i < MAX_NUM_OF_RESOURCE_DATA; i++) {
		string name, day;
		fin >> name >> day;

		if (players.find(name) != players.end()) {
			players[name].attendanceDays.push_back(day);
			continue;
		}

		std::shared_ptr<Player> player = make_shared<Player>();
		player->id = i + 1;
		player->name = name;
		player->attendanceDays.push_back(day);
	}
}

void calculateBonusPoints() {
	for (int i = 1; i <= NUM_OF_USERS; i++) {
		int count_wed_attendance = countOfDay[i][static_cast<int>(Day::Wednesday)];
		if (count_wed_attendance >= WEDNESDAY_ATTENDANCE_COUNT_FOR_BONUS) {
			points[i] += BONUS_WEDNESDAY_SCORE;
		}

		int count_weekend_attendance = countOfDay[i][static_cast<int>(Day::Saturday)] + countOfDay[i][static_cast<int>(Day::Sunday)];
		if (count_weekend_attendance >= WEEKEND_ATTENDANCE_COUNT_FOR_BONUS) {
			points[i] += BONUS_WEEKEND_SCORE;
		}
	}
}

void calculateGrades() {
	for (int i = 1; i <= NUM_OF_USERS; i++) {
		if (points[i] >= GOLD_SCORE) {
			grade[i] = GOLD_GRADE;
		}
		else if (points[i] >= SILVER_SCORE) {
			grade[i] = SILVER_GRADE;
		}
		else {
			grade[i] = NORMAL_GRADE;
		}
	}
}

void print() {
	for (int i = 1; i <= NUM_OF_USERS; i++) {
		cout << "NAME : " << names[i] << ", ";
		cout << "POINT : " << points[i] << ", ";
		cout << "GRADE : ";

		if (grade[i] == GOLD_GRADE) {
			cout << GOLD << "\n";
		}
		else if (grade[i] == SILVER_GRADE) {
			cout << SILVER << "\n";
		}
		else {
			cout << NORMAL << "\n";
		}
	}
}

bool needToRemove(int i) {
	int count_wed_attendance = countOfDay[i][static_cast<int>(Day::Wednesday)];
	int count_weekend_attendance = countOfDay[i][static_cast<int>(Day::Saturday)] + countOfDay[i][static_cast<int>(Day::Sunday)];

	bool isNormalGrade = (grade[i] == NORMAL_GRADE);
	bool isWedAttendZero = (count_wed_attendance == 0);
	bool isWeekendAttendZero = (count_weekend_attendance == 0);

	return isNormalGrade && isWedAttendZero && isWeekendAttendZero;
}

void printRemovedPlayer() {
	std::cout << "\n";
	std::cout << "Removed player\n";
	std::cout << "==============\n";
	for (int i = 1; i <= NUM_OF_USERS; i++) {
		if (needToRemove(i)) {
			std::cout << names[i] << "\n";
		}
	}
}

int main() {
#ifndef _DEBUG
	inputDataFromFile();
	calculateBonusPoints();
	calculateGrades();
	print();
	printRemovedPlayer();
#else // _RELEASE
	testing::InitGoogleTest();
	return RUN_ALL_TESTS();
#endif
}

