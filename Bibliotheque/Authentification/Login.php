<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<title>Login</title>
<link rel="stylesheet" href="../css/Register.css">
</head>
<body>

	<center><h1>Login</h1></center>


    <?php
         $erreur = isset($_GET["erreur"])? $_GET["erreur"] : null;

         if(!empty($erreur)){
             if($erreur=="utilisateurNonTrouve")	echo "<p style=\"color: red;\">Nom d'utilisateur incorrect.</p>";
             else if($erreur=="motDePasseIncorrect")	echo "<p style=\"color: red;\">Mot de passe incorrect.</p>";	
         }
    ?>

	<div class="container_add">
        <div class="centered_content">
            <div class="container">

				<form action="Auth.php" method="post">
					<div class="input_box">
						<label>Nom d'utilisateur: </label>
						<input type="text" name="nomUtilisateur" placeholder="Nom d'utilisateur" required>
					</div>
				    <div class="input_box">
				    	<label>Mot de passe: </label>
				    	<input type="password" name="motDePasse" placeholder="Mot de passe" required>
				    </div>
				    
				    
				    <p>Si vous n'avez pas encore de compte. <a href='Register.php'>Inscrivez-vous ! </a></p>
				
				
						<center >
                            <input type="submit" value="Submit" class="boutton color_buttonBlack" >
                        </center>
                    
                </form>
            </div>
        </div>
    
	
	
</body>
</html>