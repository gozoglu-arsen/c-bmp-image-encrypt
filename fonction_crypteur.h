#include "fonction_crypteur.c"

//Retourne un décimal, transforme un tableau de 8 bit en decimal
int binaire_vers_decimal( int* tab, int taille);
//convertie n en binaire et le met dans tab
void decimal_vers_binaire( int* tab, int n);
/*
	assemble n octet.
	Supposons que nous avons l'octet 10101011 et 11000000
	leur assemblage sera 1010101111000000.
	cette focntion fait cette assemblage et retourne la valeur décimale.
*/
int assembler_quatre_octet(int **a, int nombre_octet);

/* Permet d'obtenir des informations sur le fichier bitmap
par exemple nous savons que la taille du fichier en octet se trouve au 2 eme octet et est codé sur 4 bits.
on fait donc taille_fichier_octet = obtenir_information(image_entree,2,4);
*/
int obtenir_information(FILE* fichier, int position_a_partir_de_zero, int nombre_octet);
// Code le caractere information dans le fichier BMP (il utilise 8 octets)
void coder_caractere(FILE* fichier, int position_a_partir_de_zero, int information);
// DeCode le caractere information dans le fichier BMP sur 8 octets
int decoder_information(FILE* fichier, int position_a_partir_de_zero);
/*
	Vérifie de façon sommaire si le fichier est BMP.
*/
int verifier_si_fichier_est_bmp(FILE* fichier);
// Code un message le parametre fin indique si oui ou non il faut coder '\0' à la fin
void coder_message(FILE* fichier, int debut, int longueur, char* message,int espacement,int fin);
// renvoie la taille de la chaine jusqu'au '\0'
int longueur_du_message(FILE* fichier, int debut, int taille_fichier_octet);
// Renvoie la taille du fichier txt
int taille_fichier(FILE* src);
// Copie fichier (commande cp LINUX)
void copier_fichier(char* nom,char* sortie);
// Récupere du texte dans le fichier du début eme octet jusqu'a fin. Met cette chaine dans tab
void recuperer_texte_dans_le_fichier(FILE* fichier,char* tab,int debut, int fin);
// Renvoie la longueur du mot de passe. 0 si il n'y en a pas.
int longueur_du_mot_de_passe(FILE* fichier, int debut, int taille_fichier_octet);
//MEt le fichier txt dans un tableau
char* mettre_fichier_dans_tableau(FILE* fichier);
// Verifie si le fichier txt contient uniquement des caracteres ascii
int est_ascii(FILE* fichier);
// verifie qu'il y a la signature "projet_l1_2020     "
int verifier_signature(FILE* fichier, int debut);
//Renvoie le mot de passe contenu dans le fichier bmp. NULL si pas de mdp
char* recuperer_mot_de_passe(FILE* fichier, int debut, int taille_fichier_octet);
// Affiche le texte décodé 
void afficher_texte_decode(FILE* image_entree,int espacement);
// Decryptage simple
void decrypter(FILE* image_sortie, char* mot_de_passe_entre, int espacement);
//cryptage simple
void crypter(FILE* image_entree, FILE* texte_a_crypter ,char* mot_de_passe_entre, char* nom_du_fichier_a_ouvrir, int espacement,char* nom_fichier_sortie);
// met en place l'onglet avec le texte txt comme consigne
void mettre_en_place_onglet(SDL_Renderer *renderer, char* txt);

void lancer_interface_graphique();
//Codage message parano
int coder_message_parano(FILE* fichier, int debut, char* message, int taille_fichier_octet);
// afficher message parano
void afficher_message_parano(FILE* image_entree);
// Code les mots du dictionnaire anglais dans l'image.
void coder_mot_dictionnaire(FILE* image_entree);
// Codage finalisé.
void crypter_parano(FILE* image_entree, FILE* texte_a_crypter, char* nom_du_fichier_a_ouvrir, char* nom_fichier_sortie);
