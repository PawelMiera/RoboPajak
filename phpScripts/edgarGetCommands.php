<?php


	if(isset($_POST['pass'])&&isset($_POST['lastId'])&&$_POST['pass']=="pajakkomenda")
	{
	session_start();		
	require_once "connect.php";
	$polaczenie = @new mysqli($host,$db_user,$db_password,$db_name,$db_port);
	
	if($polaczenie->connect_errno!=0)
	{
	echo "polaczenie sql nie ok".$polaczenie->connect_errno;
	}
	else
	{
		
		$sekundy=date("H")*3600+60*date("i")+date("s");
		$_SESSION['s_id']=session_id();	
		$_SESSION['email']="edgar@pajak.com";
		$lastId = $_POST['lastId'];
		$polaczenie->query(sprintf("UPDATE `users` SET `sesja`='%s', `last_logged`=$sekundy,`last_indeks`=$lastId WHERE email='%s'",mysqli_real_escape_string($polaczenie,$_SESSION['s_id']),
		mysqli_real_escape_string($polaczenie,$_SESSION['email'])));
		
		$rezultat=$polaczenie->query("SELECT * FROM komendy_Edgar ORDER BY id DESC LIMIT 1");
		if($rezultat)
			{			

			$wyniki=$rezultat->fetch_assoc();
			
			$_SESSION['lastId']=$wyniki['id'];			

			if($_SESSION['lastId']!=$lastId)
				{
				$_SESSION['komenda']=$wyniki['komenda'];
				$_SESSION['lastId']=$wyniki['id'];

				echo "k".$_SESSION['lastId']."@".$_SESSION['komenda']."%";
				}
			else
				{
				echo "ok";
				}	
				$rezultat->close();
			}			
		else
			{
				echo "tabela sql nie ok";
			}

		$polaczenie->close();
	}	
	session_destroy();
	}
	else
	{
		echo "brak danych nie ok";
	}
?>
