#include "../headers/Team.hpp"
#include "../headers/TeamMatch.hpp"
#include <iostream>
#include <assert.h>
#include <string>

namespace Match
{
    Team::Team(std::string Name, float decay, float PlayersExperience, float TrainerSkill, float FieldFactor, std::vector<FieldUnit::Player> Players, FieldUnit::Player GoalKeeper, Formations Formation)
    {
        this->Name = Name;
        this->decay = decay;
        this->PlayersExperience = PlayersExperience;
        this->TrainerSkill = TrainerSkill;
        this->FieldFactor = FieldFactor;
        this->Players = Players;
        this->GoalKeeper = GoalKeeper;
        this->Formation = Formation;
    }
    FieldUnit::Player Team::GetPlayerByIndex(int i)
    {
        assert(i < 0 || i < this->GetPlayerCount());
        return this->Players.at(i);
    }
    size_t Team::GetPlayerCount()
    {
        return this->Players.size();
    }
    void Team::Reset()
    {
        this->MidFieldSkill = 0;
        this->DefensiveSkill = 0;
        this->OffensiveSkill = 0;
        this->MidFieldSkill = 0;
        this->decay = 0;
    }
}