#include <iostream>
#include <assert.h>
#include "../headers/Team.hpp"
#include "../headers/TeamMatch.hpp"

namespace Match
{
	Team::Team(std::string name, FieldType field, std::vector<FieldUnit::Player> players, FieldUnit::Player goalKeeper, Formations formation)
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
	void Team::CalculateTeamRating() 
	{
		double GK = 0, D = 0, M = 0, S = 0;
		bool isHomeField = this->GetFieldType() == Match::Team::HOME ? true : false;
		for (int i = 0; i < this->GetPlayerCount(); i++)
		{
			FieldUnit::Player p = this->GetPlayerByIndex(i);
			switch (p.GetPosition())
			{
			case FieldUnit::Player::GOALKEEPER:
				GK = p.GetRating();
				break;
			case FieldUnit::Player::MIDFIELDER:
			{
				M += p.GetRating() + (isHomeField) ? 0.1 : 0;
				break;
			}
			case FieldUnit::Player::STRIKERS:
			{
				S += p.GetRating() + (isHomeField) ? 0.1 : 0;
				break;
			}
			case FieldUnit::Player::DEFENDER:
				D += p.GetRating() + (isHomeField) ? 0.1 : 0;
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