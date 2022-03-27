#include <iostream>
#include <assert.h>
#include "../headers/Team.hpp"
#include "../headers/TeamMatch.hpp"

namespace Match
{
	Team::Team(std::string name, FieldType field, const std::vector<FieldUnit::Player>& players, const FieldUnit::Player& goalKeeper, Formations formation)
	{
		assert(players.size() + 1 > 10);

		this->SetName(name);
		this->SetFieldType(field);
		this->SetPlayers(players);
		this->SetGoalKeeper(goalKeeper);
		this->SetFormation(formation);
		this->SetScore(0);
	}
	FieldUnit::Player Team::GetPlayerByIndex(int i)
	{
		assert(i < 0 || i < this->GetPlayerCount());

		return this->GetPlayers().at(i);
	}
	size_t Team::GetPlayerCount()
	{
		return this->GetPlayers().size();
	}
	FieldUnit::Player Team::GetShootingPlayer() {
		std::default_random_engine generator;
		FieldUnit::Player::PositionType r;
		if (true) {
			std::discrete_distribution<int> distribution{1,3,3,3,2,2,2,2,2,2};
			r = static_cast<FieldUnit::Player::PositionType>(distribution(generator));
		} else {
			std::discrete_distribution<int> distribution{ 3,3,3,2,2,2,2,2,2,2 };
			r = static_cast<FieldUnit::Player::PositionType>(distribution(generator));
		}
		return this->GetRandomPlayerOfType(r);
	}
	FieldUnit::Player Team::GetRandomPlayerOfType(FieldUnit::Player::PositionType type) {
		std::vector<FieldUnit::Player> vec;
		for (auto& var : this->GetPlayers())
		{
			if (var.GetPosition() == type) vec.push_back(var);
		}
		return vec.at(rand() % vec.size());
	}
	void Team::CalculateTeamRating() 
	{
		double D = 0, M = 0, S = 0;
		bool isHomeField = this->GetFieldType() == Match::Team::FieldType::HOME ? true : false;
		for (int i = 0; i < this->GetPlayerCount(); i++)
		{
			FieldUnit::Player p = this->GetPlayerByIndex(i);
			switch (p.GetPosition())
			{
			case FieldUnit::Player::PositionType::MIDFIELDER:
			{
				M += p.GetRating();
				M += (isHomeField) ? 0.1 : 0;
				break;
			}
			case FieldUnit::Player::PositionType::STRIKERS:
			{
				S += p.GetRating();
				S += (isHomeField) ? 0.1 : 0;
				break;
			}
			case FieldUnit::Player::PositionType::DEFENDER:
				D += p.GetRating() + (isHomeField) ? 0.1 : 0;
				D += (isHomeField) ? 0.1 : 0;
				break;
			default:
				break;
			}
		}
		this->SetMidFieldSkill(M + S * 0.5 + D * 0.2);
		this->SetOffensiveSkill(S + M * 0.5 + D * 0.1);
		this->SetDefensiveSkill(D + M * 0.5 + D * 0.1);
		this->SetGateDefenseSkill(this->GetGoalKeeper().GetRating() + D * 0.75);
	}

	void Team::ApplyDecayAll()
	{
		for (auto& var : players){
			var.ApplyDecay();
		}
		this->GetGoalKeeper().ApplyDecay(0.5); // Goalkeeper should always have lower decay since he doesn't move as much
	}

	void Team::Reset()
	{
		this->SetMidFieldSkill(0);
		this->SetDefensiveSkill(0);
		this->SetOffensiveSkill(0);
		this->SetGateDefenseSkill(0);
		this->CalculateTeamRating(); // Just in case
	}
}