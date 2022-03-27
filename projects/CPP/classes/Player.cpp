#include "../headers/Player.hpp"
#include <iostream>
namespace FieldUnit
{
    Player::Player(double rating, double stamina, int age, double experience, std::string name, PositionType position)
    {
        if (stamina < 0 && stamina > 20 || experience < 0 && experience > 100) {
            throw new std::exception("Stamina or experience is not within allowed ranges");
        }
        this->SetRating(rating);
        this->SetStamina(stamina);
        this->SetAge(age);
        this->SetExperience(experience);
        this->SetName(name);
        this->SetPosition(position);
    }

	void Player::ApplyDecay(double lessenBy)
	{
        this->SetRating((this->GetRating() / 100) * (100 - std::abs((((this->GetAge() / 9.5) + (this->GetStamina() / 10) + (2 * this->GetExperience() / 100)) / 10)) * lessenBy));
	}
}