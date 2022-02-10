<?php
	 session_start();		
	//Incluimos o código de conexão ao BD
	include 'comBD.php';

	$TensaoAlta = $_SESSION['TensaoAlta'];
	$TensaoBaixa = 1;//$_SESSION['TensaoBaixa'];
	$CorrenteAlta = 1;//$_SESSION['CorrenteAlta'];
	$VAAlta = 1;//$_SESSION['VAAlta'];
	$WAlta = 1;//$_SESSION['WAlta'];

	// como inserir dados
	$SQL = "UPDATE `configuracao` SET `id`=1,`Tensao_Max`='$TensaoAlta',`Tensao_Min`='$TensaoBaixa',`Corrente_Max`='$CorrenteAlta',`VA_Max`='$VAAlta',`W_Max`='$WAlta' WHERE 1";
	

	if (mysqli_query($mysqli,$SQL)=== true) {
		//echo "cad ok";
	} 
 	else{
 		//echo mysqli_errno($mysqli);
 	}
	?>