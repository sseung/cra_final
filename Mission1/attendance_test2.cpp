#include "gmock/gmock.h"
#include <vector>
#include <string>
#include <memory>

#include "attendance.h" 

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
