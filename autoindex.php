<?php
$path    = $_GET["PATH"];
$files = scandir($path);
$files = array_diff(scandir($path), array('.', '..'));
echo "<ul>";
// echo "<li><a href='/$path/..'>..</a></li>";
foreach($files as $file){
  echo "<li><a href='/$path/$file'>$file</a></li>";
}
echo "<ul>";
?>