<?php
   
$mail_address = "mr.datel@gmail.com";
$message_add = "";
$subject = "";

$sms_address = "+420724232639@sms.cz.o2.com";
$send_sms = False;
$sms_text = "";

$datastream_name = '';
  
$trigger = stripslashes($_POST["body"]);

// decode the JSON

$json = json_decode($trigger);

// extract the variables

$environment_id = $json->{'environment'}->{'id'};
$datastream_id = $json->{'triggering_datastream'}->{'id'};
$value = $json->{'triggering_datastream'}->{'value'}->{'value'};
$timestamp = $json->{'timestamp'};

// build the message

//outdoor temperature
// 
if ($datastream_id == "T2899BDCF02000076") {
  $datastream_name = "Venkovn� teplota";
  if ($value<=-10) {
    $sms_text = "Venku sakra mrzne";
    $message_add = $sms_text;
    $message_add .= ". A pt�ci chod� p�ky.";
    $subject = "Venku mrzne";
    $send_sms = True;
  }
  if ($value<=0) {
    $message_add = "Venku mrzne";
    $message_add .= ". Vypustil si vodu z potrub� na zahrad�? Jestli ne, tak si v�l!!!";
    $subject = "Venku mrzne";
    $send_sms = False;
  }

  if ($value>25) {
    $sms_text = "Do baz�nu";
    $message_add = $sms_text;
    $message_add .= "! To bude zase dneska pa��k.";
    $subject = "Do baz�nu";
    $send_sms = True;
  }
}

if ($datastream_id == "T28E8B84104000016") {
  $datastream_name = "Teplota lo�nice";
  if ($value<=0) {
    $message_add = "V lo�nici mrzne";
    $subject = "V lo�nici mrzne";
    $send_sms = True;
  }
}

if ($datastream_id == "WindS") {
  $datastream_name = "V�tr";
  $subject = "Zatim moc nefici ";
  $message_add = "To je jen v�t���ek.";
  if ($value>20) {
    $subject = "Fici";
    $message_add = "Fi��. Ale zat�m to nen� tak hrozn�.";
    $send_sms = False;
  }
  if ($value>40) {
    $subject = "Fici";
    $message_add = "U� fi�� jako v pohrani��.";
    $send_sms = False;
  }
  if ($value>60) {
    $subject = "Fici";
    $message_add = $subject;
    $message_add = "Fi��. A snad nebudou padat stromy.";
    $send_sms = True;
  }
  $subject .= " ";
  $subject .= strval($value/4);
  $subject .= " [m/s] = ";
  $subject .= strval(($value/4)*3.6);
  $subject .= " [km/hod]";
  $sms_text = $subject;
}

 if ($datastream_id == "Press") {
  $datastream_name = "Tlak vzduchu";
  $subject = "Tlak vzduchu ";
  $subject .= strval($value);
  $subject .= " hPa";
}

if (empty($datastream_id)) {
  $message = "Testovac� mail z pandatel.wz.cz.";
  $subject = "Test";
}
else
{
  $message = "Byl aktivov�n trigger na feedu $environment_id, datastream $datastream_id $datastream_name, s hodnotou $value v $timestamp.\n\n";
  $message .= $message_add;
  $message .= "\n\nKompletn� zpr�va: \n $trigger";
}

// use wordwrap in case lines are too long for email
//$message = wordwrap($message, 70);

// then send the email -- be sure to enter your email address below!
if ( mail($mail_address, $subject, $message) )
{
  echo("Odesl�n testovac� mail.<br><br> $message");
}
else
{
  echo("Probl�m p�i odes�l�n� mailu<br><br> $message");
}   


if ($send_sms == True) {
  if ( mail($sms_address, "", $sms_text) )
  {
  echo("email message sent. <br><br> $message");
  }
  else
  {
  echo("problem sending email message. <br><br> $message");
  }   
}

?>

