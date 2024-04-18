<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <title>Bibliotheque-Ajouter un membre</title>
    <link rel="stylesheet" href="../css/Biblio.css">
	<link rel="stylesheet" href="../css/Register.css">
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

	<center><h1>Ajout de membre</h1></center>
	<div class="container_add">
        <div class="centered_content">
            <div class="container">

				<form action="../Authentification/Inscription.php" method="post">
						<div class="input_box">
	    					<input type="text" name="nom" placeholder="Nom" required>
	    				</div>
	    				<div class="input_box">
	    					<input type="text" name="prenom" placeholder="Prenom" required>
	    				</div>
	    				<div class="input_box">
					    <input type="date" name="dateNaissance" placeholder="Naissance" required>
					    </div>
					    <div class="input_box">
					    <input type="email" name="mail" placeholder="Email" required>
					    </div>
					    <div class="input_box">
					    <input type="tel" name="tel" placeholder="Telephone" required>
					    </div>
					    <div class="input_box">
					    <input type="text" name="adresse" placeholder="Adresse" required>
					    </div>
					    <div class="input_box">
					    <input type="text" name="nomUtilisateur" placeholder="Nom d'utilisateur" required>
					    </div>
					    <div class="box">
					    
						    <input type="radio" name="statut" value="Administrateur" required>Administrateur
						    <input type="radio" name="statut" value="Etudiant" required>Etudiant(e)
						    <input type="radio" name="statut" value="Personne" required>Personne
	    
					    </div>
					    <div class="input_box">
					    <input type="password" name="motDePasse" placeholder="Mot de passe" required>
					    </div>
					    <div class="input_box">
					    <input type="password" name="confirmationMotDePasse" placeholder="Confirmer le mot de passe" required>
					    </div>
	   
						<center >
                            <input type="submit" value="Submit" class="boutton color_buttonBlack" >
                        </center>
				</form>

            </div>
        </div>
        
    </main>
    
    <footer>
        <p>&copy; 2024 Bibliotheque</p>
    </footer>
</body>
</html>
