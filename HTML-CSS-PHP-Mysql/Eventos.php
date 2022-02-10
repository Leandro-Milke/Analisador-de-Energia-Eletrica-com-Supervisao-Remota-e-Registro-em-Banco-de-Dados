<?php
 session_start();
// para ocultar erros na página
ini_set('display_errors', 0);
//Incluimos o código de conexão ao BD
//include 'comBD.php';
?>


<!DOCTYPE html>
<HTML>
<HEAD>
	<META charset="UTF-8">
	<TITLE>TCC - Analisador de Energia</TITLE>
	<LINK rel="stylesheet" TYPE="text/css" href="css/folhadeestilo.css">
	<script type="text/javascript" src="js\jquery-3.4.1.js"></script>
	<script src="http://ajax.googleapis.com/ajax/libs/jquery/1.11.0/jquery.min.js"></script>

</HEAD>
<body>

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
			

		</div>

		<div>
			<br><br>
				<form action="" method="post">
				<center><h2>Selecione a data:________  <input name="date" type="date" class="ImputData">
  				<input type="submit" value="Buscar" class="ImputConfirmar" /></h2></center>
  				</form>

  				<?php 
  					$_SESSION['data'] = $_POST['date'];
  					
  				?>
				
			  			
		</div>
		<div class="eventos">
			<br><br>
    	   <center><h2>Registro de Eventos</h2>
    	   	<br><br>
    	   <iframe marginwidth="0" marginheight="0" width="800" height="1008" scrolling="no" frameborder="0" src="TabelaEventos.php"></iframe></center>
		</div>
</body>
</HTML>


