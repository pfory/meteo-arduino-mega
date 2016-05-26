<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN">
<html>
<head>
    <meta http-equiv="content-type" content="text/html;charset=UTF-8">
    <title>Srážky</title>
    <link rel="stylesheet" type="text/css" href="main.css">
</head>
<body>

<a href=main.php>Zpět na hlavní stránku</a><br>


<?php

// Set timezone
date_default_timezone_set("+1");
$koef = 0.2;

//phpinfo();

class DateTimeCzech extends DateTime {
    public function format($format) {
        $english = array('Monday', 'Tuesday', 'Wednesday', 'Thursday', 'Friday', 'Saturday', 'Sunday', 
          'January', 'February', 'March', 'April', 'May', 'June', 'July', 'August', 'September', 'October', 'November', 'December');
        $czech = array('Pondělí', 'Úterý', 'Středa', 'Čtvrtek', 'Pátek', 'Sobota', 'Neděle',
          'Leden', 'Únor', 'Březen', 'Duben', 'Květen', 'Červen', 'Červenec', 'Srpen', 'Září', 'Říjen', 'Listopad', 'Prosinec');
        return str_replace($english, $czech, parent::format($format));
    }
}

/*$today = unixtojd(mktime(0, 0, 0, 8, 16, 2003));
echo $today;
print_r(cal_from_jd($today, CAL_GREGORIAN));
*/

if ($_GET["day"]) {
  $dArray=cal_from_jd($_GET["day"], CAL_GREGORIAN);
  $den=new DateTimeCzech($dArray[year].'-'.$dArray[month].'-'.$dArray[day]);
}
else {
  $den=new DateTimeCzech('NOW');
}

$denJD=cal_to_jd(CAL_GREGORIAN, $den->format('n'), $den->format('j'), $den->format('Y'));

/*
$days_in_month=date('t');
//printf("Dnu v aktualnim mesici %s", $days_in_month);
//echo "</br>";
$denPred = $den->format('j')-1;

if ($denPred==0) {
  $d=new DateTimeCzech('2013-'.(((int) $_GET["month"])-1).'-'.(int) $_GET["day"]);
  echo $d->format('l j.n.Y H:i:s e');
  $denPred=date('t');
  $mesicPred=$den->format('n')-1;
} else {
  $mesicPred=$den->format('n');
}
$denPo=$denPred+2;
if ($denPo>$days_in_month) {
  $denPo=1;
  $mesicPo=$den->format('n')+1;
} else {
  $mesicPo=$den->format('n');
}
*/

$dnes=new DateTimeCzech('NOW');

printf("<a href='rain.php'>Dnes</a> je %s</br>", $dnes->format('l j.n.Y H:i:s e'));
printf("<a href='rain.php?day=%s'><< </a>",$denJD-1);
printf("%s", $den->format('l j.n.Y'));
printf("<a href='rain.php?day=%s'> >></a>",$denJD+1);


// mysql
$c = mysql_connect("mysql.webzdarma.cz", "pandatel23", "c5e56n");
mysql_select_db("pandatel23");

$result = mysql_query("SET lc_time_names = 'cs_CZ'");
if (!$result) { // check for errors.
    echo 'Could not run query: ' . mysql_error();
    exit;
}


//souhrn za den po jednotlivych hodinach
$sql_base = "SELECT hour(datetime) as hour, sum(value) * ".$koef." as mm FROM data WHERE id='Rain' ";
$sql_where = " and datetime between '".$den->format('Y-m-d')." 00:00:00' and '".$den->format('Y-m-d')." 23:59:59' ";
$sql_groupby = " group by hour(datetime) "; 
$sql = $sql_base.$sql_where.$sql_groupby;

$result = mysql_query($sql);
if (!$result) { // check for errors.
    echo 'Could not run query: ' . mysql_error();
    exit;
}

$maximum = -1;
while ($row = mysql_fetch_array($result, MYSQL_NUM)) {
  if ($row[1]>$maximum && $row[1]>0) {
    $maximum = $row[1];
  }
}
  
$result = mysql_query($sql);
  
$naprseloDen = 0;
printf("<div class=tabulka>");
printf("<table class=tabulka>");
printf("<tr style='font-weight:bold'><td colspan=2>Souhrn srážek v mm za %s po jednotlivých hodinách</td></tr>",$den->format('l j F Y (j.n.Y)'));
printf("<tr><td>Hodina</td><td>mm</td></tr>");
while ($row = mysql_fetch_array($result, MYSQL_NUM)) {
  if ($row[1]>0) {
   $naprseloDen += $row[1];
    printf("<tr><td>%s</td><td class=zvyrazni>%s", $row[0], round($row[1],2));  
  }
  else
  {
    printf("<tr><td>%s</td><td>", $row[0]);  
  }
  if ($row[1] == $maximum) {
    printf(" (max)");
  }
  printf("</td></tr>");
}
printf("</table>");
printf("</div>");


