<?php
include('PachubeAPI.php');

$pachube = new PachubeAPI("q1PY6QqB9jvSHGKhmCQNBRdCofeSAKxpKzliaHJGWUc5UT0g");
$feed = 63310;
$user = "datel";

// mysql
$c = mysql_connect("mysql.webzdarma.cz", "pandatel23", "c5e56n");
mysql_select_db("pandatel23");
mysql_select_db("pandatel23");

$datumCas = new DateTime('NOW');

$temp = $pachube->getDatastream('json', $feed, 'WindS');
$json = json_decode($temp);
$windSpeed = $json->{'current_value'};
$result = mysql_query("INSERT into data (ID, DATETIME, VALUE) select 'WindS','".$datumCas->format('Y-m-d H:i:s')."',".$windSpeed);

$temp = $pachube->getDatastream('json', $feed, 'koefWind');
$json = json_decode($temp);
$koefWind = $json->{'current_value'};
$pachube->updateDatastream("csv", $feed, "WindSpeed", strval(sprintf("%.1f",$windSpeed / $koefWind)));

$temp = $pachube->getDatastream('json', $feed, 'WindSM');
$json = json_decode($temp);
$windSpeedMax = $json->{'current_value'};
$result = mysql_query("INSERT into data (ID, DATETIME, VALUE) select 'WindSM','".$datumCas->format('Y-m-d H:i:s')."',".$windSpeedMax);
$pachube->updateDatastream("csv", $feed, "WindSpeedMax", strval(sprintf("%.1f",$windSpeedMax / $koefWind)));

$temp = $pachube->getDatastream('json', $feed, 'Rain');
$json = json_decode($temp);
$rain = $json->{'current_value'};
if ($rain!="0") {
  $result = mysql_query("INSERT into data (ID, DATETIME, VALUE) select 'Rain','".$datumCas->format('Y-m-d H:i:s')."',".$rain);
}

$temp = $pachube->getDatastream('json', $feed, 'Humidity');
$json = json_decode($temp);
$humidity = $json->{'current_value'};
$result = mysql_query("INSERT into data (ID, DATETIME, VALUE) select 'Humidity','".$datumCas->format('Y-m-d H:i:s')."',".$humidity);

$temp = $pachube->getDatastream('json', $feed, 'Press');
$json = json_decode($temp);
$press = $json->{'current_value'};
$result = mysql_query("INSERT into data (ID, DATETIME, VALUE) select 'Press','".$datumCas->format('Y-m-d H:i:s')."',".$press);

$temp = $pachube->getDatastream('json', $feed, 'T2899BDCF02000076');
$json = json_decode($temp);
$temperature = $json->{'current_value'};
$result = mysql_query("INSERT into data (ID, DATETIME, VALUE) select 'Temperature','".$datumCas->format('Y-m-d H:i:s')."',".$temperature);

$temp = $pachube->getDatastream('json', $feed, 'WindD');
$json = json_decode($temp);
$windDirection = $json->{'current_value'};
$result = mysql_query("INSERT into data (ID, DATETIME, VALUE) select 'WindDir','".$datumCas->format('Y-m-d H:i:s')."',".$windDirection);


//vypocet rosneho bodu
$pachube->updateDatastream("csv", $feed, "DewPoint", strval(sprintf("%.1f",calcDewPoint($humidity, $temperature))));


function calcDewPoint ($h, $t)  
{  
    $logEx = 0.66077 + (7.5 * $t) / (237.3 + $t) + (log10($h) - 2);  
    return ($logEx - 0.66077) * 237.3 / (0.66077 + 7.5 - $logEx);  
} 


?><!--WZ-REKLAMA-1.0IZ--><div align="center"><table width="496" border="0"
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