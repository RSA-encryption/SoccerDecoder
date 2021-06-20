#include "../headers/TeamMatch.hpp"
#include "../headers/ErlangDistribution.hpp"
#include <chrono>
#define HALF_TIME 2
#define ACTIONS_PER_HALF_TIME 100

namespace Match
{
    FaceOff::FaceOff(Match::Team a, Match::Team b)
    {
        this->Teams.push_back(a);
        this->Teams.push_back(b);
    }
    void FaceOff::SetupMatch()
    {
        this->ScoreA = 0;
        this->ScoreB = 0;
        this->CurrentZone = MIDFIELD;
        for (auto &var : this->Teams)
        {
            float AdditionalBonus = var.GetPlayersExperience() + var.GetTrainerSkill() + var.GetFieldFactor();
            var.SetGoalKeeperSkill(var.GetGoalKeeper().GetSkill());
            for (size_t i = 0; i < var.GetPlayerCount(); i++)
            {
                FieldUnit::Player p = var.GetPlayerByIndex(i);
                switch (p.GetPosition())
                {
                    case FieldUnit::Player::GOALKEEPER:
                        break;
                    case FieldUnit::Player::MIDFIELDER:
                    {
                        var.SetMidFieldSkill(var.GetMidFieldSkill() + p.GetSkill());
                        var.SetDefensiveSkill(var.GetDefensiveSkill() + (p.GetSkill() * 1 / 2));
                        var.SetOffensiveSkill(var.GetOffensiveSkill() + (p.GetSkill() * 1 / 2));
                        break;
                    }
                    case FieldUnit::Player::STRIKERS:
                    {
                        var.SetOffensiveSkill(var.GetOffensiveSkill() + p.GetSkill());
                        break;
                    }
                    case FieldUnit::Player::DEFENDER:
                        var.SetDefensiveSkill(var.GetDefensiveSkill() + p.GetSkill());
                        break;
                    default:
                        break;
                }
            }
            var.SetOffensiveSkill(var.GetOffensiveSkill() + AdditionalBonus);
            var.SetDefensiveSkill(var.GetDefensiveSkill() + AdditionalBonus);
            var.SetMidFieldSkill(var.GetMidFieldSkill() + AdditionalBonus);
            (&this->Teams.back() != &var) ? this->AttackingTeam = var : this->DefendingTeam = var;
        }
    }

    void FaceOff::Simulate()
    {
        this->SetupMatch();
        for (size_t j = 0; j < HALF_TIME; j++)
        {
            for (size_t i = 0; i < ACTIONS_PER_HALF_TIME; i++)
            {
                Match::Team result = this->Fight();
                if (this->AttackingTeam.GetName() == result.GetName())
                {
                    if (this->AttackingTeam.GetName() == this->Teams.front().GetName())
                    {
                        if (this->CurrentZone == GOALGATE_B)
                        {
                            this->ScoreA += 1;
                            this->CurrentZone = MIDFIELD;
                            std::swap(this->AttackingTeam, this->DefendingTeam);
                        }
                        else
                            this->CurrentZone = static_cast<Zone>(static_cast<int>(this->CurrentZone) + 1);
                    }
                    else
                    {
                        if (this->CurrentZone == GOALGATE_A)
                        {
                            this->ScoreB += 1;
                            this->CurrentZone = MIDFIELD;
                            std::swap(this->AttackingTeam, this->DefendingTeam);
                        }
                        else
                            this->CurrentZone = static_cast<Zone>(static_cast<int>(this->CurrentZone) - 1);
                    }
                }
                else
                {
                    this->CurrentZone = MIDFIELD;
                    std::swap(this->AttackingTeam, this->DefendingTeam);
                }
            }
        }
    }

    Match::Team FaceOff::Fight()
    {
        Match::Team x;
        switch (this->CurrentZone)
        {
            case GOALGATE_A:
            case GOALGATE_B:
                x = this->Compare(this->AttackingTeam.GetOffensiveSkill(), this->DefendingTeam.GetGoalKeeperSkill());
                this->AttackingTeam.SetOffensiveSkill(this->AttackingTeam.GetOffensiveSkill() - std::abs((this->AttackingTeam.GetOffensiveSkill() / 100 * this->AttackingTeam.GetDecay())));
                this->DefendingTeam.SetGoalKeeperSkill(this->DefendingTeam.GetGoalKeeperSkill() - std::abs((this->DefendingTeam.GetGoalKeeperSkill() / 100 * this->DefendingTeam.GetDecay())));
                return x;
                break;
            case MIDFIELD:
                x = this->Compare(this->AttackingTeam.GetMidFieldSkill(), this->DefendingTeam.GetMidFieldSkill());
                this->AttackingTeam.SetMidFieldSkill(this->AttackingTeam.GetMidFieldSkill() - std::abs((this->AttackingTeam.GetMidFieldSkill() / 100 * this->AttackingTeam.GetDecay())));
                this->DefendingTeam.SetMidFieldSkill(this->DefendingTeam.GetMidFieldSkill() - std::abs((this->DefendingTeam.GetMidFieldSkill() / 100 * this->DefendingTeam.GetDecay())));
                return x;
                break;
            case FIELD_A:
            case FIELD_B:
                x = this->Compare(this->AttackingTeam.GetOffensiveSkill(), this->DefendingTeam.GetDefensiveSkill());
                this->AttackingTeam.SetOffensiveSkill(this->AttackingTeam.GetOffensiveSkill() - std::abs((this->AttackingTeam.GetOffensiveSkill() / 100 * this->AttackingTeam.GetDecay())));
                this->DefendingTeam.SetDefensiveSkill(this->DefendingTeam.GetDefensiveSkill() - std::abs((this->DefendingTeam.GetDefensiveSkill() / 100 * this->DefendingTeam.GetDecay())));
                return x;
                break;
            default:
                abort(); //Avoid -Wreturn-type warning
                break;
        }
    }

    Match::Team FaceOff::Compare(float a, float b)
    {
        if (Generator::ErlangDistribution<double>::Distribute(a) > Generator::ErlangDistribution<double>::Distribute(b))
            return this->AttackingTeam;
        return this->DefendingTeam;
    }
}