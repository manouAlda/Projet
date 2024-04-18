<?php
	session_start();
	if (!isset($_SESSION['utilisateur'])) {
        header("Location: ../Authentification/Login.php");
        exit();
    }
?>

<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<title>Bibliotheque-Emprunt</title>
<link rel="stylesheet" href="../css/Emprunt.css">
<link rel="stylesheet" href="../css/Biblio.css">
<link rel="stylesheet" href="../css/Livre.css">
</head>
<body>

	<header>
        <h1>Bibliotheque</h1>
        <nav>
        <ul class="menu">
                <li><a href="../index.php">Accueil</a></li>
                <li><a href="ChargerLivre.php">Livres</a></li>
                <li><a href="Emprunt.php">Emprunts</a></li>
                <li><a href="../Authentification/Register.php">Inscription</a></li>
                <li><a href="../Authentification/Login.php">Connexion</a></li>
				<li><a href="../Authentification/Deconnexion.php">Deconnexion</a></li>
            </ul>
        </nav>
    </header>
    
    <main>

	<center><h1>Emprunts</h1></center>

	<div class="container_function">
	        <ul class="container_list">
	            <li class="list">
	                <div>
	                    <strong>Check me !</strong><br><br>
	                    <a href="PageAjoutEmprunt.php">Ajouter un emprunt</a>
	                    <p>Ajouter un emprunt de livre !</p>
	                    <a href="PageAjoutEmprunt.php" class="boutton color_buttonBlack">Ajouter</a>
	                </div>
	            </li>
	            <li class="list">
	                <div>
	                    <strong>Check me !</strong><br><br>
	                    <a href="ListeEmprunt.php">Liste des emprunts</a>
	                    <p>Consulter tous les emprunts !</p>
	                    <a href="ListeEmprunt.php" class="boutton color_buttonBlack">Lister</a>
	                </div>
	            </li>
	            <li class="list">
	                <div>
	                    <strong>Check me !</strong><br><br>
	                    <a href="Retour.php">Remise</a>
	                    <p>Formulaire de retour de livre !</p>
	                    <a href="Retour.php" class="boutton color_buttonBlack">Remise</a>
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