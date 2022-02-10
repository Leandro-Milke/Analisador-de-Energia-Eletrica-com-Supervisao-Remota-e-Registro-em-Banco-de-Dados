<?php
//Incluimos o código de conexão ao BD
include 'comBD.php';
//Variável responsável por guardar o valor enviado pelo ESP32
//$id = $_GET["id"];
$tensaoMax = $_GET["tensao_max"];
$tensaoMin = $_GET["tensao_minima"];
$corrente = $_GET["corrente_maxima"];
$VAmax = $_GET["VA_maximo"];
$VAmin = $_GET["VA_minimo"];
$Wmax = $_GET["W_maximo"];
$Wmin = $_GET["W_minimo"];
$Kwh = $_GET["Kwh"];


//Captura a Data e Hora do SERVIDOR (onde está hospedada a página):
date_default_timezone_set('America/Sao_Paulo');
$hora = date('H:i:s');
$data = date('Y-m-d');


// como inserir dados
$SQL = "INSERT INTO `acomulador`(`Tensao_Max`,`Tensao_Min`, `Corrente_Max`,`VA_Max`, `VA_Min`, `W_Max`, `W_Min`, `Kwh`, `Data`, `Hora`) VALUES ('{$tensaoMax}','{$tensaoMin}','{$corrente}','{$VAmax}','{$VAmin}','{$Wmax}','{$Wmin}','{$Kwh}','{$data}','{$hora}')";

$hora = date('i'); // pega apenas o mnuto para sincronisar a hora$

$StringEnvio = 'a'.'cad ok'.'b'.$hora.'c';
if (mysqli_query($mysqli,$SQL)=== true) {

	echo "$StringEnvio";
 } 
 else{
 	echo "erro1";
 	echo mysqli_errno($mysqli);
 }




$mysqli-> close();


?>
