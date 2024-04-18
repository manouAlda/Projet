<?php
session_start();

$servername = "localhost";
$username = "mit";
$password = "123456";
$database = "bibliotheque";

$connection = new mysqli($servername, $username, $password, $database);
if($connection->connect_error){
    die("TSY NETY");
}
else {
    $nom = $_POST["nom"];
    $prenom = $_POST["prenom"];
    $naissance = $_POST["dateNaissance"];
    $mail = $_POST["mail"];
    $tel = $_POST["tel"];
    $adresse = $_POST["adresse"];
    $nomUtilisateur = $_POST["nomUtilisateur"];
    $statut = $_POST["statut"];
    $mdp = $_POST["motDePasse"];
    $confirmMdp = $_POST["confirmationMotDePasse"];

    if ($mdp !== $confirmMdp) {
        header("Location: Register.php?erreur=motsDePasseNonIdentiques");
        exit;
    } else {
        
        $stmt = $connection->prepare("SELECT * FROM Personne WHERE Nom = ? and Nom_utilisateur = ? and Email = ?");
        $stmt->bind_param("sss", $nom, $nomUtilisateur, $mail);
        $stmt->execute();
        $result = $stmt->get_result();

        if ($result->num_rows > 0) {
            header("Location: Register.php?erreur=utilisateurExistant");
            exit;
        } else {
            $stmt = $connection->prepare("INSERT INTO Personne (Nom, Prenom, Nom_utilisateur, Date_naissance, Email, Telephone, Adresse, Statut) VALUES (?, ?, ?, ?, ?, ?, ?, ?)");
            $stmt->bind_param("ssssssss", $nom, $prenom, $nomUtilisateur, $naissance, $mail, $tel, $adresse, $statut);
            $stmt->execute();

            $stmt = $connection->prepare("INSERT INTO Utilisateur (Nom_utilisateur, Mot_de_passe) VALUES (?, ?)");
            $stmt->bind_param("ss", $nomUtilisateur, $mdp);
            $stmt->execute();

            $_SESSION['utilisateur'] = $nomUtilisateur;
            header("Location: ../index.php");
            exit;
        }
    }

    $connection->close();
}
?>
