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
        $id_utilisateur = $_POST['id_utilisateur'];
        $id_livre = $_POST['id_livre'];

        // Verifier le statut de l'utilisateur
        $query = "SELECT Statut FROM Utilisateur WHERE id_utilisateur = ?";
        $statement = $connection->prepare($query);
        $statement->bind_param("i", $id_utilisateur);
        $statement->execute();
        $result = $statement->get_result();
        $row = $result->fetch_assoc();

        if ($row['Statut'] === 'suspendu') {
            $_SESSION['message'] = "Vous ne pouvez pas emprunter de livres car votre compte est suspendu.";
            header("Location: ListeEmprunt.php");
            exit();
        }

        $query = "SELECT Quantite FROM Livre WHERE id_livre = ?";
        $statement = $connection->prepare($query);
        $statement->bind_param("i", $id_livre);
        $statement->execute();
        $result = $statement->get_result();
        $row = $result->fetch_assoc();

        if ($row['Quantite'] > 0) {

            $query = "UPDATE Livre SET Quantite = Quantite - 1 WHERE id_livre = ?";
            $statement = $connection->prepare($query);
            $statement->bind_param("i", $id_livre);
            $statement->execute();

            $query = "INSERT INTO Emprunts (id_livre, id_utilisateur, Date_emprunt, Date_retour) VALUES (?, ?, NOW(), DATE_ADD(NOW(), INTERVAL 2 WEEK))";
            $statement = $connection->prepare($query);
            $statement->bind_param("ii", $id_livre, $id_utilisateur);
            $statement->execute();

            //echo "Livre emprunte avec succes.";
            $_SESSION['message'] = "Livre emprunte avec succes.";
        } else {
            //echo "Le livre n'est pas disponible.";
            $_SESSION['message'] = "Le livre n'est pas disponible.";
        }

        $connection->close();
    }

    header("Location: ListeEmprunt.php");
    
?>
