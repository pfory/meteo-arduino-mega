<?php

include('PachubeAPI.php');

$pachube = new PachubeAPI("q1PY6QqB9jvSHGKhmCQNBRdCofeSAKxpKzliaHJGWUc5UT0g");
$feed = 63310;
$user = "datel";


$temp = $pachube->getDatastream('json', $feed, 'WindS');
$json = json_decode($temp);
$speed = $json->{'current_value'};

//echo("Vitr:");
//echo($speed);

$pachube->updateDatastream("csv", $feed, "WindSpeed", strval($speed / 4));
?>