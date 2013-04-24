<?php
  $message = "Topír!!!!";
  $sms_address = "+420725774743@sms.cz.o2.com";

  if ( mail($sms_address, "", $message) )
  {
  echo("email message sent. <br><br> $message");
  }
  else
  {
  echo("problem sending email message. <br><br> $message");
  }   

  $message = "Topír JH!!!!";
  $sms_address = "+420724232639@sms.cz.o2.com";

  if ( mail($sms_address, "", $message) )
  {
  echo("email message sent. <br><br> $message");
  }
  else
  {
  echo("problem sending email message. <br><br> $message");
  }   
?>
