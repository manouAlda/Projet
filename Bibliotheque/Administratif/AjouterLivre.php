<?php

    $servername = "localhost"; // Nom du serveur
    $username = "mit"; // Nom d'utilisateur
    $password = "123456"; // Mot de passe
    $database = "bibliotheque";  //nom bd 

    $connection = new mysqli($servername, $username, $password, $database);
    if($connection->connect_error){
        die("TSY NETY");
    }
    else {
        $titre = $_POST["titre"];
        $auteur = $_POST["auteur"];
        $anneePublication = intval($_POST["anneePublication"]);
        $categories = $_Post["categories"];
        $quantites = $_POST["quantites"];

        mysqli_query($connection, "INSERT INTO Livre (Titre, Auteur, Annee_publication, Categorie, Quantite) VALUES ('$titre', '$auteur', $anneePublication, '$categories', '$quantites')");
        

        header("Location: ChargerLivre.php");
    }
?>