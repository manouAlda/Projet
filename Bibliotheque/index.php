<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<title>Bibliotheque - Accueil</title>
<link rel="stylesheet" href="css/Biblio.css">
</head>
<body>

	<?php
		session_start();

		$utilisateur = isset($_SESSION['utilisateur'])? $_SESSION['utilisateur'] : null;

		if(empty($utilisateur)){
			header("Location: Authentification/Login.php");
			exit();
		}
	?>
	
	<header>
        <h1>Bibliotheque</h1>
        <nav>
            <ul class="menu">
                <li><a href="index.php">Accueil</a></li>
                <li><a href="Administratif/ChargerLivre.php">Livres</a></li>
                <li><a href="Administratif/Emprunt.php">Emprunts</a></li>
                <li><a href="Authentification/Register.php">Inscription</a></li>
                <li><a href="Authentification/Login.php">Connexion</a></li>
				<li><a href="Authentification/Deconnexion.php">Deconnexion</a></li>
            </ul>
        </nav>
    </header>
    
    <main>
    	<h2>Bienvenue   	</h2>
	    	
	    
        <div class="container_function">
	        <div class=text_center>
	            <h2 class="text_title"> Fonctionnalites </h2>
	        </div>
	        <ul class="container_list">
	            <li class="list">
	                <div>
	                    <strong>Check me !</strong><br><br>
	                    <a href="Administratif/ListeMembre.php">Liste des adherants</a>
	                    <p>Vous pouvez voir les listes de tous les adherants consultant cette bibliotheque !</p>
	                    <a href="Administratif/ListeMembre.php" class="boutton color_buttonBlack">Pick list</a>
	                </div>
	            </li>
	            <li class="list">
	                <div>
	                    <strong>Check me !</strong><br><br>
	                    <a href="Administratif/ChargerLivre.php">Liste des livres</a>
	                    <p>Consulter tous les livres avec leurs categories et stock !</p>
	                    <a href="Administratif/ChargerLivre.php" class="boutton color_buttonBlack">Pick list</a>
	                </div>
	            </li>
	            <li class="list">
	                <div>
	                    <strong>Check me !</strong><br><br>
	                    <a href="Administratif/Emprunt.php">Emprunts</a>
	                    <p>Listes des emprunts de livres </p>
	                    <a href="Administratif/Emprunt.php" class="boutton color_buttonBlack">Pick list</a>
	                </div>
	            </li>
				
	        </ul>
	    </div>
    
    </main>
    
    <footer>
        <p>&copy; 2024 Bibliotheque</p>
    </footer>

</body>
</html>