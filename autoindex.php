<?php
$path    = $_GET["PATH"];
$files = scandir($path);
$files = array_diff(scandir($path), array('.', '..'));
foreach($files as $file){
  echo "<a href='$path/$file'>$file</a>";
  echo "<P>";
}
?>