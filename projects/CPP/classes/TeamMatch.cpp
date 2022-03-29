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
				this->PushToHistory(this->GetCurrentZone());
				if (this->IsTeamPushing()) {
					if (result.GetName() == this->GetAttackingTeam().GetName()) {
						this->GetAttackingTeam().SetCounterOffensive(true);
					}
					else {
                        this->GetDefendingTeam().SetCounterOffensive(true);
					}
				}
				else {
                    this->GetAttackingTeam().SetCounterOffensive(false);
                    this->GetDefendingTeam().SetCounterOffensive(false);
				}
                if (this->GetAttackingTeam().GetName() == result.GetName())
                {
                       if (this->GetCurrentZone() == ((this->GetAttackingTeam().GetPrefferedZone() == GATE_A) ? GATE_B : GATE_A))
                        {
                            Match::Team& team = this->GetAttackingTeam();
                            team.SetScore(team.GetScore() + 1);
                            this->SetCurrentZone(MIDFIELD);
                            this->SwapTeams();
                        }
                        else {
                            if (static_cast<Zone>(this->GetAttackingTeam().GetPrefferedZone()) == GATE_A) {
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

    void FaceOff::PushToHistory(Zone val) {
        if (this->history.size() == 3) {
            std::vector<Zone> inefficiency;
            inefficiency.insert(inefficiency.begin(), 1, this->history.at(0));
            inefficiency.insert(inefficiency.begin(), 1, this->history.at(1)); // Insanely inefficient, it's late at night and I don't have the willpower to think rn
            inefficiency.insert(inefficiency.begin(), 1, val);
            this->history = std::move(inefficiency);
        }
        else this->history.insert(this->history.begin(), 1, val);
    }
    bool FaceOff::IsTeamPushing() {
		if (this->history.size() == 3) {
			if (static_cast<int>(this->history.at(0)) + 1 == static_cast<int>(this->history.at(1)) && static_cast<int>(this->history.at(1)) + 1 == static_cast<int>(this->history.at(2))) {
				return true;
			}
		}
		return false;
    }

    Match::Team FaceOff::Fight(){
        Match::Team x;
        switch (this->GetCurrentZone()) 
        {
            case GATE_B:
            case GATE_A:
                x = this->Compare(this->GetAttackingTeam().GetShootingPlayer().GetRating(), this->GetDefendingTeam().GetGoalKeeper().GetRating());
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