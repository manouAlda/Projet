<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<title>Bibliotheque-Ajout</title>
<link rel="stylesheet" href="../css/Biblio.css">
<link rel="stylesheet" href="../css/Register.css"> 
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

	<center><h1>Ajout</h1></center>

	<div class="container_add">
        <div class="centered_content">
            <div class="container">

				<form action="AjouterLivre.php" method="post">
					<div class="input_box">
						<label for="titre">Titre:</label>
				    	<input type="text" id="titre" name="titre" required>
					</div>
					<div class="input_box">
						<label for="auteur">Auteur:</label>
				    	<input type="text" id="auteur" name="auteur" required>
					</div>
					<div class="input_box">
						<label for="anneePublication">Annee de Publication:</label>
				    	<input type="number" id="anneePublication" name="anneePublication" required>
					</div>
					<div class="input_box">
						<label for="categories">Categories:</label>
				    	<input type="text" id="categories" name="categories" required>
					</div>
					<div class="input_box">
						<label for="quantites">Quantites:</label>
				    	<input type="number" id="quantites" name="quantites" required>
					</div>
				    
				    <div class="input_box">
				    	<input type="submit" value="Ajouter Livre">
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