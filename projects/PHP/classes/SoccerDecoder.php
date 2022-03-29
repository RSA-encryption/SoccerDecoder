<?php

include_once "Player.php";
include_once "Team.php";
include_once "TeamMatch.php";

set_error_handler(array('SoccerDecoder', 'errorHandlerAll'));

define('SQL_LAYOUTA', 'SELECT `layout` FROM `teams` WHERE `id` LIKE 1');
define('SQL_LAYOUTB', 'SELECT `layout` FROM `teams` WHERE `id` LIKE 2');
define('SQL_PLAYERSA', 'SELECT * FROM `players` WHERE `team_id` LIKE 1 ORDER BY `rating` ASC LIMIT 11');
define('SQL_PLAYERSB', 'SELECT * FROM `players` WHERE `team_id` LIKE 2 ORDER BY `rating` ASC LIMIT 11');
define('SQL_USE_DB', 'USE soccer');

class SoccerDecoder
{
    private object $conn;
    private int $faceoffs;
    private object $attackingTeam;
    private object $defendingTeam;
    private int $duration = 0;
    private int $aw = 0;
    private int $bw = 0;
    private int $dw = 0;

    public function __construct(int $faceoffs, int $mode, string $loc)
    {
        $layoutA = $layoutB = $playersA = $playersB = null;
        $this->faceoffs = $faceoffs;
        switch ($mode) {
            case 0:{
                    $this->conn = $this->SQLITE_Open($loc, SQLITE3_OPEN_READWRITE);
                    $layoutA = $this->SQLITE_Fetch($this->SQLITE_Execute($this->conn, SQL_LAYOUTA))['layout'];
                    $layoutB = $this->SQLITE_Fetch($this->SQLITE_Execute($this->conn, SQL_LAYOUTB))['layout'];
                    $playersA = $this->SQLITE_Fetch($this->SQLITE_Execute($this->conn, SQL_PLAYERSA));
                    $playersB = $this->SQLITE_Fetch($this->SQLITE_Execute($this->conn, SQL_PLAYERSB));
                    break;
                }
            case 1:
            case 2:{
                    $this->conn = new mysqli('localhost', 'root', 'securepassword', 'soccer');
                    $layoutA = $this->MYSQL_Fetch($this->conn, SQL_LAYOUTA)['layout'];
                    $layoutB = $this->MYSQL_Fetch($this->conn, SQL_LAYOUTB)['layout'];
                    $playersA = $this->MYSQL_Fetch($this->conn, SQL_PLAYERSA);
                    $playersB = $this->MYSQL_Fetch($this->conn, SQL_PLAYERSB);
                    break;
                }
            default: throw new Exception('SD :: Invalid launch option');
        }

        assert(count($playersA) - 1 == 10 && count($playersB) - 1 == 10);
        
        $this->setAttackingTeam(new \Contest\Team("Attackers", \Contest\FieldType::AWAY, $this->fillPlayers($playersA, $layoutA), new \FieldUnit\Player($playersA[10]['rating'], $playersA[10]['stamina'], $playersA[10]['age'], $playersA[10]['experience'], $playersA[10]['name'], $playersA[10]['position']), $layoutA));
        $this->setDefendingTeam(new \Contest\Team("Defenders", \Contest\FieldType::HOME, $this->fillPlayers($playersB, $layoutB), new \FieldUnit\Player($playersB[10]['rating'], $playersB[10]['stamina'], $playersB[10]['age'], $playersB[10]['experience'], $playersB[10]['name'], $playersB[10]['position']), $layoutB));
    }

    private function fillPlayers($result, $layout) {
        $row = 0;
        $players = [];
        for ($i = 0; $i < 3; ++$i) {
            $temp = $layout % 10;
            for ($j = 0; $j < $temp; ++$j) {
                switch ($i) {
                    case 0:
                        {
                            array_push($players, new \FieldUnit\Player($result[$row]['rating'], $result[$row]['stamina'], $result[$row]['age'], $result[$row]['experience'], $result[$row]['name'], \FieldUnit\PositionType::DEFENDER));
                            break;
                        }
                    case 1:
                        {
				            array_push($players, new \FieldUnit\Player($result[$row]['rating'], $result[$row]['stamina'], $result[$row]['age'], $result[$row]['experience'], $result[$row]['name'], \FieldUnit\PositionType::STRIKER));
                            break;
                        }
                    case 2:
                        {
                            array_push($players, new \FieldUnit\Player($result[$row]['rating'], $result[$row]['stamina'], $result[$row]['age'], $result[$row]['experience'], $result[$row]['name'], \FieldUnit\PositionType::MIDFIELDER));
                            break;
                        }
                    default:
                        throw new Exception("Fatal error");
                }
                $row++;
            }
            $layout /= 10;
        }
        return $players;
    }

