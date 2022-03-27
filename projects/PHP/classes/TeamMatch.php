<?php

namespace Contest {
    include_once "Team.php";
    include_once "Player.php";

    define("HALF_TIME", 2);
    define("ACTIONS_PER_HALF_TIME", 100);

    abstract class Zone
    {
        const GATE_A = 0;
        const GOAL_A = 1;
        const FIELD_A = 2;
        const MIDFIELD = 3;
        const FIELD_B = 4;
        const GOAL_B = 5;
        const GATE_B = 6;
    };

    class FaceOff
    {
        private \Contest\Team $attackingTeam;
        private \Contest\Team $defendingTeam;
        private int $currentZone;

        public function __construct(\Contest\Team $a, \Contest\Team $b)
        {
            $a->setPrefferedZone(Zone::GATE_B);
            $this->setAttackingTeam($a);
            $b->setPrefferedZone(Zone::GATE_A);
            $this->setDefendingTeam($b);
        }

        public function setupMatch()
        {
            $this->setCurrentZone(Zone::MIDFIELD);
            $this->getAttackingTeam()->calculateTeamRating();
            $this->getDefendingTeam()->calculateTeamRating();
        }

        private function swapTeams()
        {
            $tmp = $this->getAttackingTeam();
            $this->setAttackingTeam($this->getDefendingTeam());
            $this->setAttackingTeam($tmp);
        }

        public function simulate()
        {
            $this->setupMatch();
            for ($i = 0; $i < HALF_TIME; $i++) {
                for ($j = 0; $j < ACTIONS_PER_HALF_TIME; $j++) {
                    $result = $this->fight();
                    if ($this->getAttackingTeam()->getname() == $result->getName()) {
                        if ($this->getCurrentZone() == (($this->getAttackingTeam()->getPrefferedZone() == Zone::GATE_A) ? Zone::GATE_B : Zone::GATE_A)) {
                            $team = $this->getAttackingTeam();
                            $team->setScore($team->getScore() + 1);
                            $this->setCurrentZone(Zone::MIDFIELD);
                            $this->swapTeams();
                        } else {
                            if ($this->getAttackingTeam()->getPrefferedZone() == Zone::GATE_A) {
                                $this->setCurrentZone($this->getCurrentZone() + 1);
                            } else {
                                $this->setCurrentZone($this->getCurrentZone() - 1);
                            }
                        }
                    } else {
                        $this->setCurrentZone(Zone::MIDFIELD);
                        $this->swapTeams();
                    }
                }
            }
        }

        private function fight()
        {
            $x = null;
            switch ($this->getCurrentZone()) {
                case Zone::GATE_B:
                case Zone::GATE_A:
                    $x = $this->compare($this->getAttackingTeam()->getShootingPlayer()->getRating(), $this->getAttackingTeam()->getGoalKeeper()->getRating());
                    break;
                case Zone::MIDFIELD:
                    $x = $this->compare($this->getAttackingTeam()->getMidfieldSkill(), $this->getDefendingTeam()->getMidFieldSkill());
                    break;
                case Zone::FIELD_A:
                case Zone::FIELD_B:
                    $x = $this->compare($this->getAttackingTeam()->getMidFieldSkill(), $this->getDefendingTeam()->getDefensiveSkill());
                    break;
                case Zone::GOAL_A:
                case Zone::GOAL_B:
                    $x = $this->compare($this->getAttackingTeam()->getOffensiveSkill(), $this->getDefendingTeam()->getGateDefenseSkill());
                    break;
                default:
                    die("Fatal error during simulation");
                    break;
            }
            $this->getAttackingTeam()->applyDecayAll();
            $this->getAttackingTeam()->calculateTeamRating();
            $this->getDefendingTeam()->applyDecayAll();
            $this->getDefendingTeam()->calculateTeamRating();
            return $x;
        }

        private function compare(float $a, float $b)
        {
            if ($this->erlangDistribution($a) > $this->erlangDistribution($b)) {
                return $this->getAttackingTeam();
            }

            return $this->getDefendingTeam();
        }

        private function erlangDistribution(float $alpha, float $beta = 1.0)
        {
            $d = $alpha - 1.0 / 3.0;
            $c = 1.0 / sqrt(9.0 * $d);
            $z = 0.0;
            while (true) {
                $x = 0.0;
                $v = 0.0;
                do {
                    $x = $this->randomUniform(0, 1);
                    $tmp = 1.0 + $c * $x;
                    if(is_nan($tmp)) break;
                    $v = $tmp;
                } while ($v <= 0);
                $v = pow($v, 3.0);
                $u = $this->RandomNormal(0.0, 1.0);
                if ($u < 1.0 - 0.0331 * pow($x, 4.0)) {
                    $z = $d * $v;
                    break;
                }
                if (log($u) < 0.5 * pow($x, 2.0) + $d * (1 - $v + log($v))) {
                    $z = $d * $v;
                    break;
                }
            }
            return abs($z / $beta);
        }

        private function randomUniform($a, $b)
        {
            $u = rand() / getrandmax();
            return $a + $u * ($b - $a);
        }

        private function RandomNormal($mu, $sigma)
        {
            $u1 = $this->randomUniform(0, 1);
            $u2 = $this->randomUniform(0, 1);
            $z0 = sqrt(-2 * log($u1)) * sin(2 * M_PI * $u2);
            return $mu + $z0 * $sigma;
        }

        /**
         * Get the value of attackingTeam
         */
        public function getAttackingTeam()
        {
            return $this->attackingTeam;
        }

        /**
         * Set the value of attackingTeam
         *
         * @return  self
         */
        private function setAttackingTeam($attackingTeam)
        {
            $this->attackingTeam = $attackingTeam;
        }

        /**
         * Get the value of defendingTeam
         */
        public function getDefendingTeam()
        {
            return $this->defendingTeam;
        }

        /**
         * Set the value of defendingTeam
         *
         * @return  self
         */
        private function setDefendingTeam($defendingTeam)
        {
            $this->defendingTeam = $defendingTeam;
        }

        /**
         * Get the value of currentZone
         */
        private function getCurrentZone()
        {
            return $this->currentZone;
        }

        /**
         * Set the value of currentZone
         *
         * @return  self
         */
        private function setCurrentZone($currentZone)
        {
            $this->currentZone = $currentZone;
        }
    }
}
