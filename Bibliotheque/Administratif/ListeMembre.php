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
            $query = "SELECT * FROM Personne";
            
            if (!empty($recherche)) {
                $query = $query . " WHERE Nom LIKE '%$recherche%' OR Prenom LIKE '%$recherche%' OR Nom_utilisateur LIKE '%$recherche%' OR Date_naissance LIKE '%$recherche%' OR Email LIKE '%$recherche%' OR Telephone LIKE '%$recherche%' OR Adresse LIKE '%$recherche%' OR Statut LIKE '%$recherche%'";
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
        		<h2>Liste des personnes adherents</h2>
				<div>
					<div>
						<form action='ListeMembre.php' method='get'> 
						<input type="search" name="recherche">
						<input type="submit">
						</form>			
					</div>
					<div>
					<a href='PageAjoutMembre.php'> <button class="boutton color_buttonBlack" > Ajouter </button> </a>
					</div>
				</div>
			</div>
        <div class="container_add">
            <div class="centered_content">
                <div class="container">
                    <table>
                        <tr>
                            <th>Id</th>
                            <th>Nom</th>
                            <th>Prenom</th>
                            <th>Nom utilisateur</th>
                            <th>Date de naissance</th>
                            <th>Email</th>
                            <th>Telephone</th>
                            <th>Adresse</th>
                            <th>Profession</th>
                        </tr>

                        <?php

                            while($line = mysqli_fetch_assoc($infos)){
                                $id = $line['id_personne'];
                                $nom = $line['Nom'];
                                $prenom = $line['Prenom'];
                                $nomUtilisateur = $line['Nom_utilisateur'];
                                $dateNaissance = $line['Date_naissance'];
                                $mail = $line['Email'];
                                $tel = $line['Telephone'];
                                $adresse = $line['Adresse'];
                                $statut = $line['Statut'];

                                echo "<tr>
                                            <td> $id </td>
                                            <td> $nom  </td>
                                            <td> $prenom </td>
                                            <td> $nomUtilisateur </td>
                                            <td> $dateNaissance </td>
                                            <td> $mail </td>
                                            <td> $tel </td>
                                            <td> $adresse </td>
                                            <td> $statut </td>

                                     </tr>
                                ";
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