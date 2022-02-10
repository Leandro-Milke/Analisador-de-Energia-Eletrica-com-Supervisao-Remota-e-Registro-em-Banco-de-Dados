<?php
 session_start();
// para ocultar erros na página
ini_set('display_errors', 0);

//Incluimos o código de conexão ao BD
include 'comBD.php';

$SQL = "SELECT * FROM `configuracao`";
   $dados = mysqli_query($mysqli,$SQL);
    if($dados == false){
    	die('erro');
    }
    $protecoes = mysqli_fetch_assoc($dados);
?>

<!DOCTYPE html>
<HTML>
<HEAD>
	<META charset="UTF-8">
	<TITLE>TCC - Analisador de Energia</TITLE>
	<LINK rel="stylesheet" TYPE="text/css" href="css/folhadeestilo.css">
	<script type="text/javascript" src="js\jquery-3.4.1.js"></script>

</HEAD>
<body>

		<div class="container-principal">
			<header>
				<div class="logo">
					<a href="index.html">
						<img src="imagem/logoTCC.png" alt="logo site1" href="TCC/index.php">
					</a>
				</div>

				<nav>
					<ul>
						<li><a href="index.php">Home</a></li>
						<li><a href="historico.php">Histórico</a></li>
						<li><a href="Eventos.php">Eventos</a></li>
						<li><a href="Configuração.php">Configuração</a></li>
					</ul>
				</nav>		
			</header>

			<hr>

		</div>

		<div class="medicoes">
			<br><br>
			<center><h2> Parâmetros configurados <br><br></h2>
				<div  id="grandezas">
					<p>Tensão Alta [V]
					<p><?= $protecoes['Tensao_Max']?> 
				</div>
				<div  id="grandezas">
					<p>Tensão Baixa [V]
					<p><?= $protecoes['Tensao_Min']?>
				</div>
				<div id="grandezas">
					<p>Corrente Alta [A]
					<p><?= $protecoes['Corrente_Max']?> 
				</div>
				<div  id="grandezas">
					<p>Sobrecarga [W]
					<p><?= $protecoes['W_Max']?>
				</div>
			</center>

		</div>

		<div class="configuracao">
			<br><br>
			<form action="" method="post">
				<center><h2>Novos Valores</h2>
				<br><br>
				Tensao Alta: <input type="text" name="tensaoalta" size="4" maxlength="3" value="">    
				Tensao Baixa: <input type="text" name="tensaobaixa" size="4" maxlength="3" value="">    
				Corrente Alta: <input type="text" name="correntealta" size="4" maxlength="4" value="">   
				Sobrecarga: <input type="text" name="sobrecarga" size="4" maxlength="4" value="">
				<br><br><br><br>
  				<input type="submit" value="Salvar" onclick="Atualiza()" class="ImputConfirmar"/></center>
  				
			</form>

				<br><br><br><br>	 				
  			
		</div>


</body>
</HTML>

<script type="text/javascript">
					function Atualiza()
					{
					
					<?php
					$TensaoAlta = $_POST['tensaoalta'];
					$TensaoBaixa = $_POST['tensaobaixa'];
					$CorrenteAlta = $_POST['correntealta'];
					$VAAlta = $_POST['sobrecarga'];
					$WAlta = $_POST['sobrecarga'];

					// inserir dados
					$SQL = "UPDATE `configuracao` SET `id`=1,`Tensao_Max`='$TensaoAlta',`Tensao_Min`='$TensaoBaixa',`Corrente_Max`='$CorrenteAlta',`VA_Max`='$VAAlta',`W_Max`='$WAlta' WHERE 1";
	

					if (mysqli_query($mysqli,$SQL)=== true) {
						if( ($ESP32 = file_get_contents('http://192.168.0.30/aAtualiza_Parb'))=== false){}
						 else header('Refresh:0');
						
						} 
 					else{					
 						echo mysqli_errno($mysqli);
 						}

					?>
					}
					document.getElementById("salvar").onclick = function(e) {
						Atualiza();
						e.preventDefault();
					}
</script>










