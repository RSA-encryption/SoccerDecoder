#include <chrono>
#include "../headers/TeamMatch.hpp"
#include "../headers/ErlangDistribution.hpp"

#define HALF_TIME 2
#define ACTIONS_PER_HALF_TIME 100

namespace Match
{
    FaceOff::FaceOff(Match::Team a, Match::Team b)
    {
        a.SetPrefferedZone(static_cast<int>(GATE_B)); // Why the casts you may ask, INCLUDING HEADER WITH DEFINITON FOR ZONE TO TEAM HEADER GIVES ME AIDS
        this->SetAttackingTeam(a);
        b.SetPrefferedZone(static_cast<int>(GATE_A));
        this->SetDefendingTeam(b);
    }

    void FaceOff::SetupMatch()
    {
        this->SetCurrentZone(MIDFIELD);
        this->GetAttackingTeam().CalculateTeamRating();
        this->GetDefendingTeam().CalculateTeamRating();
    }

    void FaceOff::Simulate()
    {
        this->SetupMatch();
        for (size_t i = 0; i < HALF_TIME; i++)
        {
            for (size_t j = 0; j < ACTIONS_PER_HALF_TIME; j++)
            {
                Match::Team result = this->Fight();
                if (this->GetAttackingTeam().GetName() == result.GetName())
                {
                       if (this->GetCurrentZone() == ((this->GetAttackingTeam().GetPrefferedZone() == GOAL_A) ? GOAL_B : GOAL_A))
                        {
                            Match::Team& team = this->GetAttackingTeam();
                            team.SetScore(team.GetScore() + 1);
                            this->SetCurrentZone(MIDFIELD);
                            this->SwapTeams();
                        }
                        else {
                            if (static_cast<Zone>(this->GetAttackingTeam().GetPrefferedZone()) == GOAL_A) {
                                this->SetCurrentZone(static_cast<Zone>(static_cast<int>(this->GetCurrentZone()) + 1));
                            }
                            else {
                                this->SetCurrentZone(static_cast<Zone>(static_cast<int>(this->GetCurrentZone()) - 1));
                            }
                        }
                }
                else
                {
                    this->SetCurrentZone(MIDFIELD);
                    this->SwapTeams();
                }
            }
        }
    }

    Match::Team FaceOff::Fight(){
        Match::Team x;
        switch (this->GetCurrentZone()) 
        {
            case GATE_B:
            case GATE_A:
                x = this->Compare(this->GetAttackingTeam().GetOffensiveSkill(), this->GetDefendingTeam().GetGoalKeeper().GetRating());
                break;
            case MIDFIELD:
                x = this->Compare(this->GetAttackingTeam().GetMidFieldSkill(), this->GetDefendingTeam().GetMidFieldSkill());
                break;
            case FIELD_A:
            case FIELD_B:
				x = this->Compare(this->GetAttackingTeam().GetMidFieldSkill(), this->GetDefendingTeam().GetDefensiveSkill());
				break;
			case GOAL_A:
			case GOAL_B:
				x = this->Compare(this->GetAttackingTeam().GetOffensiveSkill(), this->GetDefendingTeam().GetGateDefenseSkill());
				break;
            default:
                abort(); //Avoid -W return-type
                break;
        }
        this->GetAttackingTeam().ApplyDecayAll();
        this->GetAttackingTeam().CalculateTeamRating();
        this->GetDefendingTeam().ApplyDecayAll();
        this->GetDefendingTeam().CalculateTeamRating();
        return x;
    }

    Match::Team FaceOff::Compare(double a, double b)
    {
        if (Generator::ErlangDistribution<double>::Distribute(a) > Generator::ErlangDistribution<double>::Distribute(b))
            return this->GetAttackingTeam();
        return this->GetDefendingTeam();
    }
}