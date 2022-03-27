<?php

use FieldUnit\Player;
use Contest\Team;
use Contest\FaceOff;

// Could use autoload but nah
include_once dirname(__DIR__)."\PHP\classes\Player.php";
include_once dirname(__DIR__)."\PHP\classes\Team.php";
include_once dirname(__DIR__)."\PHP\classes\TeamMatch.php";
include_once dirname(__DIR__)."\PHP\classes\SoccerDecoder.php";

if(count($argv) <= 2){
    echo "Invalid argument count";
    exit(-1);
}

$decoder = new SoccerDecoder($argv[1], $argv[2], count($argv) > 3 ? $argv[3] : '');
$decoder->start();
$decoder->printResultJson();

?>