<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN">
<html>
<head>
    <meta http-equiv="content-type" content="text/html;charset=UTF-8">
    <title>Teploty</title>
    <link rel="stylesheet" type="text/css" href="main.css">
</head>
<body>

<a href=main.php>Zpět na hlavní stránku</a><br>

<?php

// Set timezone
date_default_timezone_set("+1");

//phpinfo();

$den = new DateTime('NOW');
printf("Datum a čas : %s",$den->format('l j.n.Y H:i:s e'));
//$den = new DateTime('2013-09-02');

// mysql
$c = mysql_connect("mysql.webzdarma.cz", "pandatel23", "c5e56n");
mysql_select_db("pandatel23");

$sql_base = "SELECT day(datetime) as den, min(value) as teplotaMin FROM data WHERE id='Temperature' ";
$sql_where = " and datetime between '".$den->format('Y-m')."-01 00:00:00' and '".$den->format('Y-m-t')." 23:59:59' ";
$sql_groupby = " group by month(datetime) ";
$sql = $sql_base.$sql_where.$sql_groupby;

$result = mysql_query($sql);
if (!$result) { // check for errors.
    echo 'Could not run query: ' . mysql_error();
    exit;
}

$minTempMonth = mysql_result($result, 0);

echo $minTempMonth;

$sql_base = "SELECT day(datetime) as den, max(value) as teplotaMin FROM data WHERE id='Temperature' ";
$sql_where = " and datetime between '".$den->format('Y-m')."-01 00:00:00' and '".$den->format('Y-m-t')." 23:59:59' ";
$sql_groupby = " group by month(datetime) ";
$sql = $sql_base.$sql_where.$sql_groupby;

$result = mysql_query($sql);
if (!$result) { // check for errors.
    echo 'Could not run query: ' . mysql_error();
    exit;
}

$maxTempMonth = mysql_result($result, 0);
echo $maxTempMonth;


//nejvyssi a nejnizsi teplota po dnech
$sql_base = "SELECT day(datetime) as den, min(value) as teplotaMin, max(value) as teplotaMax FROM data WHERE id='Temperature' ";
$sql_where = " and datetime between '".$den->format('Y-m')."-01 00:00:00' and '".$den->format('Y-m-t')." 23:59:59' ";
$sql_groupby = " group by dayofmonth(datetime) ";
$sql = $sql_base.$sql_where.$sql_groupby;

$result = mysql_query($sql);
if (!$result) { // check for errors.
    echo 'Could not run query: ' . mysql_error();
    exit;
}

printf("<div class=tabulka>");
printf("<table border=1>");
printf("<tr style='font-weight:bold'><td colspan=3>Minimální maximální teploty za měsíc %s po jednotlivých dnech</td></tr>",$den->format('F Y (n/Y)'));
printf("<tr><td>Den</td><td>min</td><td>max</td></tr>");
while ($row = mysql_fetch_array($result, MYSQL_NUM)) {
  // if ($row[1]>0)
  // {
    // printf("<tr><td>%s</td><td style='background-color:green;color:white'>%.1f</td><td style='background-color:green;color:white'>%.1f</td></tr>", $row[0], $row[1], $row[2]);  
  // }
  // else
  // {
    printf("<tr><td>%s</td><td>%.1f</td><td>%.1f</td></tr>", $row[0], $row[1], $row[2]);  
  //}
}
printf("</table>");

mysql_free_result($result);

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
<!--WZ-REKLAMA-1.0IK--></body>
</html>