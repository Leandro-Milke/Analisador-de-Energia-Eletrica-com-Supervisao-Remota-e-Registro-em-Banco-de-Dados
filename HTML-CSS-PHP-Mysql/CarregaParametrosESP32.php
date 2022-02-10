<?php
//Incluimos o código de conexão ao BD
include 'comBD.php';

$SQL = "SELECT * FROM `configuracao`";
   $dados = mysqli_query($mysqli,$SQL);
    if($dados == false){
    	die('erro');
    }
    $protecoes = mysqli_fetch_assoc($dados);



$tensaoMax =$protecoes['Tensao_Max'];
$tensaoMin = $protecoes['Tensao_Min'];
$correnteMax = $protecoes['Corrente_Max'];
$VAmax =  $protecoes['VA_Max'];
$Wmax = $protecoes['W_Max'];

date_default_timezone_set('America/Sao_Paulo');
$hora = date('i'); // pega apenas o minuto para sincronisar o ESP32

$StringEnvio = 'a'.$tensaoMax.'b'.$tensaoMin.'c'.$correnteMax.'d'.$VAmax.'e'.$Wmax.'f'.$hora.'g';

echo "$StringEnvio";

$mysqli-> close();

?>
