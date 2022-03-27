<?php

namespace Contest{
    include_once "Player.php";

    abstract class FieldType {
        const HOME = 0;
        const AWAY = 1;
    };

    abstract class Formations
    {
        const _5_3_2 = 532;
        const _5_4_1 = 541;
        const _4_5_1 = 451;
        const _4_4_2 = 442;
        const _3_5_2 = 352;
        const _4_3_3 = 433;
        const _3_4_3 = 343;
    };

    class Team{
        private string $name;
        private int $field;
        private array $players;
        private \FieldUnit\Player $goalKeeper;
        private int $formation;
        private int $score;
        private float $defensiveSkill;
        private float $offensiveSkill;
        private float $gateDefenseSkill;
        private float $midfieldSkill;
        private int $prefferedZone;
        
        public function __construct(string $name, int $field, array $players, \Fieldunit\Player $goalKeeper, int $formation)
        {
            assert(count($players) + 1 == 11);
            $this->setName($name);
            $this->setFieldType($field);
            $this->setPlayers($players);
            $this->setGoalKeeper($goalKeeper);
            $this->setFormation($formation);
            $this->calculateTeamRating();
            $this->setScore(0);
        }

        public function getPlayerCount()
        {
            return count($this->getPlayers());
        }
    
        public function applyDecayAll()
        {
            foreach($this->getPlayers() as &$player){
                call_user_func( array( $player, 'applyDecay' ) );
            }
            call_user_func( array( $this->getGoalKeeper(), 'applyDecay' ), 0.5 );
        }

        public function getPlayerByIndex(int $i)
        {
            assert($i < 0 || $i < $this->getPlayerCount());

            return $this->getPlayers()[$i];
        }

        public function GetShootingPlayer() {
            if (true) {
                $array = array(1,3,3,3,2,2,2,2,2,2);
            } else {
                $array = array(3,3,3,2,2,2,2,2,2,2);
            }
            return $this->getRandomPlayerOfType($array[rand() % count($array)]);
        }          

        public function getRandomPlayerOfType(int $type) {
            $arr = array();
            foreach($this->getPlayers() as &$player){
                if(call_user_func(array($player, 'getPosition')) == $type) array_push($arr, $player);
            }
            return $arr[rand() % count($arr)];
        }

        public function calculateTeamRating() 
        {
            $D = 0; $M = 0; $S = 0;
            $isHomeField = $this->getFieldType() == \Contest\FieldType::HOME ? true : false;
            for ($i = 0; $i < $this->getPlayerCount(); $i++)
            {
                $p = $this->getPlayerByIndex($i);
                switch (call_user_func(array($p, 'getPosition')))
                {
                case \FieldUnit\PositionType::MIDFIELDER:
                {
                    $M += call_user_func(array($p, 'getRating'));
                    $M += ($isHomeField) ? 0.1 : 0;
                    break;
                }
                case \FieldUnit\PositionType::STRIKER:
                {
                    $S += call_user_func(array($p, 'getRating'));
                    $M += ($isHomeField) ? 0.1 : 0;
                    break;
                }
                case \FieldUnit\PositionType::DEFENDER:
                    $D += call_user_func(array($p, 'getRating'));
                    $M += ($isHomeField) ? 0.1 : 0;
                    break;
                default:
                    break;
                }
            }
            $this->setMidFieldSkill($M + $S * 0.5 + $D * 0.2);
            $this->setOffensiveSkill($S + $M * 0.5 + $D * 0.1);
            $this->setDefensiveSkill($D + $M * 0.5 + $D * 0.1);
            $this->setGateDefenseSkill($this->getGoalKeeper()->getRating() + $D * 0.75);
        }
    
        public function reset()
        {
            $this->setMidFieldSkill(0);
            $this->setDefensiveSkill(0);
            $this->setOffensiveSkill(0);
            $this->setGateDefenseSkill(0);
            $this->calculateTeamRating(); // Just in case
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
         * Get the value of field
         */ 
        public function getFieldType()
        {
            return $this->field;
        }
    
        /**
         * Set the value of field
         *
         * @return  self
         */ 
        public function setFieldType($field)
        {
            $this->field = $field;
        }
    
        /**
         * Get the value of players
         */ 
        public function getPlayers()
        {
            return $this->players;
        }
    
        /**
         * Set the value of players
         *
         * @return  self
         */ 
        public function setPlayers($players)
        {
            $this->players = $players;
        }
    
        /**
         * Get the value of goalKeeper
         */ 
        public function getGoalKeeper()
        {
            return $this->goalKeeper;
        }
    
        /**
         * Set the value of goalKeeper
         *
         * @return  self
         */ 
        public function setGoalKeeper($goalKeeper)
        {
            $this->goalKeeper = $goalKeeper;
        }
    
        /**
         * Get the value of formation
         */ 
        public function getFormation()
        {
            return $this->formation;
        }
    
        /**
         * Set the value of formation
         *
         * @return  self
         */ 
        public function setFormation($formation)
        {
            $this->formation = $formation;
        }
    
        /**
         * Get the value of score
         */ 
        public function getScore()
        {
            return $this->score;
        }
    
        /**
         * Set the value of score
         *
         * @return  self
         */ 
        public function setScore($score)
        {
            $this->score = $score;
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
         * Get the value of offensiveSkill
         */ 
        public function getOffensiveSkill()
        {
            return $this->offensiveSkill;
        }
    
        /**
         * Set the value of offensiveSkill
         *
         * @return  self
         */ 
        public function setOffensiveSkill($offensiveSkill)
        {
            $this->offensiveSkill = $offensiveSkill;
        }
    
        /**
         * Get the value of gateDefenseSkill
         */ 
        public function getGateDefenseSkill()
        {
            return $this->gateDefenseSkill;
        }
    
        /**
         * Set the value of gateDefenseSkill
         *
         * @return  self
         */ 
        public function setGateDefenseSkill($gateDefenseSkill)
        {
            $this->gateDefenseSkill = $gateDefenseSkill;
        }
    
        /**
         * Get the value of defensiveSkill
         */ 
        public function getDefensiveSkill()
        {
            return $this->defensiveSkill;
        }
    
        /**
         * Set the value of defensiveSkill
         *
         * @return  self
         */ 
        public function setDefensiveSkill($defensiveSkill)
        {
            $this->defensiveSkill = $defensiveSkill;
        }
    
        /**
         * Get the value of midfieldSkill
         */ 
        public function getMidfieldSkill()
        {
            return $this->midfieldSkill;
        }
    
        /**
         * Set the value of midfieldSkill
         *
         * @return  self
         */ 
        public function setMidfieldSkill($midfieldSkill)
        {
            $this->midfieldSkill = $midfieldSkill;
        }
         /**
         * Get the value of prefferedZone
         */ 
        public function getPrefferedZone()
        {
                return $this->prefferedZone;
        }

        /**
         * Set the value of prefferedZone
         *
         * @return  self
         */ 
        public function setPrefferedZone($prefferedZone)
        {
                $this->prefferedZone = $prefferedZone;
        }

    }
}

?>