<?php


include('PachubeAPI.php');

$pachube = new PachubeAPI("q1PY6QqB9jvSHGKhmCQNBRdCofeSAKxpKzliaHJGWUc5UT0g");
$feed = 63310;
$user = "datel";

$datastream_name = '';
$trigger = stripslashes($_POST["body"]);
$json = json_decode($trigger);
$datastream_id = $json->{'triggering_datastream'}->{'id'};
$value = $json->{'triggering_datastream'}->{'value'}->{'value'};

if ($datastream_id == "WindS") {
  $pachube->updateDatastream("csv", $feed, "WindSpeed", strval($value / 4));
}

?>