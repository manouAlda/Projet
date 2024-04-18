<?php
    session_start();

    if (!isset($_SESSION['utilisateur'])) {
        header("Location: ../Authentification/Login.php");
        exit();
    }

    $servername = "localhost";
    $username = "mit";
    $password = "123456";
    $database = "bibliotheque";

    $connection = new mysqli($servername, $username, $password, $database);
    if ($connection->connect_error) {
        die("TSY NETY");
    }
    else {
        if (!isset($_POST['id_emprunt'])) {
            $_SESSION["message"] = "ID de l'emprunt manquant.";
            header("Location: ListeEmprunt.php");
            exit();
        }
        $id_emprunt = $_POST['id_emprunt'];
        $date_retour_reelle = date("Y-m-d");

        $query = "SELECT * FROM Emprunts WHERE id_emprunts = ?";
        
        $statement = $connection->prepare($query);
        
        $statement->bind_param("i", $id_emprunt);
        $statement->execute();
        $result = $statement->get_result();

        if ($result->num_rows == 0) {
            $_SESSION["message"] = "L'emprunt n'existe pas.";
        } else {

            $data = $result->fetch_assoc();
            $id_livre = $data['id_livre'];
            $id_utilisateur = $data['id_utilisateur'];
            $date_retour_prevue = $data['Date_retour'];
    
            $query = "UPDATE Emprunts SET Date_retour = ? WHERE id_emprunts = ?";
            $statement = $connection->prepare($query);
            $statement->bind_param("si", $date_retour_reelle, $id_emprunt);
            $statement->execute();

            $query = "UPDATE Livre SET Quantite = Quantite + 1 WHERE id_livre = ?";
            $statement = $connection->prepare($query);
            $statement->bind_param("i", $id_livre);
            $statement->execute();
    
            if ($date_retour_reelle > $date_retour_prevue) {
                $diff = date_diff(date_create($date_retour_prevue), date_create($date_retour_reelle));
                $jours_de_retard = $diff->format("%a");
    
                $penalite = $jours_de_retard;

                $query = "UPDATE Utilisateur SET Statut = 'suspendu' WHERE id_utilisateur = ?";
                $statement = $connection->prepare($query);
                $statement->bind_param("i", $id_utilisateur);
                $statement->execute();
    
                $_SESSION["message"] = "Livre retourne tard. Penalite de $penalite jours appliquee.";
            
            } else {
                $_SESSION["message"] = "Livre retourne a temps.";
            }
            
        }

        $connection->close();

        header("Location: ListeEmprunt.php");
        exit();
    }


?>
