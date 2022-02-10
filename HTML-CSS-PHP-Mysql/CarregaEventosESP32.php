<?php
//Incluimos o código de conexão ao BD
include 'comBD.php';

$evento = $_GET["Evento"];


//Captura a Data e Hora do SERVIDOR (onde está hospedada a página):
date_default_timezone_set('America/Sao_Paulo');
$hora = date('H:i:s');
$data = date('Y-m-d');

// Insere no BD o evento
$SQL = "INSERT INTO `eventos`(`Evento`, `Data`, `Hora`) VALUES ('{$evento}','{$data}','{$hora}')";

if (mysqli_query($mysqli,$SQL)=== true) {

	echo "cad do evento ok";
 } 
 else{
 	echo "erro1";
 	echo mysqli_errno($mysqli);
 }

$mysqli-> close();


?>
