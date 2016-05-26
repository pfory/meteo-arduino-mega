<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN">
<html>
<head>
    <meta http-equiv="content-type" content="text/html;charset=UTF-8">
    <title>Průměrné teploty</title>
    <link rel="stylesheet" type="text/css" href="main.css">
</head>
<body>

<a href=main.php>Zpět na hlavní stránku</a><br>


<?php

// mysql
$c = mysql_connect("mysql.webzdarma.cz", "pandatel23", "c5e56n");
mysql_select_db("pandatel23");

$sql="SELECT DAY (datetime) AS den, month( datetime ) AS mesic, year( datetime ) AS rok, sum( value ) / count( * ) AS prumernaTeplota, count( * ) AS pocetMereni FROM data 
WHERE id = 'Temperature' GROUP BY DAY (datetime), month( datetime ) , year( datetime ) 
ORDER BY year( datetime ), month( datetime ), DAY (datetime)";

$result = mysql_query($sql);

if (!$result) { // check for errors.
    echo 'Could not run query: ' . mysql_error();
    exit;
}
  
$naprseloDen = 0;
printf("<div class=tabulka>");
printf("<table class=tabulka>");
printf("<tr style='font-weight:bold'><td colspan=5>Průměrné teploty</td></tr>");
printf("<tr><td>Den</td><td>Měsíc</td><td>Rok</td><td>Průměrná teplota</td><td>Počet měření</td></tr>");
while ($row = mysql_fetch_array($result, MYSQL_NUM)) {
  printf("<tr><td>%s</td><td>%s</td><td>%s</td><td>%4.1f</td><td>%s</td></tr>", $row[0], $row[1], $row[2], $row[3], $row[4]);  
}
printf("</table>");
printf("</div>");

?>

</br>
</br>

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