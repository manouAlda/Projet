<?php
    session_start();
    $utilisateur = isset($_SESSION['utilisateur'])? $_SESSION['utilisateur'] : null;

		if(empty($utilisateur)){
			header("Location: ../Authentification/Login.php");
		}

        $servername = "localhost"; // Nom du serveur
        $username = "mit"; // Nom d'utilisateur
        $password = "123456"; // Mot de passe
        $database = "bibliotheque";  //nom bd 
    
        $connection = new mysqli($servername, $username, $password, $database);
        if($connection->connect_error){
            die("TSY NETY");
        }
        else {
            
            $recherche = isset($_GET["recherche"])? $_GET["recherche"] : null;
            $query = "SELECT * FROM Livre";

            if (!empty($recherche)) {
                
                $query .= " WHERE Titre LIKE '%$recherche%' OR Auteur LIKE '%$recherche%' OR Categorie LIKE '%$recherche%' OR Quantite LIKE '%$recherche%' OR Annee_publication LIKE '%$recherche%'";
            }

            $infos = $connection->query($query);

        }
?>

<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <title>Bibliotheque - Liste des personnes</title>
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
			<div class="listeL">
        		<h2>Liste des livres</h2>
				<div>
					<div>
						<form action='ChargerLivre.php' method='get'> 
                            <input type="search" name="recherche">
                            <input type="submit">
						</form>			
					</div>
					<div>
					<a href='PageAjoutLivre.php'> <button class="boutton color_buttonBlack" > Ajouter </button> </a>
					</div>
				</div>
			</div>
        <div class="container_add">
            <div class="centered_content">
                <div class="container">
                    <table>
                    <tr>
                        <th>ID</th>
                        <th>Titre</th>
                        <th>Auteur</th>
                        <th>Annee de Publication</th>
                        <th>Categories</th>
                        <th>Quantites</th>
                    </tr>

                        <?php

                            while($line = mysqli_fetch_assoc($infos)){
                                $id = $line['id_livre'];
                                $titre = $line['Titre'];
                                $auteur = $line['Auteur'];
                                $anneePublication = $line['Annee_publication'];
                                $categories = $line['Categorie'];
                                $quantites = $line['Quantite'];

                                echo "<tr>
                                    <td> $id</td>
                                    <td> $titre </td>
                                    <td> $auteur </td>
                                   <td> $anneePublication </td>
                                    <td> $categories </td>
                                    <td> $quantites </td>
                                </tr>";
                                
                            }

                        ?>

                  </table>
                </div>
            </div>
        </div>
    </main>
    <footer>
        <p>&copy; 2024 Bibliotheque</p>
    </footer>
</body>
</html>