<?php
	 session_start();		
	//Incluimos o código de conexão ao BD
	include 'comBD.php';

	$TensaoAlta = $_SESSION['TensaoAlta'];
	$TensaoBaixa = 1;
	$CorrenteAlta = 1;
	$VAAlta = 1;
	$WAlta = 1;

	// Atualiza BD com novos valores
	$SQL = "UPDATE `configuracao` SET `id`=1,`Tensao_Max`='$TensaoAlta',`Tensao_Min`='$TensaoBaixa',`Corrente_Max`='$CorrenteAlta',`VA_Max`='$VAAlta',`W_Max`='$WAlta' WHERE 1";
	

	if (mysqli_query($mysqli,$SQL)=== true) {
		//echo "cad ok";
	} 
 	else{
 		//echo mysqli_errno($mysqli);
 	}
	?>