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
            $query = "SELECT * FROM Emprunts";
            
            if (!empty($recherche)) {
                $query = $query . "WHERE id_utilisateur LIKE '%$recherche%' OR id_Emprunt LIKE '%$recherche%' OR Date_emprunt LIKE '%$recherche%' OR Date_retour LIKE '%$recherche%' ";
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
        		<h2>Liste des emprunts</h2>
				<div>
					<div>
						<form action='ListeEmprunt.php' method='get'> 
                            <input type="search" name="recherche">
                            <input type="submit">
						</form>			
					</div>
					<div>
					<a href='PageAjoutEmprunt.php'> <button class="boutton color_buttonBlack" > Ajouter </button> </a>
					</div>
				</div>
			</div>

            <?php
                if (isset($_SESSION['message']) && !empty($_SESSION['message'])) {
                    echo '<div class="alert alert-info">' . $_SESSION['message'] . '</div>';
                    unset($_SESSION['message']);
                }
            ?>
        <div class="container_add">
            <div class="centered_content">
                <div class="container">
                    <table>
                        <tr>
                            <th>Id emprunt</th>
                            <th>Id adherent</th>
                            <th>Id livre</th>
                            <th>Date d'emprunt</th>
                            <th>Date de retour</th>
                        </tr>

                        <?php

                            while($line = mysqli_fetch_assoc($infos)){
                                $id = $line['id_emprunts'];
                                $id_e = $line['id_utilisateur'];
                                $id_a = $line['id_livre'];
                                $emprunt = $line['Date_emprunt'];
                                $retour = $line['Date_retour'];

                                echo "<tr>
                                            <td> $id </td>
                                            <td> $id_e  </td>
                                            <td> $id_a </td>
                                            <td> $emprunt </td>
                                            <td> $retour </td>
                                            
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
