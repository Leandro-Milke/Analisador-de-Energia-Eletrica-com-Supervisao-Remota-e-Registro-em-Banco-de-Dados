<?php
 session_start();
// para ocultar erros na página
ini_set('display_errors', 0);
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
			<br>
			<diva>
				
				<form action="" method="post">
				
				Selecionar dados:
  				<select name="grandezas" class="SelectGrandezas">
  				<option value='Tensao_Max'>Tensao Maxima</option>
  				<option value='Tensao_Min'>Tensao Minima</option>
  				<option value='Corrente_Max'>Corrente Maxima</option>
  				<option value='VA_Max'>Potencia Aparente Maxima</option>
  				<option value='VA_Min'>Potencia Aparente Minima</option>
  				<option value= 'W_Max'>Potencia Ativa Maxima</option>
  				<option value='W_Min'>Potencia Ativa Minima</option>
  				<option value='Kwh'>Kw</option>
  				</select>
  				
  				Data:
				<input name="date" type="date" required pattern="[0-9]{4}-[0-9]{2}-[0-9]{2}" class="ImputData"> 
				
  				<input type="submit" value=" Confirmar " name="submeter" class="ImputConfirmar">

  				<?php $_SESSION['valor'] = $_POST['grandezas'];
  					  $_SESSION['data'] =  $_POST['date'];
  				?>


				</form>
			</div>
			<br>
			<div>
				<img alt= "" src="grafico.php" title="">
			</div>	

		</div>
</body>
</HTML>

<script>
$(function() {
  // #datepicker é o seletor para o campo criado
  // datepicker() é o método do plugin que diz que aquele campo
  // deverá obedecer as regras de data
  $('#datepicker').datepicker();
});
</script>