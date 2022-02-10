<?php
//Conexão com o banco de dados

    $mysqli = new mysqli('localhost', 'leandro', '', 'test');
	if ($mysqli->connect_error) {
       die('Erro de conexão!!!!: (' . $mysqli->connect_errno . ')');
    }


?>