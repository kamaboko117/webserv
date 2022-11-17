
<?PHP
 if (isset($_POST ["cookie"]))
 {
   $cookie = $_POST ["cookie"];
 }
 else if (isset($_COOKIE["cookie"])){
    $cookie = $_COOKIE["cookie"];
 }
 else{
   $cookie = "";
 }

    $cookie_name = "cookie";
    $cookie_value = $cookie;
    setcookie($cookie_name, $cookie_value, time() + (86400 * 30), "/"); // 86400 = 1 day

?>

<html>
<body>

<FORM ACTION=/cgi-bin/cookie.php METHOD=POST>

<P>Enter input:
<?PHP
  echo " <INPUT TYPE=TEXT NAME=cookie VALUE=\"$cookie\" SIZE=75>\n";
?>
<P><BR><P>
<INPUT TYPE=SUBMIT VALUE="Show Me!">


<?php
if(!isset($_COOKIE[$cookie_name])) {
  echo "Cookie named '" . $cookie_name . "' is not set!";
  var_dump($_COOKIE);
} else {
  echo "Cookie '" . $cookie_name . "' is set!<br>";
  echo "Value is: " . $_COOKIE[$cookie_name];
}
?>

</body>
</html>