#ifndef CAA3B821_DFBB_452C_A072_4A1DE8E70127
#define CAA3B821_DFBB_452C_A072_4A1DE8E70127

#include <vector>
#include <memory>
#include "../headers/TeamMatch.hpp"
#include "../headers/ErlangDistribution.hpp"
#include "../headers/Player.hpp"
#include "../headers/Team.hpp"

namespace Match
{
    class FaceOff
    {
	public:
		enum Zone
		{
			GATE_A,
			GOAL_A,
			FIELD_A,
			MIDFIELD,
			FIELD_B,
			GOAL_B,
			GATE_B
		};

		Zone currentZone;

		FaceOff(Match::Team a, Match::Team b);
		Match::Team Compare(double a, double b);
		Match::Team Fight();
		void SetupMatch();
		void Simulate();
		void PushToHistory(Zone val);
		bool IsTeamPushing();

		void SwapTeams() {
			std::swap(this->attackingTeam, this->defendingTeam);
		}

		Match::Team& GetDefendingTeam() { return defendingTeam; }
		void SetDefendingTeam(Match::Team val) { defendingTeam = val; }

		Match::Team& GetAttackingTeam() { return attackingTeam; }
		void SetAttackingTeam(Match::Team val) { attackingTeam = val; }

		Match::FaceOff::Zone GetCurrentZone() const { return currentZone; }
		void SetCurrentZone(Match::FaceOff::Zone val) { currentZone = val; }

    private:
		Match::Team attackingTeam;
		Match::Team defendingTeam;
		std::vector<Zone> history;
    };
}

#endif /* CAA3B821_DFBB_452C_A072_4A1DE8E70127 */
