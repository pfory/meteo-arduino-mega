<?php
// odkazy na konkretni dny
$selected_days[2009][2][24] = true;
$selected_days[2009][2][27] = true;
$selected_days[2009][3][5]  = true;
$selected_days[2009][3][17] = true;
$selected_days[2009][4][20] = true;

// prvotni inicializace, nastavime aktualni cas,  mesic a rok
$date = time();
$day = date('d', $date);
$month = date('m', $date);
$year = date('Y', $date);


// pokud v GETu existuje mesic ci rok, tak "aktualni" mesic a rok je ten z URL
if ($_GET["year"]) $year = (int) $_GET["year"];
if ($_GET["month"]) $month = (int) $_GET["month"];

// pro dalsi zobrazeni potrebujeme prvni den v mesici, pocet dni v mesici ...
$first_day = mktime(0,0,0,$month, 1, $year);
$title = date('n', $first_day);
$day_of_week = date('D', $first_day);
$days_in_month = date('t', $first_day);


// ceske mesice
$cz_months = array( 
                    1 => "Leden",
                         "Únor",
                         "Bøezen",
                         "Duben",
                         "Kvìten",
                         "Èerven",
                         "Èervenec",
                         "Srpen",
                         "Záøí",
                         "Øíjen",
                         "Listopad",
                         "Prosinec"
                );

$prevY = $nextY = $year;
$prevM = $nextM = $month;

// nastaveni odkazu pro predchozi a nasledujici mesic / rok
if ($month - 1 < 1) { $prevM = 12; $prevY--;} else {$prevM = $month - 1;}; 
if ($month + 1 > 12){ $nextM = 1; $nextY++;} else {$nextM = $month + 1;};  

$prev = "<a href='?month=".($prevM)."&year=".($prevY)."'><<</a>";
$next = "<a href='?month=".($nextM)."&year=".($nextY)."'>>></a>";

// timto si vyplnime v kalendari prazdne bunky, 1 den v mesici a prvniho neni vzdy pondeli ...
$emptyTD = array("Mon" => 0, "Tue" => 1, "Wed" => 2, "Thu" => 3, "Fri" => 4, "Sat" => 5, "Sun" => 6);
$blank = $emptyTD[$day_of_week];

// vykresleni kalendare
echo "<table border=1 align=center id=calendar>";
echo "<tr><th>$prev</th><th colspan=5> ".$cz_months[$title]." $year</th><th>$next</th></tr>";
echo "<tr><td width=42>M</td><td width=42>T</td><td width=42>W</td><td width=42>T</td><td width=42>F</td><td width=42>S</td><td width=42>S</td></tr>";

$day_count = 1;

echo "<tr>";

// zde prave zjistime pocet prazdnych bunek pred 1 dnem v mesici 
while ( $blank > 0 )
{
  echo "<td></td>";
  $blank--;
  $day_count++;
}

$day_num = 1;


// veskere dny v kalendari
while ($day_num <= $days_in_month)
{
    
    if (isset($selected_days[$year][$month][$day_num]))
    {
    
      if($day_num == $day)
      {
        echo "<td class='today'><b><a href='?year=$year&month=$month&day=$day_num'>$day_num</a></b></td>";
      }
      else
      {
        echo "<td class='days'><a href='?year=$year&month=$month&day=$day_num'>$day_num</a></td>";
      }
    
    }
    else
    {
        echo "<td class='days'>$day_num</td>";  
    }   
    
    


  $day_num++;
  $day_count++;
  
  if ($day_count > 7)
  {
    echo "</tr><tr>";
    $day_count = 1;
  }
}


// timto zajistime spravne zobrazeni kalendare a dopocitani prazdnych bunek
while ($day_count >1 && $day_count <=7)
{
  echo "<td> </td>";
  $day_count++;
}

echo "</tr></table>";
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