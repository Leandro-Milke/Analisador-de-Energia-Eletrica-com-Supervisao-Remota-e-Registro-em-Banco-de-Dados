<?php
				session_start();
				//echo $_SESSION['valor'];
				// arquivo para gerar o grafico quando acessar a tela de historico

    			include 'grafico/phplot.php';
    			include 'grafico/rgb.inc.php';
    			

    			// inclui arquivos para conectar ao BD.
				include 'comBD.php';

					// seleciona a tabela
				$data =  $_SESSION['data']; //'2020-05-23';
				

				$SQL = "SELECT * FROM `acomulador` WHERE Data = '{$data}' limit 24";
    			$dados = mysqli_query($mysqli,$SQL);
    			if($dados == false){
    			die('erro');
    			}
    			//$medicoes = mysqli_fetch_assoc($dados);

    			

    			$grafico = new PHPlot(1242,400);

    			$grafico -> SetFileFormat("png");
    			//$grafico -> SetPlotType('bars');

	

    			// seleção da grandeza
 				$grandeza =  $_SESSION['valor']; 

 			

				#Indicamos o título do gráfico e o título dos dados no eixo X e Y do mesmo
 				if ($grandeza === 'Tensao_Max') {      // para seeção do titulo
 					$grafico->SetTitle("Tensao Maxima");
 				}
 				elseif ($grandeza === 'Tensao_Min') {
 					$grafico->SetTitle("Tensao Minima");
 				}
 				elseif ($grandeza === 'Corrente_Max') {
 					$grafico->SetTitle("Corrente Maxima");
 				}
 				elseif ($grandeza === 'VA_Max') {
 					$grafico->SetTitle("Potencia Aparente Maxima");
 				}
 				elseif ($grandeza === 'VA_Min') {
 					$grafico->SetTitle("Potencia Aparente Minima");
 				}
 				elseif ($grandeza === 'W_Max') {
 					$grafico->SetTitle("Potencia Ativa Maxima");
 				}
 				elseif ($grandeza === 'W_Min') {
 					$grafico->SetTitle("Potencia Ativa Minima");
 				}
 				elseif ($grandeza === 'Kwh') {
 					$grafico->SetTitle("Kwh");
 				}

				$grafico->SetXTitle("Hora");
				$grafico->SetYTitle("Valor");
 				
 					
 
				#Definimos os dados do gráfico
				
				$dadosGrafico = array();
				$quanidadeDados = mysqli_num_rows($dados);
				foreach ($dados as $res) {
					$aux = array();
					$Hora = $res['Hora'];
					array_push($aux,$res['Hora'],$res[$grandeza]);
					array_push($dadosGrafico, $aux);

				}
			
 
				$grafico->SetDataValues($dadosGrafico);
 
				#Exibimos o gráfico
				$grafico->DrawGraph();
				
				
  			
		?>