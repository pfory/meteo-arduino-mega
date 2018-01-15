<?php

include('api.php');

//T2890A5F30100001C input
//T2828C6F3010000E5 output
//T28D376F301000020 bojler

echo("Solar JH");


$pachube = new PachubeAPI("tlujVMiq756QxqK9sRIXbqwbIhyEcjAcZl3u7u1VOvpbYtw2");
$feed = 75618;
$user = "jholcik";
$dsBojler = "T28D376F301000020";

$trigger = stripslashes($_POST["body"]);
$json = json_decode($trigger);
$datastream_id = $json->{'triggering_datastream'}->{'id'};
$value1 = $json->{'triggering_datastream'}->{'value'}->{'value'};

echo($datastream_id);

if ($datastream_id == "T2828C6F3010000E5" || $datastream_id == "T2890A5F30100001C") {
  //$datastream_id = "T2890A5F30100001C";
  //$value1 = 35;
  echo("<br>Zpracovani");

  if ($datastream_id == "T2890A5F30100001C") {
    $ds = "T2828C6F3010000E5";
  }
  if ($datastream_id == "T2828C6F3010000E5") {
    $ds = "T2890A5F30100001C";
  }

  $temp = $pachube->getDatastream('json', $feed, $ds);
  $json = json_decode($temp);

  $value2 = $json->{'current_value'};

  if ($datastream_id == "T2890A5F30100001C") {
    $Tin=$value1;
    $Tout=$value2;
  }

  if ($datastream_id == "T2828C6F3010000E5") {
    $Tin=$value2;
    $Tout=$value1;
  }

  echo("<br>Teplota vstup:");
  echo($Tin);
  echo("<br>Teplota výstup:");
  echo($Tout);

  $deltaT = $Tout - $Tin;
  echo("<br>Delta teplot:");
  echo($deltaT);

  $temp = $pachube->getDatastream('json', $feed, $dsBojler);
  $json = json_decode($temp);

  $Tbojler = $json->{'current_value'};
  echo("<br>Teplota bojler:");
  echo($Tbojler);

  if ((($Tout-$Tbojler) > 2) && $deltaT>0)
  {
    $power = $deltaT * 343;
  } 
  else
  {
    $power = 0;
  }

  echo("<br>Výkon:");
  echo($power);

//  if ($power>0) {
    $pachube->updateDatastream("csv", $feed, "Power", strval($power));
}
?>