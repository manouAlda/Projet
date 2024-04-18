CREATE TABLE Personne (
	id_personne INT AUTO_INCREMENT PRIMARY KEY,
	Nom VARCHAR(200) NOT NULL,
	Prenom VARCHAR(200),
	Nom_utilisateur VARCHAR(200) UNIQUE NOT NULL,
	Date_naissance DATE,
	Email VARCHAR(100) UNIQUE,
	Telephone VARCHAR(20) UNIQUE,
	Adresse VARCHAR(100) NOT NULL,
	Statut VARCHAR(50) NOT NULL
);

CREATE TABLE Utilisateur (
	id_utilisateur INT AUTO_INCREMENT PRIMARY KEY,
	Nom_utilisateur VARCHAR(200) UNIQUE NOT NULL,
	Mot_de_passe VARCHAR(50) NOT NULL,
	Statut VARCHAR(20),
	
	CONSTRAINT fk_utilisateur
		FOREIGN KEY (Nom_utilisateur) REFERENCES Personne(Nom_utilisateur)
);

CREATE TABLE Livre (
	id_livre INT AUTO_INCREMENT PRIMARY KEY,
	Titre VARCHAR(100) NOT NULL,
	Auteur VARCHAR(100) NOT NULL,
	Annee_publication INT,
	Categorie VARCHAR(50),
	Quantite INT
);

INSERT INTO Livre (Titre,Auteur,Annee_publication,Categorie,Quantite) Values ('Le Petit Prince', 'Antoine de Saint-Exupéry', 1943, 'Litterature', 10), ('1984', 'George Orwell', 1949, 'Science-fiction', 15), ('Les Miserables', 'Victor Hugo', 1862, 'Histoire', 20), ('La Selection', 'Kiera Cass', 2018, 'Romance', 5), ('Mitaraina ny tany', 'Andry Andraina', 1979, 'Manuel Scolaire', 12);

CREATE TABLE Emprunts (
	id_emprunts INT AUTO_INCREMENT PRIMARY KEY,
	id_utilisateur INT,
	id_livre INT,
	Date_emprunt DATE,
	Date_retour DATE,

	FOREIGN KEY (id_utilisateur) REFERENCES Utilisateur(id_utilisateur),
	FOREIGN KEY (id_livre) REFERENCES Livre(id_livre)
);

