#ifndef C6XA8396_XXXX_46DB_A44B_CF3E89A72726
#define C6XA8396_XXXX_46DB_A44B_CF3E89A72726

#include <iostream>
#include <random>
#include <time.h>
#include <chrono>
#include <vector>
#include <assert.h>
#include <thread>
#include <atomic>
#include <algorithm>
#include <windows.h>
#include <string.h>

#include "ErlangDistribution.hpp"
#include "TeamMatch.hpp"
#include "Team.hpp"
#include "sqlite3.h"
#include "Player.hpp"
#include "LiteDriver.hpp"
#include "../headers/mysql_driver.h"
#include "../headers/cppconn/statement.h"
#include "../headers/mysql_error.h"
#include "../headers/mysql_connection.h"
#include "../headers/cppconn/resultset.h"

#define DEFAULT_MATCHES 1000
#pragma warning( disable : 4244)

    class SoccerDecoder
    {
    public:
		enum class DatabaseType{
			SQLITE = 0,
			MYSQL,
			MARIADB,
			ORACLE
		};

		SoccerDecoder() = default;
		SoccerDecoder(int argc, char* argv[]);
		void Start();

		std::vector<FieldUnit::Player>& GetPlayersA() { return APlayers; }
		void SetPlayersA(std::vector<FieldUnit::Player> val) { APlayers = val; }

		std::vector<FieldUnit::Player>& GetPlayersB() { return BPlayers; }
		void SetPlayersB(std::vector<FieldUnit::Player> val) { BPlayers = val; }

		FieldUnit::Player& GetGoalKeeperA() { return GoalKeeperA; }
		void SetGoalKeeperA(FieldUnit::Player val) { GoalKeeperA = val; }

		FieldUnit::Player& GetGoalKeeperB() { return GoalKeeperB; }
		void SetGoalKeeperB(FieldUnit::Player val) { GoalKeeperB = val; }

		std::chrono::milliseconds GetDuration() const { return duration; } // Not needed but why not let it chill in here
		void SetDuration(std::chrono::milliseconds val) { duration = val; }

		std::string GetResultJson() {
			return (std::stringstream() << "{\"a\":" << this->aw.load() << "," << "\"b\":" << this->bw.load() << "," << "\"draws\":" << this->dw.load() << "," << "\"duration\":" << this->duration.count() << "}").str();
		}
	private:
		std::atomic<int> aw{ 0 };
		std::atomic<int> dw{ 0 };
		std::atomic<int> bw{ 0 };

		std::chrono::milliseconds duration;
		std::vector<FieldUnit::Player> APlayers;
		std::vector<FieldUnit::Player> BPlayers;
		FieldUnit::Player GoalKeeperA;
		FieldUnit::Player GoalKeeperB;
		unsigned int threadCount;
		unsigned int faceoffs;

		void Func(int chunk);
		std::vector<int> Chunks(const int count);
		std::vector<FieldUnit::Player> FillPlayersSQLITE(DB::LiteDriver& db, int layout);
		std::vector<FieldUnit::Player> FillPlayersMYSQL(sql::ResultSet* res, int layout);
    };

#endif /* C6XA8396_XXXX_46DB_A44B_CF3E89A72726 */