#ifndef C6AC8396_XXXX_46DB_A44B_CF3E89A72726
#define C6AC8396_XXXX_46DB_A44B_CF3E89A72726

#include <string>

namespace FieldUnit
{
    class Player
    {
        public:
            enum PositionType { 
                GOALKEEPER,
                DEFENDER,
                STRIKERS,
                MIDFIELDER
            };
            Player() = default;
            Player(float Skill, float FaulFactor, std::string Name, PositionType Position);
            float GetSkill()
            {
                return this->Skill;
            }

            void SetSkill(float Skill)
            {
                this->Skill = Skill;
            }

            float GetFaulFactor()
            {
                return this->FaulFactor;
            }

            void SetFaulFactor(float FaulFactor)
            {
                this->FaulFactor = FaulFactor;
            }

            std::string GetName()
            {
                return this->Name;
            }

            void SetName(std::string Name)
            {
                this->Name = Name;
            }

            PositionType GetPosition()
            {
                return this->Position;
            }

            void SetPosition(PositionType Position)
            {
                this->Position = Position;
            }
        private: 
            float Skill;
            float FaulFactor;
            static PositionType positions;
            std::string Name;
            PositionType Position;
    };
}

#endif /* C6AC8396_XXXX_46DB_A44B_CF3E89A72726 */
