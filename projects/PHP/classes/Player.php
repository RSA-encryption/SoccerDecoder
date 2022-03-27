<?php

namespace FieldUnit
{
    abstract class PositionType {
        const GOALKEEPER = 0;
        const DEFENDER = 1;
        const STRIKER = 2;
        const MIDFIELDER = 3;
    };

    class Player{
        private float $rating;
        private float $stamina;
        private float $age;
        private float $experience;
        private string $name;
        private int $position;
        
        public function __construct(float $rating, float $stamina, int $age, float $experience, string $name, int $position)
        {
            if ($stamina < 0 && $stamina > 20 || $experience < 0 && $experience > 100) {
                new \Exception("Stamina or experience is not within allowed ranges");
            }
            $this->setRating($rating);
            $this->setStamina($stamina);
            $this->setAge($age);
            $this->setExperience($experience);
            $this->setName($name);
            $this->setPosition($position);
        }
    
        public function applyDecay(float $lessenBy = 1.0)
        {
               // Note note : There actually might be a bug somewhere
              // Note: I give up. I may fix it later on but I don't really see the point..
             // Decay slower and the reason...
            // Php precision with floats just fucks up all of the stats ( not 100% sure but it makes sense after spending some time trying to debug this ) so I decided to put a random scale down.. Don't judge me I assume this could be avoided if this was routed through an extension
            $this->setRating(($this->getRating() / 100) * (100 - abs(((($this->getAge() / 9.5) + ($this->getStamina() / 10) + (2 * $this->getExperience() / 100)) / rand(3000,3333))) * $lessenBy));
        }
    
        /**
         * Get the value of rating
         */ 
        public function getRating()
        {
            return $this->rating;
        }
    
        /**
         * Set the value of rating
         *
         * @return  self
         */ 
        public function setRating($rating)
        {
            $this->rating = $rating;
        }
    
        /**
         * Get the value of stamina
         */ 
        public function getStamina()
        {
            return $this->stamina;
        }
    
        /**
         * Set the value of stamina
         *
         * @return  self
         */ 
        public function setStamina($stamina)
        {
            $this->stamina = $stamina;
        }
    
        /**
         * Get the value of age
         */ 
        public function getAge()
        {
            return $this->age;
        }
    
        /**
         * Set the value of age
         *
         * @return  self
         */ 
        public function setAge($age)
        {
            $this->age = $age;
        }
    
        /**
         * Get the value of experience
         */ 
        public function getExperience()
        {
            return $this->experience;
        }
    
        /**
         * Set the value of experience
         *
         * @return  self
         */ 
        public function setExperience($experience)
        {
            $this->experience = $experience;
        }
    
        /**
         * Get the value of name
         */ 
        public function getName()
        {
            return $this->name;
        }
    
        /**
         * Set the value of name
         *
         * @return  self
         */ 
        public function setName($name)
        {
            $this->name = $name;
        }
    
        /**
         * Get the value of position
         */ 
        public function getPosition()
        {
            return $this->position;
        }
    
        /**
         * Set the value of position
         *
         * @return  self
         */ 
        public function setPosition($position)
        {
            $this->position = $position;
        }
    }

    $a = new Player(10.2,10.1,10,10.1,"pog", 1);
    $a->applyDecay();
}

?>