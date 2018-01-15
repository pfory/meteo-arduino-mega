<?php

include('PachubeAPI.php');

$pachube = new PachubeAPI("azCLxsU4vKepKymGFFWVnXCvTQ6Ilze3euIsNrRKRRXuSPO8");
$feed = 538561447;
$user = "datel";

// mysql
$c = mysql_connect("mysql.webzdarma.cz", "pandatel23", "c5e56n");
mysql_select_db("pandatel23");


$temp = $pachube->getDatastream('json', $feed, 'Status');
$json = json_decode($temp);
$status = $json->{'current_value'};

$temp = $pachube->getDatastream('json', $feed, 'IN');
$json = json_decode($temp);
$IN = $json->{'current_value'};

$temp = $pachube->getDatastream('json', $feed, 'OUT');
$json = json_decode($temp);
$OUT = $json->{'current_value'};

$temp = $pachube->getDatastream('json', $feed, 'ROOM');
$json = json_decode($temp);
$ROOM = $json->{'current_value'};

$temp = $pachube->getDatastream('json', $feed, 'const');
$json = json_decode($temp);
$const = $json->{'current_value'};

$temp = $pachube->getDatastream('json', $feed, 'EnergyAday');
$json = json_decode($temp);
$energyDay = $json->{'current_value'};

$updateEnergy = 0;
if ($energyDay != null && $OUT != null && $IN != null) {
  $updateEnergy = 1; 
}

//nulovani denni energie
$den=new DateTime('NOW');
if ($den != null) {
  if ($den->format('H') * 60 + $den->format('i') < 60) {
    $energyDay = 0;
    $pachube->updateDatastream("csv", $feed, "EnergyAday", sprintf("%8.4f",$energyDay)); //strval(printf("%8.4f",$energyDay + $energyDiff));
  }
}

$temp = $pachube->getDatastream('json', $feed, 'EnergyTotal');
$json = json_decode($temp);
$energyTotal = $json->{'current_value'};
$updateEnergyTotal = 0; 
if ($energyTotal != null && $energyTotal>0) {
  $updateEnergyTotal = 1; 
}


$power = 0;
if ($status==1) { //TOPIR
  if ($OUT-$IN>0)
  {
    $power = ($OUT-$IN) * $const;
    $energyDiff = ($power * 5) / 3600 / 1000;
    $energyDay = $energyDay + $energyDiff; //kWh
    $energyTotal = $energyTotal + $energyDiff; //kWh
  } 
}
$pachube->updateDatastream("csv", $feed, "Power", strval($power));

//$pachube->updateDatastream("csv", $feed, "EnergyAday", strval($energyDay)); //strval(printf("%8.4f",$energyDay + $energyDiff));
if ($updateEnergy==1) {
  if ($energyDay>0) {
    $pachube->updateDatastream("csv", $feed, "EnergyAday", sprintf("%8.4f",$energyDay)); //strval(printf("%8.4f",$energyDay + $energyDiff));
  }
  if ($updateEnergyTotal==1) {
    if ($energyTotal>0) {
      $pachube->updateDatastream("csv", $feed, "EnergyTotal", sprintf("%8.4f",$energyTotal)); //strval(printf("%8.4f",$energyDay + $energyDiff));
    }
  }
}

$datumCas = new DateTime('NOW');
$result = mysql_query("INSERT into dataSolar (DATETIME, IN0, OUT0, ROOM, STATUS) values ('".$datumCas->format('Y-m-d H:i:s')."',".$IN.",".$OUT.",".$ROOM.",".$status.")");


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