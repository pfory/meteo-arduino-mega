<?php

//include('api.php');

require_once('api.php');
//$xi = \Xively\Api::forge();
$xi = \Xively\Api::forge('tlujVMiq756QxqK9sRIXbqwbIhyEcjAcZl3u7u1VOvpbYtw2');

var_dump($xi->paths);

echo("<br><br>Test Xively API");

$r = $xi->feeds()->read(array(
'per_page' => 10,
'page' => 5,
'tag' => 'Press',
))->get();
var_dump($r);

$r = $xi->feeds()->list()->get();
var_dump($r);
die();

?>