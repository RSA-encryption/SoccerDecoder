#include "../headers/SoccerDecoder.hpp"

#define SQL_LAYOUTA "SELECT `layout` FROM `teams` WHERE `id` LIKE 1"
#define SQL_LAYOUTB "SELECT `layout` FROM `teams` WHERE `id` LIKE 2"
#define SQL_PLAYERSA "SELECT * FROM `players` WHERE `team_id` LIKE 1 ORDER BY `rating` ASC LIMIT 11"
#define SQL_PLAYERSB "SELECT * FROM `players` WHERE `team_id` LIKE 2 ORDER BY `rating` ASC LIMIT 11"
#define SQL_USE_DB "USE soccer"

SoccerDecoder::SoccerDecoder(int argc, char* argv[]) {
	auto t = std::chrono::high_resolution_clock::now();
	unsigned int seed = std::chrono::system_clock::now().time_since_epoch().count();
	this->threadCount = std::thread::hardware_concurrency();
	this->duration = std::chrono::duration_cast<std::chrono::seconds>(t - t);
	if (argc >= 3) {
		DatabaseType x = static_cast<enum class DatabaseType>(std::stoi(argv[2]));
		this->faceoffs = std::stoi(argv[1]);
		switch (x) { // NOTE: Original idea is every player will have pre determined position but I wanted to get this part quickly done, I might change it later
			case DatabaseType::SQLITE : { 
				DB::LiteDriver db(argv[3]);

				db.execSingleStmt(SQL_LAYOUTA, true);
				int layoutA = db.getIntegerFromRow(db.getStorageRefference(), "layout", 0);
				this->layoutA = static_cast<Match::Team::Formations>(layoutA);

				db.execSingleStmt(SQL_LAYOUTB, true);
				int layoutB = db.getIntegerFromRow(db.getStorageRefference(), "layout", 0);
				this->layoutB = static_cast<Match::Team::Formations>(layoutB);

				db.execSingleStmt(SQL_PLAYERSA, true);
				this->SetPlayersA(this->FillPlayersSQLITE(db, layoutA));
				this->SetGoalKeeperA(FieldUnit::Player(db.getDoubleFromRow(db.getStorageRefference(), "rating", 10), db.getDoubleFromRow(db.getStorageRefference(), "stamina", 10), db.getDoubleFromRow(db.getStorageRefference(), "age", 10), db.getDoubleFromRow(db.getStorageRefference(), "experience", 10), db.getStringFromRow(db.getStorageRefference(), "name", 10), FieldUnit::Player::PositionType::GOALKEEPER));

				db.execSingleStmt(SQL_PLAYERSB, true);
				this->SetPlayersB(this->FillPlayersSQLITE(db, layoutB));
				this->SetGoalKeeperB(FieldUnit::Player(db.getDoubleFromRow(db.getStorageRefference(), "rating", 10), db.getDoubleFromRow(db.getStorageRefference(), "stamina", 10), db.getDoubleFromRow(db.getStorageRefference(), "age", 10), db.getDoubleFromRow(db.getStorageRefference(), "experience", 10), db.getStringFromRow(db.getStorageRefference(), "name", 10), FieldUnit::Player::PositionType::GOALKEEPER));
				break;
			}
			case DatabaseType::MARIADB:
			case DatabaseType::MYSQL: {
				sql::Connection* conn = nullptr;
				sql::mysql::MySQL_Driver* driver = nullptr;
				sql::Statement* stmt = nullptr;
				sql::ResultSet* resLayout = nullptr;
				sql::ResultSet* resPlayers = nullptr;
				try{
					driver = sql::mysql::get_mysql_driver_instance();
					if (x == DatabaseType::MARIADB) { // Could be in config/argv, will probably move it later on when I have time to cleanup
						conn = driver->connect("tcp://127.0.0.1:3306", "root", "securepassword");
					}
					else {
						conn = driver->connect("tcp://127.0.0.1:4306", "root", "securepassword");
					}
					stmt = conn->createStatement();
					stmt->execute(SQL_USE_DB);
					resPlayers = stmt->executeQuery(SQL_PLAYERSA);
					resLayout = stmt->executeQuery(SQL_LAYOUTA);
					resLayout->first();
					this->SetPlayersA(this->FillPlayersMYSQL(resPlayers, resLayout->getInt("layout")));
					resPlayers->last();
					this->SetGoalKeeperA(FieldUnit::Player(static_cast<double>(resPlayers->getDouble("rating")), static_cast<double>(resPlayers->getDouble("stamina")), static_cast<double>(resPlayers->getDouble("age")), static_cast<double>(resPlayers->getDouble("experience")), resPlayers->getString("name"), FieldUnit::Player::PositionType::GOALKEEPER));

					resPlayers = stmt->executeQuery(SQL_PLAYERSB);
					resLayout = stmt->executeQuery(SQL_LAYOUTB);
					resLayout->first();
					this->SetPlayersB(this->FillPlayersMYSQL(resPlayers, resLayout->getInt("layout")));
					resPlayers->last();
					this->SetGoalKeeperB(FieldUnit::Player(static_cast<double>(resPlayers->getDouble("rating")), static_cast<double>(resPlayers->getDouble("stamina")), static_cast<double>(resPlayers->getDouble("age")), static_cast<double>(resPlayers->getDouble("experience")), resPlayers->getString("name"), FieldUnit::Player::PositionType::GOALKEEPER));
				}
				catch (std::exception& e){
					std::cout << e.what();
					exit(-1);
				}
				break;
			}
		}
	} else { // Default sample data
		this->layoutA = Match::Team::Formations::_4_3_3;
		this->layoutB = Match::Team::Formations::_4_3_3;

		this->SetPlayersA({
			FieldUnit::Player(8.5, 10.0, 34, 1.0, "Kanker", FieldUnit::Player::PositionType::MIDFIELDER),
			FieldUnit::Player(6.5, 10.0, 34, 1.0, "asda", FieldUnit::Player::PositionType::MIDFIELDER),
			FieldUnit::Player(6.5, 10.0, 34, 10.0, "Epfdgfdgfgd", FieldUnit::Player::PositionType::MIDFIELDER),
			FieldUnit::Player(7.5, 10.0, 34, 40.0, "Epxcxxcxcic", FieldUnit::Player::PositionType::MIDFIELDER),
			FieldUnit::Player(8.5, 10.0, 34, 4.0, "Eqwqwwqwqwpic", FieldUnit::Player::PositionType::STRIKERS),
			FieldUnit::Player(8.5, 10.0, 34, 40.0, "Epicgfdgfdgfdg", FieldUnit::Player::PositionType::STRIKERS),
			FieldUnit::Player(6.5, 10.0, 34, 40.0, "Eczxxczxczxczpic", FieldUnit::Player::PositionType::STRIKERS),
			FieldUnit::Player(0.5, 10.0, 34, 4.0, "Epqqqqqqqqqic", FieldUnit::Player::PositionType::DEFENDER),
			FieldUnit::Player(4.5, 10.0, 34, 4.0, "Epaaaaaaaaic", FieldUnit::Player::PositionType::DEFENDER),
			FieldUnit::Player(8.5, 10.0, 34, 40.0, "Epzzzzzzzzzzic", FieldUnit::Player::PositionType::DEFENDER) });
		this->SetGoalKeeperA(FieldUnit::Player(9.5, 10.0, 34, 74.0, "Pot", FieldUnit::Player::PositionType::GOALKEEPER));

		this->SetPlayersB({
			FieldUnit::Player(8.5, 10.0, 34, 1.0, "Kanker", FieldUnit::Player::PositionType::MIDFIELDER),
			FieldUnit::Player(8.5, 10.0, 34, 1.0, "asda", FieldUnit::Player::PositionType::MIDFIELDER),
			FieldUnit::Player(6.5, 10.0, 34, 10.0, "Epfdgfdgfgd", FieldUnit::Player::PositionType::MIDFIELDER),
			FieldUnit::Player(8.5, 6.0, 34, 4.0, "Epxcxxcxcic", FieldUnit::Player::PositionType::MIDFIELDER),
			FieldUnit::Player(8.5, 10.0, 34, 4.0, "Eqwqwwqwqwpic", FieldUnit::Player::PositionType::STRIKERS),
			FieldUnit::Player(7.5, 10.0, 34, 4.0, "Epicgfdgfdgfdg", FieldUnit::Player::PositionType::STRIKERS),
			FieldUnit::Player(8.5, 10.0, 34, 4.0, "Eczxxczxczxczpic", FieldUnit::Player::PositionType::STRIKERS),
			FieldUnit::Player(8.5, 7.0, 34, 4.0, "Epqqqqqqqqqic", FieldUnit::Player::PositionType::DEFENDER),
			FieldUnit::Player(5.5, 10.0, 34, 4.0, "Epaaaaaaaaic", FieldUnit::Player::PositionType::DEFENDER),
			FieldUnit::Player(8.5, 10.0, 34, 4.0, "Epzzzzzzzzzzic", FieldUnit::Player::PositionType::DEFENDER) });
		this->SetGoalKeeperB(FieldUnit::Player(7.5, 10.0, 34, 74.0, "Nod", FieldUnit::Player::PositionType::GOALKEEPER));

		this->faceoffs = DEFAULT_MATCHES;
	}
}

