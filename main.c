#include <stdio.h>
#include <stdlib.h>
#include "fonction_crypteur.h"
#include <string.h>


int main(int argc, char** argv)
{
	int i,j,espacement = 1, parano=0;
	char* nom_du_fichier_a_ouvrir = NULL;
    char* nom_du_fichier_a_importer = NULL;
    char* mot_de_passe_entre = NULL;
    char* nom_fichier_sortie = NULL;
    FILE* image_entree = NULL;
    FILE* texte_a_crypter = NULL;
    nom_fichier_sortie = malloc(sizeof(char) * 50);
    strcat(nom_fichier_sortie,"fichier_cryptee.bmp");
    if(argc == 1){
        printf("Erreur d'argument. Utilisation : ./crypteur -crypt;-decrypt(-c,-d) image.bmp -fichier(-f) source [-sortie,(-s)] sortie.bmp [-mdp,-m] motdepasse [-e] n [-p]\n");
        exit(EXIT_FAILURE);
    }
        for(i=1; i <argc; ++i){
            if(i+1 < argc)
            {
                if( (strcmp(argv[i],"-crypt") == 0 || strcmp(argv[i],"-decrypt") == 0 || strcmp(argv[i],"-c") == 0 || strcmp(argv[i],"-d") == 0) && nom_du_fichier_a_ouvrir == NULL)
                {
                    nom_du_fichier_a_ouvrir = malloc(sizeof(char) * strlen(argv[i+1]));
                    nom_du_fichier_a_ouvrir = argv[i+1];
                    image_entree = fopen(nom_du_fichier_a_ouvrir, "r+");
                    i++;
                }
                else if( (strcmp(argv[i],"-fichier") == 0 || strcmp(argv[i],"-f") == 0) && nom_du_fichier_a_importer == NULL)
                {
                    nom_du_fichier_a_importer = malloc(sizeof(char) * strlen(argv[i+1]));
                    for(j=0;j<strlen(argv[i+1]); ++j){
                    nom_du_fichier_a_importer[j] = argv[i+1][j];
                    }
                    texte_a_crypter = fopen(nom_du_fichier_a_importer,"r");
                    i++;
                }
                else if(strcmp(argv[i],"-e") == 0 || strcmp(argv[i],"-espace") == 0)
                {

                    int espacement_utilisateur = atoi(argv[i+1]);
                    if(espacement_utilisateur > 0) espacement = espacement_utilisateur;
                    else{
                        printf("Erreur d'argument. Utilisation : ./crypteur -crypt;-decrypt(-c,-d) image.bmp -fichier(-f) source [-sortie,(-s)] sortie.bmp [-mdp,-m] motdepasse [-e] n [-p]\n");
                        exit(EXIT_FAILURE);
                    }
                    i++;
                    
                }
                else if( (strcmp(argv[i],"-mdp") == 0 || strcmp(argv[i],"-m") == 0) && mot_de_passe_entre == NULL)
                {
                    mot_de_passe_entre = malloc(sizeof(char) * strlen(argv[i+1]));
                    mot_de_passe_entre = argv[i+1];
                    i++;
                    if(strlen(mot_de_passe_entre) > 12){
                        printf("Le mot de passe ne peut pas contenir plus de 12 caracteres !\n");
                        exit(EXIT_FAILURE);
                    }
                }
                else if( (strcmp(argv[i],"-sortie") == 0 || strcmp(argv[i],"-s") == 0) && strcmp(nom_fichier_sortie,"fichier_cryptee.bmp") == 0 ){
                {
                    nom_fichier_sortie = malloc(sizeof(char) * strlen(argv[i+1]));
                    nom_fichier_sortie = argv[i+1];
                    i++;
                }
                }else{
                    printf("Erreur d'argument. Utilisation : ./crypteur -crypt;-decrypt(-c,-d) image.bmp -fichier(-f) source [-sortie,(-s)] sortie.bmp [-mdp,-m] motdepasse [-e] n [-p]\n");
                    exit(EXIT_FAILURE);
                }
            }
            else if(strcmp(argv[i],"-parano") == 0 || strcmp(argv[i],"-p") == 0 )
            {
                parano = 1;
            }
            else if(strcmp(argv[1],"-gui") != 0){
                printf("Erreur d'argument. Utilisation : ./crypteur -crypt;-decrypt(-c,-d) image.bmp -fichier(-f) source [-sortie,(-s)] sortie.bmp [-mdp,-m] motdepasse [-e] n [-p]\n");
                exit(EXIT_FAILURE);
            }

        }

        if(strcmp(argv[1],"-crypt") == 0 ||strcmp(argv[1],"-c") == 0)
        {
            //Si l'utilisateur veut crypter, on crypte.
            if(parano)
                crypter_parano(image_entree, texte_a_crypter, nom_du_fichier_a_ouvrir,nom_fichier_sortie);
            else
                crypter(image_entree, texte_a_crypter , mot_de_passe_entre,nom_du_fichier_a_ouvrir,espacement,nom_fichier_sortie);
        }


        else if(strcmp(argv[1],"-decrypt") == 0 ||strcmp(argv[1],"-d") == 0)
        {
            //Si l'utilisateur veut decrypter, on decrypte.
            if(parano)
                afficher_message_parano(image_entree);
            else
                decrypter(image_entree,mot_de_passe_entre,espacement);
        }

        else if(strcmp(argv[1],"-gui") == 0) 
        {
            //Si l'utilisateur veut lancer l'interface graphique, on lance l'interface graphique.
            lancer_interface_graphique();
        }

        else printf("Erreur d'argument. Utilisation : ./crypteur -crypt;-decrypt(-c,-d) image.bmp -fichier(-f) source [-sortie,(-s)] sortie.bmp [-mdp,-m] motdepasse [-e] n [-p]\n");    

}
