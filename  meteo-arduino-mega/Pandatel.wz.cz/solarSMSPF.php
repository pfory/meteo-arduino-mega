<?php
  include('PachubeAPI.php');

  $pachube = new PachubeAPI("azCLxsU4vKepKymGFFWVnXCvTQ6Ilze3euIsNrRKRRXuSPO8");
  $feed = 538561447;
  $user = "datel";
  
  $temp = $pachube->getDatastream('json', $feed, 'Status');
  $json = json_decode($temp);
  $status = $json->{'current_value'};

  if ($status==1)
    $message = "Top�r!!!!";
  else
    $message = "Netop�r!!!!";

  $sms_address = "+420724232639@sms.cz.o2.com";
  $mail_address = "mr.datel@gmail.com";

  /*if ( mail($sms_address, "", $message) )
  {
  echo("email message sent. <br><br> $message");
  }
  else
  {
  echo("problem sending email message. <br><br> $message");
  } 
  */
  if ( mail($mail_address, "", $message) )
  {
  echo("email message sent. <br><br> $message");
  }
  else
  {
  echo("problem sending email message. <br><br> $message");
  }   
?>
<!--WZ-REKLAMA-1.0IZ--><div align="center"><table width="496" border="0"
cellspacing="0" cellpadding="0"><tr><td><a href="http://www.webzdarma.cz/"><img
src="http://i.wz.cz/banner/nudle03.gif" width="28" height="60" 
style="margin: 0; padding: 0; border-width: 0" alt="WebZdarma.cz" /></a></td><td>
<script type='text/javascript'><!--//<![CDATA[
   var m3_u = (location.protocol=='https:'?'https':'http')+'://ad.wz.cz/openx/www/delivery/ajs.php';
   var m3_r = Math.floor(Math.random()*99999999999);
   if (!document.MAX_used) document.MAX_used = ',';
   document.write ("<scr"+"ipt type='text/javascript' src='"+m3_u);
   document.write ("?zoneid=27");
   document.write ('&amp;cb=' + m3_r);
   if (document.MAX_used != ',') document.write ("&amp;exclude=" + document.MAX_used);
   document.write (document.charset ? '&amp;charset='+document.charset : (document.characterSet ? '&amp;charset='+document.characterSet : ''));
   document.write ("&amp;loc=" + escape(window.location));
   if (document.referrer) document.write ("&amp;referer=" + escape(document.referrer));
   if (document.context) document.write ("&context=" + escape(document.context));
   if (document.mmm_fo) document.write ("&amp;mmm_fo=1");
   document.write ("'><\/scr"+"ipt>");
//]]>--></script><noscript><div><a href='http://ad.wz.cz/openx/www/delivery/ck.php?n=a5977468&amp;cb=123'><img src='http://ad.wz.cz/openx/www/delivery/avw.php?zoneid=27&amp;cb=123&amp;n=a5977468' style='margin: 0; padding: 0; border-width: 0' alt='' /></a></div></noscript>
</td></tr></table></div>
<!--WZ-REKLAMA-1.0IK-->