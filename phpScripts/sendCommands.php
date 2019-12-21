<?php
	session_start();
	
	$komenda=$_POST['name'];
			
	//$fileStatus=file_put_contents("datastorage.txt",$komenda,FILE_APPEND);
		
	if(isset($_POST['name'])&&isset($_SESSION['email'])&&$_SESSION['admin']==1)
	{
		
	require_once "connect.php";
	
	if($komenda=="speed"&&$_SESSION['predkosc']==true){
		$_SESSION['predkosc']=false;	
		exit();
	}
	elseif($komenda=="speed"&&$_SESSION['predkosc']==false){
		$_SESSION['predkosc']=true;		
		exit();
	}
	
	if($komenda=="s1"&&$_SESSION['predkosc']==true)
	{
		$komenda="ss";
	}
	if($komenda=="s2"&&$_SESSION['predkosc']==true)
	{
		$komenda="sb";
	}

	
		$polaczenie = @new mysqli($host,$db_user,$db_password,$db_name,$db_port);
	
		
		if($polaczenie->connect_errno!=0)
		{
		echo "polaczenie sql nie ok".$polaczenie->connect_errno;
		}
		else
		{	


			$rezultat=$polaczenie->query("SELECT * FROM komendy_Edgar ORDER BY id DESC LIMIT 1");
		
			if($rezultat)
			{			

			$wyniki=$rezultat->fetch_assoc();
			
			$indeks=$wyniki['id']+1;				
			
			if($indeks>100){
			$polaczenie->query("DELETE FROM komendy_Edgar WHERE 1=1");
			$polaczenie->query("ALTER TABLE komendy_Edgar AUTO_INCREMENT = 1");			
			}
			
			$rezultat->close();
			}			
			else
			{
				echo "tabela sql nie ok";
			}
			
						
			$polaczenie->query(sprintf("INSERT INTO `komendy_Edgar`(`komenda`) VALUES ('%s')",mysqli_real_escape_string($polaczenie,$komenda)));

			$polaczenie->close();
			
			
		}	
	}
	else
	{
		echo "brak danych nie ok";
	}
	
	
?>