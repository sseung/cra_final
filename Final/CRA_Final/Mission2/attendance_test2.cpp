#include "gmock/gmock.h"
#include <vector>
#include <string>
#include <memory>
#include <iostream>
#include "../Mission2/attendance.h"

using namespace std;

class MockScorePolicy : public IScorePolicy {
public:
    MOCK_METHOD(int, calculateScore, (const vector<string>& attendance), (const, override));
};

class MockGradePolicy : public IGradePolicy {
public:
    MOCK_METHOD(int, determineGrade, (int score), (const, override));
};

TEST(AttendanceManagerTest, AddPlayerAndProcess) {
    auto mockScorePolicy = make_unique<MockScorePolicy>();
    auto mockGradePolicy = make_unique<MockGradePolicy>();

    EXPECT_CALL(*mockScorePolicy, calculateScore(::testing::_))
        .WillRepeatedly(::testing::Return(10));
    EXPECT_CALL(*mockGradePolicy, determineGrade(::testing::_))
        .WillRepeatedly(::testing::Return(SILVER_GRADE));

    AttendanceManager manager(move(mockScorePolicy), move(mockGradePolicy));

    Player player1(1, "Alice", { MONDAY, WEDNESDAY });
    Player player2(2, "Bob", { SATURDAY, SUNDAY });
    manager.addPlayer(player1);
    manager.addPlayer(player2);

    manager.process();

    //
    const auto& players = manager.getPlayers();
    ASSERT_EQ(players.size(), 2);
    EXPECT_EQ(players[0].name, "Alice");
    EXPECT_EQ(players[0].score, 10);
    EXPECT_EQ(players[0].grade, SILVER_GRADE);
    EXPECT_EQ(players[1].name, "Bob");
    EXPECT_EQ(players[1].score, 10);
    EXPECT_EQ(players[1].grade, SILVER_GRADE);
}

TEST(AttendanceManagerTest, ProcessPlayers) {
    auto mockScorePolicy = make_unique<MockScorePolicy>();
    auto mockGradePolicy = make_unique<MockGradePolicy>();

    EXPECT_CALL(*mockScorePolicy, calculateScore(::testing::_))
        .WillOnce(::testing::Return(100))
        .WillOnce(::testing::Return(50));

    EXPECT_CALL(*mockGradePolicy, determineGrade(100))
        .WillOnce(::testing::Return(GOLD_GRADE));
    EXPECT_CALL(*mockGradePolicy, determineGrade(50))
        .WillOnce(::testing::Return(SILVER_GRADE));

    AttendanceManager manager(move(mockScorePolicy), move(mockGradePolicy));

    Player player1(1, "Alice", { "Monday", "Wednesday", "Saturday" });
    Player player2(2, "Bob", { "Tuesday", "Sunday" });

    manager.addPlayer(player1);
    manager.addPlayer(player2);

    manager.process();

    const auto& players = manager.getPlayers();
    EXPECT_EQ(players[0].score, 100);
    EXPECT_EQ(players[0].grade, GOLD_GRADE);
    EXPECT_EQ(players[1].score, 50);
    EXPECT_EQ(players[1].grade, SILVER_GRADE);
}

TEST(AttendanceManagerTest, CheckElimination) {
    // Arrange
    auto mockScorePolicy = make_unique<MockScorePolicy>();
    auto mockGradePolicy = make_unique<MockGradePolicy>();

    EXPECT_CALL(*mockScorePolicy, calculateScore(::testing::_))
        .WillRepeatedly(::testing::Return(5));
    EXPECT_CALL(*mockGradePolicy, determineGrade(::testing::_))
        .WillRepeatedly(::testing::Return(NORMAL_GRADE));

    AttendanceManager manager(move(mockScorePolicy), move(mockGradePolicy));

    Player player1(1, "Charlie", { MONDAY, TUESDAY });
    Player player2(2, "Dave", { WEDNESDAY, SATURDAY });
    manager.addPlayer(player1);
    manager.addPlayer(player2);

    manager.process();

    // Act and Assert
    const auto& players = manager.getPlayers();
    ASSERT_EQ(players.size(), 2);
    EXPECT_TRUE(players[0].eliminationCandidate);
    EXPECT_FALSE(players[1].eliminationCandidate);
}

TEST(AttendanceManagerTest, PrintResult) {
    auto mockScorePolicy = make_unique<MockScorePolicy>();
    auto mockGradePolicy = make_unique<MockGradePolicy>();

    AttendanceManager manager(move(mockScorePolicy), move(mockGradePolicy));

    Player player1(1, "Alice", { "Monday", "Wednesday", "Saturday" });
    Player player2(2, "Bob", { "Tuesday", "Sunday" });

    manager.addPlayer(player1);
    manager.addPlayer(player2);

    testing::internal::CaptureStdout();
    printResult(manager.getPlayers());
    string output = testing::internal::GetCapturedStdout();

    EXPECT_THAT(output, ::testing::HasSubstr("NAME : Alice"));
    EXPECT_THAT(output, ::testing::HasSubstr("NAME : Bob"));
}

TEST(AttendanceManagerTest, CalculateScore) {
    DefaultScorePolicy scorePolicy;

    vector<string> attendance1 = { MONDAY, WEDNESDAY, SATURDAY };
    vector<string> attendance2 = { WEDNESDAY, WEDNESDAY, SUNDAY, SUNDAY };

    int score1 = scorePolicy.calculateScore(attendance1);
    int score2 = scorePolicy.calculateScore(attendance2);

    EXPECT_EQ(score1, NORMAL_SCORE + WEDNESDAY_SCORE + WEEKEND_SCORE);
}

TEST(AttendanceManagerTest, PrintRemovedPlayer) {
    Player player1(1, "Alice", { MONDAY, TUESDAY });
    Player player2(2, "Bob", { WEDNESDAY, SATURDAY });
    Player player3(3, "Charlie", { MONDAY, TUESDAY });

    vector<Player> players = { player1, player2, player3 };

    testing::internal::CaptureStdout();
    printRemovedPlayer(players);
    string output = testing::internal::GetCapturedStdout();

    EXPECT_THAT(output, ::testing::HasSubstr("Removed player"));
}

TEST(AttendanceManagerTest, DetermineGrade) {
    DefaultGradePolicy gradePolicy;

    EXPECT_EQ(gradePolicy.determineGrade(60), GOLD_GRADE);
    EXPECT_EQ(gradePolicy.determineGrade(40), SILVER_GRADE);
    EXPECT_EQ(gradePolicy.determineGrade(20), NORMAL_GRADE);
}
