<?php
	 session_start();			
    // inclui arquivos para conectar ao BD.
	include 'comBD.php';

	$data = $_SESSION['data'];
					
	$SQL = "SELECT * FROM `eventos` WHERE Data = '{$data}'";
    $dados = mysqli_query($mysqli,$SQL);
    if($dados == false){
    	die('erro');
    }
    echo '<table width="100%" border="1" cellspacing="0" cellpadding="0" BORDER RULES="rows" table style="table-layout: fixed; color:#000); width: 760px; overflow: hidden;" >';
	echo "<tr>"; 
	echo"<td align=center>"; echo "			Evento"; echo"	 		</td>";
	
	echo "<td align=center>"; echo "			Data"; echo "   		</td>";
	echo "<td align=center>"; echo "			Hora"; echo "   		</td>";
	echo "</tr>";
	
 	while($eventos = mysqli_fetch_assoc($dados))
	{
        list($ano, $mes, $dia) = explode("-", $eventos['Data']); // para ajustar a ordem da data
    	echo "<tr>";
    	echo "   <td align=center>";
    		echo $eventos['Evento'];
    		echo "   </td>";
    		echo "<td align=center>";
    		//echo  $eventos['Data'];
            echo $dia; echo "-"; echo $mes ;echo "-"; echo $ano;
    		echo "</td>";
    		echo "<td align=center>";
    		echo  $eventos['Hora'];
    		echo "</td>";
    	    	
    	echo "</tr>";
    	
	}
	echo "</table>"; 
 	
    
?>
 
  

  	