    public function start(){
        srand((function(){ 
            list($usec, $sec) = explode(' ', microtime());
            return $sec + $usec * 1000000;
        })());
        $startTime = microtime(true);
        for($i = 0; $i < $this->faceoffs; ++$i){
            $faceoff = new \Contest\FaceOff(clone $this->attackingTeam, clone $this->defendingTeam);
            $faceoff->setupMatch();
            $faceoff->simulate();
            if ($faceoff->getAttackingTeam()->getScore() > $faceoff->getDefendingTeam()->getScore()) $this->aw++;
            else if ($faceoff->getAttackingTeam()->getScore() > $faceoff->getDefendingTeam()->getScore()) $this->bw++;
            else $this->dw++;
        }
        $this->setDuration(( microtime(true) - $startTime ) * 1000);
    }

    public function printResultJson(){
        echo "{\"a\":" . $this->aw . "," . "\"b\":" . $this->bw . "," . "\"draws\":" . $this->dw . "," . "\"duration\":" . $this->getDuration() . "}";
    }

    public static function errorHandlerAll(int $errno, string $message, string $file, int $line)
    {
        if($errno === E_WARNING) {
            $errno = E_ERROR;
        }
        return false;
    }

    private function SQLITE_Open(string $location, int $mode)
    {
        $handle = new SQLite3($location, $mode);
        if (!$handle) {
            throw new Exception('SQLITE :: Connection error: ' . $handle->lastErrorMsg());
        }
        return $handle;
    }

    private function SQLITE_Execute(object $conn, string $query)
    {
        $array['dbhandle'] = $conn;
        $array['query'] = $query;
        $result = $conn->query($query);
        return $result;
    }

    private function SQLITE_Fetch(object $result)
    {
        $out = array();
        while ($row = $result->fetchArray(SQLITE3_ASSOC)) {
            $out[] = $row;
        }
        if(count($out) == 1) return $out[0];
        return $out;
    }

    private function MYSQL_Open(string $endpoint, string $username, string $password, string $db)
    {
        $handle = new mysqli($endpoint, $username, $password, $db);
        if ($handle->connect_error) {
            throw new Exception('MYSQL :: Connection error: ' . $handle->connect_error);
        }
        return $handle;
    }

    private function MYSQL_Execute(object $conn, string $sql, mixed $types = null, mixed ...$params)
    {
        if (!($stmt = $conn->prepare($sql))) {
            throw new Exception('MYSQL :: Failed to prepare statement');
            exit(-1);
        }

        if($types != null){
            if (strlen($types) > 0) {
                $bindParamArray = array();
                $bindParamArray[] = &$types;
                for ($i = 0; $i < strlen($types); ++$i) {
                    $bindParamArray[] = &$params[$i];
                }
    
                if (!(call_user_func_array(array($stmt, 'bind_param'), $bindParamArray))) {
                    $stmt->close();
                    throw new Exception('MYSQL :: Failed to call_user_func_array on bind_param');
                    exit(-1);
                }
            }
        }

        if (!($stmt->execute())) {
            $stmt->close();
            throw new Exception('MYSQL :: Failed to execute query');
            exit(-1);
        }

        $stmt->close();
        return true;
    }

    private function MYSQL_Fetch(object $conn, string $sql, mixed $types = null, mixed ...$params)
    {
        $out = array();

        if (!($stmt = $conn->prepare($sql))) {
            throw new Exception('MYSQL :: Failed to prepare statement');
            exit(-1);
        }

        if($types != null){
            if (strlen($types) > 0) {
                $bindParamArray = array();
                $bindParamArray[] = &$types;
                for ($i = 0; $i < strlen($types); ++$i) {
                    $bindParamArray[] = &$params[$i];
                }
    
                if (!(call_user_func_array(array($stmt, 'bind_param'), $bindParamArray))) {
                    $stmt->close();
                    throw new Exception('MYSQL :: Failed to call_user_func_array on bind_param');
                    exit(-1);
                }
            }
        }

        if (!($stmt->execute())) {
            $stmt->close();
            throw new Exception('MYSQL :: Failed to execute query');
            exit(-1);
        }

        if (!($result = $stmt->get_result())) {
            $stmt->close();
            throw new Exception('MYSQL :: Failed to get query result');
            exit(-1);
        }
        while ($row = $result->fetch_array(MYSQLI_ASSOC)) {
            $out[] = $row;
        }

        $stmt->close();

        if(count($out) == 1) return $out[0];
        return $out;
    }

    /**
     * Get the value of duration
     */ 
    public function getDuration()
    {
        return $this->duration;
    }

    /**
     * Set the value of duration
     *
     * @return  self
     */ 
    public function setDuration($duration)
    {
        $this->duration = $duration;
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
    public function setAttackingTeam($attackingTeam)
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
    public function setDefendingTeam($defendingTeam)
    {
        $this->defendingTeam = $defendingTeam;
    }
}
?>