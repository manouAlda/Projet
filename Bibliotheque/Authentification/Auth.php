<?php

    session_start();

    $nomUtilisateur = $_POST["nomUtilisateur"];
    $mdp = $_POST["motDePasse"];

    $servername = "localhost";
    $username = "mit";
    $password = "123456";
    $database = "bibliotheque";

    $connection = new mysqli($servername, $username, $password, $database);
    if($connection->connect_error){
        die("TSY NETY");
    }
    else {
        $stmt = $connection->prepare("SELECT * FROM Utilisateur WHERE Nom_utilisateur = ?");
        $stmt->bind_param("s", $nomUtilisateur);
        $stmt->execute();
        $result = $stmt->get_result();

        if ($result->num_rows == 0) {
            header("Location: Login.php?erreur=utilisateurNonTrouve");
            exit();
        } else {
            $data = $result->fetch_assoc();

            if ($data["Mot_de_passe"] == $mdp) {
                $_SESSION['utilisateur'] = $nomUtilisateur;
                header("Location: ../index.php");
                exit();
            } else {
                header("Location: Login.php?erreur=motDePasseIncorrect");
                exit();
            }
        }

        $connection->close();
    }

?>
