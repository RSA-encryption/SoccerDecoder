#include "../headers/SoccerDecoder.hpp"

SoccerDecoder::SoccerDecoder(int argc, char* argv[]) {
	auto t = std::chrono::high_resolution_clock::now();
	unsigned int seed = std::chrono::system_clock::now().time_since_epoch().count();
	this->threadCount = std::thread::hardware_concurrency();
	this->duration = std::chrono::duration_cast<std::chrono::seconds>(t - t); // 0
	if (argc >= 4) {
		std::string url(argv[3]);
		this->faceoffs = std::stoi(argv[1]);
		switch (static_cast<enum class DatabaseType>(std::stoi(argv[2]))) {
			case DatabaseType::SQLITE : {
				DB::LiteDriver db(url);
				break;
			}
			case DatabaseType::MYSQL: {
				break;
			}
			case DatabaseType::MONGODB: {
				break;
			}
		}
	} else { // Default sample data
		this->APlayers = {
			FieldUnit::Player(8.5, 10.0, 34, 1.0, "Kanker", FieldUnit::Player::MIDFIELDER),
			FieldUnit::Player(8.5, 10.0, 34, 1.0, "asda", FieldUnit::Player::MIDFIELDER),
			FieldUnit::Player(8.5, 10.0, 34, 1.0, "Epfdgfdgfgd", FieldUnit::Player::MIDFIELDER),
			FieldUnit::Player(8.5, 10.0, 34, 4.0, "Epxcxxcxcic", FieldUnit::Player::MIDFIELDER),
			FieldUnit::Player(8.5, 10.0, 34, 4.0, "Eqwqwwqwqwpic", FieldUnit::Player::STRIKERS),
			FieldUnit::Player(8.5, 10.0, 34, 4.0, "Epicgfdgfdgfdg", FieldUnit::Player::STRIKERS),
			FieldUnit::Player(8.5, 10.0, 34, 4.0, "Eczxxczxczxczpic", FieldUnit::Player::STRIKERS),
			FieldUnit::Player(8.5, 10.0, 34, 4.0, "Epqqqqqqqqqic", FieldUnit::Player::DEFENDER),
			FieldUnit::Player(8.5, 10.0, 34, 4.0, "Epaaaaaaaaic", FieldUnit::Player::DEFENDER),
			FieldUnit::Player(8.5, 10.0, 34, 4.0, "Epzzzzzzzzzzic", FieldUnit::Player::DEFENDER) };
		this->BPlayers = {
			FieldUnit::Player(8.5, 10.0, 34, 1.0, "Kanker", FieldUnit::Player::MIDFIELDER),
			FieldUnit::Player(8.5, 10.0, 34, 1.0, "asda", FieldUnit::Player::MIDFIELDER),
			FieldUnit::Player(8.5, 10.0, 34, 1.0, "Epfdgfdgfgd", FieldUnit::Player::MIDFIELDER),
			FieldUnit::Player(8.5, 10.0, 34, 4.0, "Epxcxxcxcic", FieldUnit::Player::MIDFIELDER),
			FieldUnit::Player(8.5, 10.0, 34, 4.0, "Eqwqwwqwqwpic", FieldUnit::Player::STRIKERS),
			FieldUnit::Player(8.5, 10.0, 34, 4.0, "Epicgfdgfdgfdg", FieldUnit::Player::STRIKERS),
			FieldUnit::Player(8.5, 10.0, 34, 4.0, "Eczxxczxczxczpic", FieldUnit::Player::STRIKERS),
			FieldUnit::Player(8.5, 10.0, 34, 4.0, "Epqqqqqqqqqic", FieldUnit::Player::DEFENDER),
			FieldUnit::Player(8.5, 10.0, 34, 4.0, "Epaaaaaaaaic", FieldUnit::Player::DEFENDER),
			FieldUnit::Player(8.5, 10.0, 34, 4.0, "Epzzzzzzzzzzic", FieldUnit::Player::DEFENDER) };
		this->faceoffs = DEFAULT_MATCHES;
	}
}

void SoccerDecoder::Start() {
	auto start = std::chrono::high_resolution_clock::now();
	std::vector<std::thread> threads;

	if (!(this->faceoffs < this->threadCount * 2)) {
		for (auto& var : Chunks(this->faceoffs))
			threads.push_back(std::thread([=]
				{ Func(this->GetAPlayers(), this->GetBPlayers(), var); }));
		for (auto& t : threads)
			if (t.joinable())
				t.join();
	}
	else Func(this->GetAPlayers(), this->GetBPlayers(), this->faceoffs);

	this->SetDuration(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start));
}

void SoccerDecoder::Func(const std::vector<FieldUnit::Player>& APlayers, const std::vector<FieldUnit::Player>& BPlayers, int chunk)
{
	srand(time(NULL));

	Match::Team a("Attackers", Match::Team::AWAY, APlayers, FieldUnit::Player(8.5, 10.0, 34, 74.0, "Pot", FieldUnit::Player::GOALKEEPER), Match::Team::Formations::_4_4_2);
	Match::Team b("Defenders", Match::Team::AWAY, BPlayers, FieldUnit::Player(8.5, 10.0, 34, 74.0, "Nod", FieldUnit::Player::GOALKEEPER), Match::Team::Formations::_4_4_2);

	for (size_t i = 0; i < chunk; i++) {
		Match::FaceOff PlayOff(a, b);

		PlayOff.Simulate();

		if (PlayOff.GetAttackingTeam().GetScore() > PlayOff.GetDefendingTeam().GetScore()) aw++;
		else if (PlayOff.GetAttackingTeam().GetScore() < PlayOff.GetDefendingTeam().GetScore()) bw++;
		else dw++;
	}
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