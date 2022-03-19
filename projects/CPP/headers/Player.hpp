#ifndef C6AC8396_XXXX_46DB_A44B_CF3E89A72726
#define C6AC8396_XXXX_46DB_A44B_CF3E89A72726

#include <string>

namespace FieldUnit
{
    class Player
    {
    public:
        enum class PositionType {
            GOALKEEPER = 0,
            DEFENDER,
            STRIKERS,
            MIDFIELDER
        };

        Player() = default;
        Player(double rating, double stamina, int age, double experience, std::string name, PositionType position);
        void ApplyDecay(double lessenBy = 1);

        double GetRating() const { return this->rating; }
        void SetRating(double val) { this->rating = val; }

        double GetExperience() const { return experience; }
        void SetExperience(double val) { experience = val; }

		double GetStamina() const { return stamina; }
		void SetStamina(double val) { stamina = val; }

        PositionType GetPosition() const { return position; }
        void SetPosition(PositionType val) { position = val; }

        std::string GetName() const { return name; }
        void SetName(std::string val) { name = val; }

        int GetAge() const { return age; }
        void SetAge(int val) { age = val; }
    private:
        double stamina;
        double rating;
        int age;
        double experience;
        static PositionType positions;
        std::string name;
        PositionType position;
    };
}

#endif /* C6AC8396_XXXX_46DB_A44B_CF3E89A72726 */
