#ifndef CAA3B821_DFBB_452C_A072_4A1DE8E70127
#define CAA3B821_DFBB_452C_A072_4A1DE8E70127

#include <vector>
#include <memory>
#include "Team.hpp"
#include "ErlangDistribution.hpp"

namespace Match
{
    class FaceOff
    {
    private:
        std::vector<Match::Team> Teams;

    public:
        enum Zone
        {
            GOALGATE_A,
            FIELD_A,
            MIDFIELD,
            FIELD_B,
            GOALGATE_B
        };
        FaceOff(Match::Team a, Match::Team b);
        Zone CurrentZone;
        void SetupMatch();
        void Simulate();
        int ScoreA;
        int ScoreB;
        Match::Team AttackingTeam;
        Match::Team DefendingTeam;
        Match::Team Compare(float a, float b);
        Match::Team Fight();
        int GetScoreA()
        {
            return this->ScoreA;
        }

        void SetScoreA(int ScoreA)
        {
            this->ScoreA = ScoreA;
        }

        int GetScoreB()
        {
            return this->ScoreB;
        }

        void SetScoreB(int ScoreB)
        {
            this->ScoreB = ScoreB;
        }
    };
}

#endif /* CAA3B821_DFBB_452C_A072_4A1DE8E70127 */