if ($naprseloDen>0) {
  if ($den==$dnes)
    printf("Dnes napršelo už %s mm.<br>", $naprseloDen);
  else
    printf("Celkem napršelo %s mm.<br>", $naprseloDen);
}
else
{
  if ($den==$dnes)
    printf("<div style='color:red'>Dnes ještě nepršelo!!!</div>");
}

mysql_free_result($result);

//$sql_base = "SELECT DATE_FORMAT(max(datetime),'%W %e.%c.%Y %k:%i:%s') as datumcas FROM data WHERE id='Rain' ";
$sql_base = "SELECT max(datetime) as datumcas FROM data WHERE id='Rain' ";
$sql_where = " and value>0 ";
$sql = $sql_base.$sql_where;

$result = mysql_query($sql);
if (!$result) { // check for errors.
    echo 'Could not run query: ' . mysql_error();
    exit;
}

$naposledPrselo = new DateTimeCzech(mysql_result($result, 0));

printf("Naposled pršelo : %s (už je to %s).", $naposledPrselo->format('l j.n.Y G:i:s'), dateDiff($dnes->format('Y-m-d H:i:s'), $naposledPrselo->format('Y-m-d H:i:s')));

mysql_free_result($result);

//souhrn srazek za mesic po jednotlivych dnech
$sql_base = "SELECT day(datetime) as den, dayname(datetime) as denJmeno, sum(value) *".$koef." as mm FROM data WHERE id='Rain' ";
$sql_where = " and datetime between '".$den->format('Y-m')."-01 00:00:00' and '".$den->format('Y-m-t')." 23:59:59' ";
$sql_groupby = " group by dayofmonth(datetime) ";
$sql = $sql_base.$sql_where.$sql_groupby;

$result = mysql_query($sql);
if (!$result) { // check for errors.
    echo 'Could not run query: ' . mysql_error();
    exit;
}

$maximum = -1;
while ($row = mysql_fetch_array($result, MYSQL_NUM)) {
  if ($row[2]>$maximum && $row[2]>0) {
    $maximum = $row[2];
  }
}


$result = mysql_query($sql);

$naprseloMesic = 0;
printf("<div class=tabulka>");
printf("<table class=tabulka>");
printf("<tr style='font-weight:bold'><td colspan=2>Souhrn srážek v mm za %s po jednotlivých dnech</td></tr>",$den->format('F Y (n/Y)'));
printf("<tr><td>Den</td><td>mm</td></tr>");
$den1=cal_to_jd(CAL_GREGORIAN, $den->format('n'), 1, $den->format('Y'))-1;
while ($row = mysql_fetch_array($result, MYSQL_NUM)) {
  if ($row[1]=="Sobota" || $row[1]=="Neděle")
    printf("<tr><td style='background-color:lightgreen'>");
  else
    printf("<tr><td>");
  
  printf("<a href='rain.php?day=%s'>%s %s</a></td>", $den1+$row[0], $row[0], $row[1]);  

  if ($row[2]>0)
  {
    $naprseloMesic += $row[2];
    printf("<td class=zvyrazni>%s", round($row[2],2));  
  }
  else
  {
    printf("<td>");  
  }
  if ($row[2] == $maximum) {
    printf(" (max)");
  }

  printf("</td></tr>");
  
}
printf("</table>");
printf("</div>");


if ($naprseloMesic>0) {
  printf("Za %s napršelo už %s mm.<br>", $den->format('F Y'), $naprseloMesic);
}
else
{
  printf("<div style='color:red'>Tento měsíc ještě nepršelo!!!</div>");
}

mysql_free_result($result);


//souhrny srazek po mesicich
$sql_base = "SELECT month(datetime) as mesic, monthname(datetime) as mesicJmeno, sum(value) *".$koef." as mm FROM data WHERE id='Rain' ";
$sql_where = " and datetime between '".$den->format('Y')."-01-01 00:00:00' and '".$den->format('Y')."-12-31 23:59:59' ";
$sql_groupby = " group by month(datetime) ";
$sql = $sql_base.$sql_groupby;

$result = mysql_query($sql);
if (!$result) { // check for errors.
    echo 'Could not run query: ' . mysql_error();
    exit;
}

$maximum = -1;
while ($row = mysql_fetch_array($result, MYSQL_NUM)) {
  if ($row[2]>$maximum && $row[2]>0) {
    $maximum = $row[2];
  }
}

$result = mysql_query($sql);