void SoccerDecoder::Start() {
	auto start = std::chrono::high_resolution_clock::now();
	std::vector<std::thread> threads;

	if (!(this->faceoffs < this->threadCount * 2)) {
		for (auto& var : Chunks(this->faceoffs))
			threads.push_back(std::thread([=]
				{ Func(var); }));
		for (auto& t : threads)
			if (t.joinable())
				t.join();
	}
	else Func(this->faceoffs);

	this->SetDuration(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start));
}

void SoccerDecoder::Func(int chunk)
{
	srand(time(NULL));

	Match::Team a("Attackers", Match::Team::FieldType::AWAY, this->GetPlayersA(), this->GetGoalKeeperA(), this->layoutA);
	Match::Team b("Defenders", Match::Team::FieldType::HOME, this->GetPlayersB(), this->GetGoalKeeperB(), this->layoutB);

	for (size_t i = 0; i < chunk; i++) {
		Match::FaceOff PlayOff(a, b);

		PlayOff.Simulate();

		if (PlayOff.GetAttackingTeam().GetScore() > PlayOff.GetDefendingTeam().GetScore()) aw++;
		else if (PlayOff.GetAttackingTeam().GetScore() < PlayOff.GetDefendingTeam().GetScore()) bw++;
		else dw++;
	}
}

