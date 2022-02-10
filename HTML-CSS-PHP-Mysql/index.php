<!DOCTYPE html>
<HTML>
<HEAD>
	<META charset="UTF-8" http-equiv="refresh" content="5">
	<TITLE>TCC - Analisador de Energia</TITLE>
	<LINK rel="stylesheet" TYPE="text/css" href="css/folhadeestilo.css">
	<script type="text/javascript" src="js\jquery-3.4.1.js"></script>

</HEAD>
<body>
		<?php
				// para ocultar erros na página
				ini_set('display_errors', 0);
				//error_reporting(0);

				// inclui arquivos aplicativos.
				include 'comBD.php';
				

    			// conexão com ESP32
    			$comunicacao = 'Ok';
    			
    				if( ($ESP32 = file_get_contents('http://192.168.0.30'))=== false){
    					$comunicacao = 'Falha de comunicação';
    					$tensao = 'xxx';
    					$corrente = 'xxx';
    					$potenca_aparente = 'xxx';
    					$potenca_ativa = 'xxx';
    					$FP = 'xxx';
    					$Kwh = 'xxx';
    					
    				}
    				else{
    					list($tensao, $corrente, $potenca_ativa, $potenca_aparente, $FP, $Kwh) = explode(":", $ESP32);
    					$comunicacao = 'Ok';
    				}			
				
  			
		?>

		<div class="container-principal">
			<header>
				<div class="logo">
					<a href="index.html">
						<img src="imagem/logoTCC.png" alt="logo site1" href="index.php">
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
			<div class="medicoes">
				<!--<img src="imagem/unifilardes.png" alt="uni">-->
				<br><br><br>
				<center><h2>Medições</h2></center>
				<br><br><br>
				<div  id="grandezas">
					<p>Tensão [V]
					<p><?= $tensao?> 
				</div>
				<div id="grandezas">
					<p>Corrente [A]
					<p><?= $corrente ?> 
				</div>
				<div  id="grandezas">
					<p>Potência Aparente [VA]
					<p><?= $potenca_aparente?> 
				</div>
				<div id="grandezas">
					<p>Potência Ativa [W]
					<p><?= $potenca_ativa?> 
				</div>
				<div id="grandezas">
					<p>Fator de Potência
					<p><?= $FP?>
				</div>
				<div id="grandezas">
					<p>Consumo [Kw/h]
					<p><?= $Kwh?> 
				</div>
				<h3>
					Comunicação:__<?=$comunicacao?>
				</h3>
			</div>

		
</body>
</HTML>
