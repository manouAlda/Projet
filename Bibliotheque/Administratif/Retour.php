<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <title>Bibliotheque-Ajouter un retour</title>
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

	<center><h1>Retour de livre</h1></center>
	<div class="container_add">
        <div class="centered_content">
            <div class="container">

				<form action="RetournerLivre.php" method="post">
                <div class="input_box">
						<label for="id_utilisateur">ID Emprunt:</label>
        				<input type="number" name="id_emprunt" required>
					</div>
					<div class="input_box">
						<label for="id_utilisateur">ID Utilisateur:</label>
        				<input type="number" id="id_utilisateur" name="id_utilisateur" required>
					</div>
					<div class="input_box">
						<label for="id_livre">ID Livre:</label>
      					  <input type="number" id="id_livre" name="id_livre" required>
					</div>
					
				    
				    <div class="input_box">
				    	<input type="submit" value="Retourner le livre">
				    </div>
				</form>

            </div>
        </div>
        
    </main>
    
    <footer>
        <p>&copy; 2024 Bibliotheque</p>
    </footer>
</body>
</html>