$naprseloRok = 0;
printf("<div class=tabulka>");
printf("<table class=tabulka>");
printf("<tr style='font-weight:bold'><td colspan=2>Souhrn srážek v mm za rok po jednotlivých měsících</td></tr>",$den->format('Y'));
printf("<tr><td>Měsíc</td><td>mm</td></tr>");
while ($row = mysql_fetch_array($result, MYSQL_NUM)) {
  printf("<tr><td><a href='rain.php?day=%s'>%s - %s</a></td>", cal_to_jd(CAL_GREGORIAN, $row[0], 1, $den->format('Y')), $row[0], $row[1]);  
  if ($row[2]>0)
  {
    $naprseloRok += $row[2];
    printf("<td class=zvyrazni>%s", round($row[2],2));  
  }
  else
  {
    printf("<td>");  
  }
  if ($row[2] == $maximum) {
    printf(" (max)");
  }

  printf("</td></tr>");
  
}
printf("</table>");
printf("</div>");

if ($naprseloRok>0) {
  printf("Za rok %s napršelo už %s mm.<br>", $den->format('Y'), round($naprseloRok,2));
}
else
{
  printf("<div style='color:red'>Letos ještě nepršelo!!!</div>");
}


mysql_free_result($result);


//souhrn srazek za poslednich 24 hodin
$sql_base = "SELECT sum(value) *".$koef." as mm FROM data WHERE id='Rain' ";
$sql_where = " and datetime between addtime(now(),'-23:59:59') and now() ";
$sql = $sql_base.$sql_where;

$result = mysql_query($sql);
if (!$result) { // check for errors.
    echo 'Could not run query: ' . mysql_error();
    exit;
}

printf("</div>");

printf("<div>");
printf("<table class=tabulka>");
printf("<tr><td>Souhrn srážek za posledních 24 hodin</td><td>%s mm</td></tr>", round(mysql_result($result, 0),2));

mysql_free_result($result);

//souhrn srazek za posledni hodinu
$sql_base = "SELECT sum(value) *".$koef." as mm FROM data WHERE id='Rain' ";
$sql_where = " and datetime between addtime(now(),'-0:59:59') and now() ";
$sql = $sql_base.$sql_where;

$result = mysql_query($sql);
if (!$result) { // check for errors.
    echo 'Could not run query: ' . mysql_error();
    exit;
}

printf("<tr><td>Souhrn srážek za poslední hodinu</td><td>%s mm</td></tr>", round(mysql_result($result, 0),2));

mysql_free_result($result);

//souhrn srazek za poslednich 10 minut
$sql_base = "SELECT sum(value) *".$koef." as mm FROM data WHERE id='Rain' ";
$sql_where = " and datetime between addtime(now(),'-0:09:59') and now() ";
$sql = $sql_base.$sql_where;

$result = mysql_query($sql);
if (!$result) { // check for errors.
    echo 'Could not run query: ' . mysql_error();
    exit;
}

printf("<tr><td>Souhrn srážek za posledních 10 minut</td><td>%s mm</td></tr>", round(mysql_result($result, 0),2));

printf("</table>");
printf("</div>");


mysql_free_result($result);

 
// Time format is UNIX timestamp or
// PHP strtotime compatible strings
function dateDiff($time1, $time2, $precision = 6) {
  $english = array('year', 'month', 'day', 'hours', 'minute', 'seconds', 'second');
  $czech = array('rok', 'měsíc', 'den', 'hodin', 'minuta', 'vteřin', 'vteřina');


  // If not numeric then convert texts to unix timestamps
  if (!is_int($time1)) {
    $time1 = strtotime($time1);
  }
  if (!is_int($time2)) {
    $time2 = strtotime($time2);
  }

  // If time1 is bigger than time2
  // Then swap time1 and time2
  if ($time1 > $time2) {
    $ttime = $time1;
    $time1 = $time2;
    $time2 = $ttime;
  }

  // Set up intervals and diffs arrays
  $intervals = array('year','month','day','hour','minute','second');
  $diffs = array();

  // Loop thru all intervals
  foreach ($intervals as $interval) {
    // Create temp time from time1 and interval
    $ttime = strtotime('+1 ' . $interval, $time1);
    // Set initial values
    $add = 1;
    $looped = 0;
    // Loop until temp time is smaller than time2
    while ($time2 >= $ttime) {
      // Create new temp time from time1 and interval
      $add++;
      $ttime = strtotime("+" . $add . " " . $interval, $time1);
      $looped++;
    }

    $time1 = strtotime("+" . $looped . " " . $interval, $time1);
    $diffs[$interval] = $looped;
  }

  $count = 0;
  $times = array();
  // Loop thru all diffs
  foreach ($diffs as $interval => $value) {
    // Break if we have needed precission
    if ($count >= $precision) {
      break;
    }
    // Add value and interval 
    // if value is bigger than 0
    if ($value > 0) {
      // Add s if value is not 1
      if ($value != 1) {
        $interval .= "s";
      }

      //$interval = str_replace($english, $czech, $interval);

      // Add value and interval to times array
      $times[] = $value . " " . $interval;
      $count++;
    }
  }
  
  // Return string with times
  return implode(", ", $times);
}

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