std::vector<FieldUnit::Player> SoccerDecoder::FillPlayersSQLITE(DB::LiteDriver& db, int layout)
{
	int row = 0;
	std::vector<FieldUnit::Player> players;
	for (int i = 0; i < 3; ++i) {
		int temp = layout % 10;
		for (int j = 0; j < temp; ++j) {
			switch (i) {
			case 0: {
				players.push_back(FieldUnit::Player(db.getDoubleFromRow(db.getStorageRefference(), "rating", row), db.getDoubleFromRow(db.getStorageRefference(), "stamina", row), db.getDoubleFromRow(db.getStorageRefference(), "age", row), db.getDoubleFromRow(db.getStorageRefference(), "experience", row), db.getStringFromRow(db.getStorageRefference(), "name", row), FieldUnit::Player::PositionType::DEFENDER));
				break;
			}
			case 1: {
				players.push_back(FieldUnit::Player(db.getDoubleFromRow(db.getStorageRefference(), "rating", row), db.getDoubleFromRow(db.getStorageRefference(), "stamina", row), db.getDoubleFromRow(db.getStorageRefference(), "age", row), db.getDoubleFromRow(db.getStorageRefference(), "experience", row), db.getStringFromRow(db.getStorageRefference(), "name", row), FieldUnit::Player::PositionType::STRIKERS));
				break;
			}
			case 2: {
				players.push_back(FieldUnit::Player(db.getDoubleFromRow(db.getStorageRefference(), "rating", row), db.getDoubleFromRow(db.getStorageRefference(), "stamina", row), db.getDoubleFromRow(db.getStorageRefference(), "age", row), db.getDoubleFromRow(db.getStorageRefference(), "experience", row), db.getStringFromRow(db.getStorageRefference(), "name", row), FieldUnit::Player::PositionType::MIDFIELDER));
				break;
			}
			default: abort();
			}
			row++;
		}
		layout /= 10;
	}
	return players;
}

std::vector<FieldUnit::Player> SoccerDecoder::FillPlayersMYSQL(sql::ResultSet* res, int layout)
{
	std::vector<FieldUnit::Player> players;
	res->first();
	for (int i = 0; i < 3; ++i) {
		int temp = layout % 10;
		for (int j = 0; j < temp; ++j) {
			switch (i) {
			case 0: {
				players.push_back(FieldUnit::Player(static_cast<double>(res->getDouble("rating")), static_cast<double>(res->getDouble("stamina")), static_cast<double>(res->getDouble("age")), static_cast<double>(res->getDouble("experience")), res->getString("name"), FieldUnit::Player::PositionType::DEFENDER));
				break;
			}
			case 1: {
				players.push_back(FieldUnit::Player(static_cast<double>(res->getDouble("rating")), static_cast<double>(res->getDouble("stamina")), static_cast<double>(res->getDouble("age")), static_cast<double>(res->getDouble("experience")), res->getString("name"), FieldUnit::Player::PositionType::STRIKERS));
				break;
			}
			case 2: {
				players.push_back(FieldUnit::Player(static_cast<double>(res->getDouble("rating")), static_cast<double>(res->getDouble("stamina")), static_cast<double>(res->getDouble("age")), static_cast<double>(res->getDouble("experience")), res->getString("name"), FieldUnit::Player::PositionType::MIDFIELDER));
				break;
			}
			default: abort();
			}
			res->next();
		}
		layout /= 10;
	}
	return players;
}

std::vector<int> SoccerDecoder::Chunks(const int count)
{
	int total = 0;
	std::vector<int> chunk;
	for (size_t i = 0; i < this->threadCount; i++) {
		chunk.push_back(std::floor(count / (this->threadCount + 1)));
		total += std::floor(count / (this->threadCount + 1));
	}
	if (std::floor(count) - total != 0)
		chunk.push_back(count - total);
	return chunk;
}