#ifndef A54771EB_319C_4921_A7D0_A964E0E1BC03
#define A54771EB_319C_4921_A7D0_A964E0E1BC03

#include <string>
#include <vector>
#include "../headers/Player.hpp"

namespace Match
{
    class Team
    {
    public:
		enum class FieldType {
			HOME,
			AWAY
		};

        enum class Formations
        {
            _5_3_2 = 532,
            _5_4_1 = 541,
            _4_5_1 = 451,
			_4_4_2 = 442,
            _3_5_2 = 352,
			_4_3_3 = 433,
			_3_4_3 = 343
        };

        Team() = default;
        Team(std::string name, FieldType field, const std::vector<FieldUnit::Player>& players, const FieldUnit::Player& goalKeeper, Formations formation);

        size_t GetPlayerCount();
        FieldUnit::Player GetPlayerByIndex(int i);
        void CalculateTeamRating();
        void ApplyDecayAll();
        void Reset();
        FieldUnit::Player GetShootingPlayer();
        FieldUnit::Player GetRandomPlayerOfType(FieldUnit::Player::PositionType type);

        double GetMidFieldSkill() const { return midFieldSkill; }
        void SetMidFieldSkill(double val) { midFieldSkill = val; }

        double GetOffensiveSkill() const { return offensiveSkill; }
        void SetOffensiveSkill(double val) { offensiveSkill = val; }

        double GetDefensiveSkill() const { return defensiveSkill; }
        void SetDefensiveSkill(double val) { defensiveSkill = val; }

        Formations GetFormation() const { return formation; }
        void SetFormation(Formations val) { formation = val; }

        FieldUnit::Player& GetGoalKeeper() { return goalKeeper; }
        void SetGoalKeeper(FieldUnit::Player val) { goalKeeper = val; }

        std::string GetName() const { return name; }
        void SetName(std::string val) { name = val; }

        std::vector<FieldUnit::Player>& GetPlayers() { return players; }
        void SetPlayers(std::vector<FieldUnit::Player> val) { players = val; }
        
        double GetGateDefenseSkill() const { return gateDefenseSkill; }
        void SetGateDefenseSkill(double val) { gateDefenseSkill = val; }

        int GetPrefferedZone() const { return prefferedZone; }
        void SetPrefferedZone(int val) { prefferedZone = val; }

        unsigned int GetScore() const { return score; }
        void SetScore(unsigned int val) { score = val; }

        Match::Team::FieldType GetFieldType() const { return field; }
        void SetFieldType(Match::Team::FieldType val) { field = val; }
    private:
        double midFieldSkill;
        double offensiveSkill;
        double defensiveSkill;
        double gateDefenseSkill;
        unsigned int score;

        FieldType field;
        Formations formation;
        FieldUnit::Player goalKeeper;
        std::string name;
        std::vector<FieldUnit::Player> players;
        int prefferedZone;
    };
}

#endif /* A54771EB_319C_4921_A7D0_A964E0E1BC03 */