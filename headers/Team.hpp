#ifndef A54771EB_319C_4921_A7D0_A964E0E1BC03
#define A54771EB_319C_4921_A7D0_A964E0E1BC03

#include <string>
#include <vector>
#include "Player.hpp"

namespace Match
{
    class Team
    {
    public:
        enum Formations
        {
            _4_4_2,
            _4_3_3,
            _3_5_2,
            _4_3_2_1
        };
        Team() = default;
        Team(std::string Name, float decay, float PlayersExperience, float TrainerSkill, float FieldFactor, std::vector<FieldUnit::Player> Players, FieldUnit::Player GoalKeeper, Formations Formation);
        size_t GetPlayerCount();
        FieldUnit::Player GetPlayerByIndex(int i);
        void Reset();

        std::string GetName()
        {
            return this->Name;
        }

        void SetName(std::string Name)
        {
            this->Name = Name;
        }

        float GetMidFieldSkill()
        {
            return this->MidFieldSkill;
        }

        void SetMidFieldSkill(float MidFieldSkill)
        {
            this->MidFieldSkill = MidFieldSkill;
        }
        float GetOffensiveSkill()
        {
            return this->OffensiveSkill;
        }

        void SetOffensiveSkill(float OffensiveSkill)
        {
            this->OffensiveSkill = OffensiveSkill;
        }

        float GetDefensiveSkill()
        {
            return this->DefensiveSkill;
        }

        void SetDefensiveSkill(float DefensiveSkill)
        {
            this->DefensiveSkill = DefensiveSkill;
        }

        FieldUnit::Player GetGoalKeeper()
        {
            return this->GoalKeeper;
        }

        void SetGoalKeeper(FieldUnit::Player GoalKeeper)
        {
            this->GoalKeeper = GoalKeeper;
        }

        Formations GetFormation()
        {
            return this->Formation;
        }

        void SetFormation(Formations Formation)
        {
            this->Formation = Formation;
        }
        std::vector<FieldUnit::Player> GetPlayers()
        {
            return this->Players;
        }

        void SetPlayers(std::vector<FieldUnit::Player> Players)
        {
            this->Players = Players;
        }

        float GetDecay()
        {
            return this->decay;
        }

        void SetDecay(float decay)
        {
            this->decay = decay;
        }
        float GetGoalKeeperSkill()
        {
            return this->GoalKeeperSkill;
        }

        void SetGoalKeeperSkill(float GoalKeeperSkill)
        {
            this->GoalKeeperSkill = GoalKeeperSkill;
        }
        float GetTrainerSkill()
        {
            return this->TrainerSkill;
        }

        void SetTrainerSkill(float TrainerSkill)
        {
            this->TrainerSkill = TrainerSkill;
        }

        float GetPlayersExperience()
        {
            return this->PlayersExperience;
        }

        void SetPlayersExperience(float PlayersExperience)
        {
            this->PlayersExperience = PlayersExperience;
        }
        float GetFieldFactor()
        {
            return this->FieldFactor;
        }

        void SetFieldFactor(float FieldFactor)
        {
            this->FieldFactor = FieldFactor;
        }

    private:
        std::string Name;
        float MidFieldSkill;
        float OffensiveSkill;
        float DefensiveSkill;
        float GoalKeeperSkill;
        float TrainerSkill;
        float PlayersExperience;
        float FieldFactor;
        float decay;
        std::vector<FieldUnit::Player> Players;
        FieldUnit::Player GoalKeeper;
        Formations Formation;
    };
}

#endif /* A54771EB_319C_4921_A7D0_A964E0E1BC03 */