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

#define DEFAULT_MATCHES 1000
#pragma warning( disable : 4244)

    class SoccerDecoder
    {
    public:
		enum class DatabaseType{
			SQLITE = 0,
			MYSQL,
			MONGODB
		};

		SoccerDecoder() = default;
		SoccerDecoder(int argc, char* argv[]);
		void Start();

		const std::vector<FieldUnit::Player>& GetAPlayers() { return APlayers; }
		void SetAPlayers(std::vector<FieldUnit::Player> val) { APlayers = val; }

		const std::vector<FieldUnit::Player>& GetBPlayers() { return BPlayers; }
		void SetBPlayers(std::vector<FieldUnit::Player> val) { BPlayers = val; }

		std::chrono::milliseconds GetDuration() const { return duration; } // Not needed but why not let it chill in here
		void SetDuration(std::chrono::milliseconds val) { duration = val; }

		std::string GetResultJson() {
			return (std::stringstream() << "{\"a\":\"" << this->aw.load() << "\"," << "\"b\":\"" << this->bw.load() << "\"," << "\"draws\":\"" << this->dw.load() << "\"," << "\"duration\":" << this->duration.count() << "}").str();
		}
	private:
		std::atomic<int> aw{ 0 };
		std::atomic<int> dw{ 0 };
		std::atomic<int> bw{ 0 };

		std::chrono::milliseconds duration;
		std::vector<FieldUnit::Player> APlayers;
		std::vector<FieldUnit::Player> BPlayers;
		unsigned int threadCount;
		unsigned int faceoffs;

		void Func(const std::vector<FieldUnit::Player>& APlayers, const std::vector<FieldUnit::Player>& BPlayers, int chunk);
		std::vector<int> Chunks(const int count);
    };

#endif /* C6XA8396_XXXX_46DB_A44B_CF3E89A72726 */