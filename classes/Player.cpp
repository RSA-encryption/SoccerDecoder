#include "../headers/Player.hpp"

namespace FieldUnit
{
    Player::Player(float Skill, float FaulFactor, std::string Name, PositionType Position)
    {
        this->Skill = Skill;
        this->FaulFactor = FaulFactor;
        this->Name = Name;
        this->Position = Position;
    }
}