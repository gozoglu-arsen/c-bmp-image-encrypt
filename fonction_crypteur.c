#include <math.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL_ttf.h>

int binaire_vers_decimal( int* tab, int taille)
{
    int i, nombre = 0, j = 0;
    for(i=taille-1, j = 0; i>=0; --i,j++)
    {
        if(tab[i] == 1)
        nombre += pow(2, j);
    }
    return nombre;
}

void decimal_vers_binaire( int* tab, int n)
{
    int i;
    for(i=0;i<8;++i) tab[i] = 0;
    for(i=7; n>0; i--, n /= 2)
    {
        tab[i] = n%2;
    }
}

int assembler_quatre_octet(int **a, int nombre_octet)
{
    int i,j,c=0;
    int* tmp = malloc(sizeof(int) * (nombre_octet*8));
    for(i=nombre_octet-1; i>=0; i--)
    {
        for(j=0; j<8; ++j)
        {
            tmp[c] = a[i][j];
            c++;
        }
    }
    int taille = 0;
    for(i=0; i<nombre_octet*8; ++i, taille++)
    {
        if(tmp[i] == 1) break;
    }
    int *tab = malloc(sizeof(int) * nombre_octet*8-taille);
    for(i=0; i<nombre_octet*8-taille; ++i)
    {
        tab[i] = tmp[i+taille];
    }
    return binaire_vers_decimal(tab, nombre_octet*8-taille);
}


int obtenir_information(FILE* fichier, int position_a_partir_de_zero, int nombre_octet)
{
    fseek(fichier, position_a_partir_de_zero, SEEK_SET);
    int **a = malloc(sizeof(int*) * nombre_octet);
    int caractere, i;
    for(i=0; i<nombre_octet; ++i){
        a[i] = malloc(sizeof(int) * 8);
        caractere = fgetc(fichier);
        decimal_vers_binaire(a[i], caractere);
    }
    return assembler_quatre_octet(a,nombre_octet);
}

void coder_caractere(FILE* fichier, int position_a_partir_de_zero, int information)
{
    // z = 1111010
    fseek(fichier, position_a_partir_de_zero, SEEK_SET);
    int i, caractere, information_en_binaire[8];
    decimal_vers_binaire(information_en_binaire,(int)information);
    for(i=0; i<8; ++i)
    {
        caractere = fgetc(fichier);
        if(information_en_binaire[i] == 1 && caractere%2 == 0)
        {
            fseek(fichier, -1, SEEK_CUR);
            caractere++;
            fputc(caractere,fichier);
        }
        else if(information_en_binaire[i] == 0 && caractere%2 == 1)
        {
            fseek(fichier, -1, SEEK_CUR);
            caractere--;
            fputc(caractere,fichier);
        }

    }
}

int decoder_information(FILE* fichier, int position_a_partir_de_zero)
{
    fseek(fichier, position_a_partir_de_zero, SEEK_SET);
    int information_en_binaire[8],i,caractere;
    for(i=0;i<8;++i)
    {
        caractere = fgetc(fichier);
        if(caractere%2 == 0) information_en_binaire[i] = 0;
        else information_en_binaire[i] = 1;
    }
    return binaire_vers_decimal(information_en_binaire,8);

}

int verifier_si_fichier_est_bmp(FILE* fichier)
{
    if(fichier != NULL){
  int code = obtenir_information(fichier,0,2);
  if(code == 19778 || code == 16706) return 1; // si le début du fichier est BA ou BM
  return 0;
    }
    return 0;
}

void coder_message(FILE* fichier, int debut, int longueur, char* message,int espacement,int fin)
{
    int i,j;
    for(i=debut, j=0; i< debut + longueur*(8*espacement); i+=(8*espacement), ++j)
    {
        coder_caractere(fichier, i, message[j]);
    }
    if(fin == 1)
    coder_caractere(fichier, debut+ longueur*(8*espacement), '\0');
}

int coder_message_parano(FILE* fichier, int debut, char* message, int taille_fichier_octet)
{
    int i=0,espacement = debut;
    while(message[i] != '\0'){

        coder_caractere(fichier,espacement,message[i]);
        espacement += ((message[i]+17)*3);
        i++;
    }
    coder_caractere(fichier,espacement,'\0');
    if(espacement >= taille_fichier_octet) return 0;
    return 1;

}

void coder_mot_dictionnaire(FILE* image_entree)
{
    FILE* dictionnaire = NULL;
    dictionnaire = fopen("Ressources/words.txt", "r");
    if(dictionnaire == NULL){ printf("Dictionnaire de mot manquant.\n");
    exit(EXIT_FAILURE);
    }
    else{
        int endroit = obtenir_information(image_entree,10,4), taille_fichier_octet = obtenir_information(image_entree,2,4);
        char c;
        while(endroit < taille_fichier_octet){
            c = fgetc(dictionnaire);
            coder_caractere(image_entree,endroit,c);
            endroit += 8;
        }
    }
    fclose(dictionnaire);
}




void afficher_message_parano(FILE* image_entree)
{
        if(image_entree != NULL){
        int  debut = obtenir_information(image_entree,10,4), i;
        i=debut; 
        while(decoder_information(image_entree, i) != '\0'){
            printf("%c", decoder_information(image_entree, i));
            i+= (int)(decoder_information(image_entree, i)+17) * 3;
        }
    }
    else{
        printf("Erreur de fichier !\n");
        exit(EXIT_FAILURE);
    }
}
int longueur_du_message(FILE* fichier, int debut, int taille_fichier_octet)
{
    int i,j=0;
    for(i=debut, j=0; i<taille_fichier_octet; i+= 8, j++)
    {
        if(decoder_information(fichier, i) == '\0') break;
    }
    return j;
}

int taille_fichier(FILE* src)
{
	rewind(src);
	char c = fgetc(src);
	int taille = 0;
	while(c != EOF){
		c = fgetc(src);
		taille++;
	}
	rewind(src);
	return taille;
}

void copier_fichier(char* nom,char* sortie)
{
    char commande[200] = "cp ";
    strcat(commande, nom);
    strcat(commande, " ");
    strcat(commande, sortie);
    system(commande);
}

void recuperer_texte_dans_le_fichier(FILE* fichier,char* tab,int debut, int fin)
{
    int i,j;
    for(i=debut,j=0; i<debut+fin*8; i += 8,j++)
    {
        tab[j] = (char)decoder_information(fichier, i);
    }
}

int longueur_du_mot_de_passe(FILE* fichier, int debut, int taille_fichier_octet)
{
    int i,j=0,present=0;
    for(i=debut, j=0; i<taille_fichier_octet; i+= 8, j++)
    {
        if((char)decoder_information(fichier, i) == '\0' && j <=12) {present = 1;break;}
    }
    if(present == 0) j = 0;
    return j;
}

char* mettre_fichier_dans_tableau(FILE* fichier)
{
    char c = -2;
    char* tab = malloc(sizeof(char) * taille_fichier(fichier));
    int i = 0;
    rewind(fichier);
    while(c != EOF)
    {
        c = fgetc(fichier);
        tab[i] = c;
        i++;
    }
    tab[taille_fichier(fichier)-1] = '\0';
    return tab;
}

int est_ascii(FILE* fichier)
{
    rewind(fichier);
    char c = -2;
    while(c != EOF)
    {
        c = fgetc(fichier);
        if(c != EOF){
            if(!(c>=0 && c <= 127)) return 0;
        }
    }
    return 1;
}

int verifier_signature(FILE* fichier, int debut)
{
    char signature[16];
    recuperer_texte_dans_le_fichier(fichier,signature,debut,strlen("projet_l1_2020"));
    if(strcmp(signature,"projet_l1_2020") == 0) return 1;
    return 0;
}


char* recuperer_mot_de_passe(FILE* fichier, int debut, int taille_fichier_octet)
{
    char signature_pass[30];
    recuperer_texte_dans_le_fichier(fichier,signature_pass,debut+strlen("projet_l1_2020")*8,strlen("pass:"));
    if(strcmp(signature_pass,"pass:") == 0){
        int longueur_mdp = longueur_du_mot_de_passe(fichier,debut + strlen("projet_l1_2020")*8 + strlen("pass:")*8,taille_fichier_octet);


        if(longueur_mdp != 0)
        {
            char* mot_de_passe = malloc(sizeof(char) * longueur_mdp);
            recuperer_texte_dans_le_fichier(fichier,mot_de_passe,debut+strlen("projet_l1_2020")*8+strlen("pass:")*8,longueur_mdp);
            return mot_de_passe;
        }
    }
    return NULL;
}

void afficher_texte_decode(FILE* image_entree,int espacement)
{
        int  debut = obtenir_information(image_entree,10,4), taille_fichier_octet = obtenir_information(image_entree,2,4), i;
        i=debut + longueur_du_message(image_entree,debut,taille_fichier_octet)*8+(8*espacement);
        while(decoder_information(image_entree, i) != '\0'){
            printf("%c", decoder_information(image_entree, i));
            i+=(8*espacement);
        }
}





void decrypter(FILE* image_sortie, char* mot_de_passe_entre, int espacement)
{

        if(image_sortie != NULL){
        int  debut = obtenir_information(image_sortie,10,4), taille_fichier_octet = obtenir_information(image_sortie,2,4);
        char* mot_de_passe = NULL;
        if(verifier_signature(image_sortie,debut) == 1)
        {
            if(recuperer_mot_de_passe(image_sortie,debut,taille_fichier_octet) == NULL)
            {
                afficher_texte_decode(image_sortie, espacement);
                exit(EXIT_SUCCESS);
            }
            else if(recuperer_mot_de_passe(image_sortie,debut,taille_fichier_octet) != NULL && mot_de_passe_entre != NULL)
            {
                mot_de_passe = recuperer_mot_de_passe(image_sortie,debut,taille_fichier_octet);
                if(strcmp(mot_de_passe,mot_de_passe_entre) == 0)
                {
                afficher_texte_decode(image_sortie,espacement);
                    exit(EXIT_SUCCESS);
                }   
                else{
                    printf("Mot de passe incorrect\n");
                    exit(EXIT_FAILURE);
                }
            }
            else{
                printf("Ce fichier comporte un mot de passe, merci de le saisir avec l'option -m ou -mdp\n");
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            printf("Ce fichier n'a pas ete crypte par ce programme.\n");
            exit(EXIT_FAILURE);
        } 
    }
    else {
        printf("Erreur de fichier !\n");
        exit(EXIT_FAILURE);
    }
}


void crypter(FILE* image_entree, FILE* texte_a_crypter ,char* mot_de_passe_entre, char* nom_du_fichier_a_ouvrir, int espacement,char* nom_fichier_sortie)
{
            FILE* image_sortie = NULL;
            char txt_signature[400] = "projet_l1_2020";
            char* message = NULL;
            if(texte_a_crypter != NULL && image_entree != NULL && verifier_si_fichier_est_bmp(image_entree) == 1)
            {
                int  debut = obtenir_information(image_entree,10,4), taille_fichier_octet = obtenir_information(image_entree,2,4);
                if(est_ascii(texte_a_crypter)){
                message = mettre_fichier_dans_tableau(texte_a_crypter);
                if(mot_de_passe_entre != NULL && strlen(mot_de_passe_entre) <= 12)
                {
                    strcat(txt_signature,"pass:");
                    strcat(txt_signature,mot_de_passe_entre);
                }
                else strcat(txt_signature,"     ");
                int caractere_possible = (int)(taille_fichier_octet - debut - strlen(txt_signature) * 8) / (8*espacement);
                if(caractere_possible > (int)(strlen(message) + strlen(txt_signature))){
                printf("Cryptage en cours ...\n");
                copier_fichier(nom_du_fichier_a_ouvrir,nom_fichier_sortie);
                image_sortie = fopen(nom_fichier_sortie, "r+");
                coder_message(image_sortie, debut, (int)strlen(txt_signature), txt_signature,1,1);
                coder_message(image_sortie, debut+(int)strlen(txt_signature)*8+(8*espacement), (int)strlen(message), message,espacement,1);
                printf("Cryptage réussi\n");

                fclose(image_entree); fclose(texte_a_crypter);
                exit(EXIT_SUCCESS);

                }
                else{
                    printf("La taille du fichier bmp ne permet pas de coder ce message, utilisez une autre image ou raccourcissez votre message\n");
                    fclose(image_entree); fclose(texte_a_crypter);
                    exit(EXIT_FAILURE);
                }
            }
            else{
                printf("Votre texte a crypter contient des caractères interdits. Caracteres ascii uniquement !\n");
                fclose(image_entree); fclose(texte_a_crypter);
                exit(EXIT_FAILURE);
            }

            }
            else{
                printf("Erreur de fichier ou d'argument. \n");
                printf("Erreur d'argument. Utilisation : ./crypteur -crypt;-decrypt(-c,-d) image.bmp -fichier(-f) source [-sortie,(-s)] sortie.bmp [-mdp,-m] motdepasse [-e] n [-p]\n");
                exit(EXIT_FAILURE);
            }
}


void crypter_parano(FILE* image_entree, FILE* texte_a_crypter, char* nom_du_fichier_a_ouvrir, char* nom_fichier_sortie)
{
            FILE* image_sortie = NULL;
            char* message = NULL;
            if(texte_a_crypter != NULL && image_entree != NULL)
            {
            int  debut = obtenir_information(image_entree,10,4), taille_fichier_octet = obtenir_information(image_entree,2,4);
                if(est_ascii(texte_a_crypter)){
                message = mettre_fichier_dans_tableau(texte_a_crypter);
                copier_fichier(nom_du_fichier_a_ouvrir,nom_fichier_sortie);
                image_sortie = fopen(nom_fichier_sortie, "r+");
                printf("Cryptage en cours ...\n");
                coder_mot_dictionnaire(image_sortie);
                if(coder_message_parano(image_sortie, debut, message,taille_fichier_octet) == 0)
                {
                    printf("La taille du fichier bmp ne permet pas de coder ce message, utilisez une autre image ou raccourcissez votre message !\n");
                    fclose(image_entree); fclose(texte_a_crypter);
                    exit(EXIT_FAILURE);
                }
                else{
                printf("Cryptage réussi\n");

                fclose(image_entree); fclose(texte_a_crypter);

                exit(EXIT_SUCCESS);
                }
                }
            else{
                printf("Votre texte a crypter contient des caractères interdits. Caracteres ascii uniquement !\n");
                fclose(image_entree); fclose(texte_a_crypter);
                exit(EXIT_FAILURE);
            }

            }
            else{
                printf("Erreur de fichier\n");
                exit(EXIT_FAILURE);
            }
}


void mettre_en_place_onglet(SDL_Renderer *renderer, char* txt)
{
    int width = 420, height = 340;
    SDL_Surface* image = IMG_Load("Ressources/croix.bmp");
    SDL_Surface* logo = IMG_Load("Ressources/logo.bmp");
    TTF_Font * font = TTF_OpenFont("Ressources/cool.ttf", 20);
    SDL_Color color = { 255, 255, 255 };

        SDL_SetRenderDrawColor( renderer, 160, 160, 160, 255 );
        SDL_RenderClear( renderer );

    SDL_Surface * surface = TTF_RenderText_Blended(font,txt, color);
    SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, surface);

        int texW = 0;
        int texH = 0;
        SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
        SDL_Rect dstrect = { 0, 0, texW, texH };
        SDL_RenderCopy(renderer, texture, NULL, &dstrect);
        SDL_Rect image_rect = {100,120,120,120};
        image_rect.x = width/2 - 60;
        image_rect.y = height/2 - 60;

    texture = SDL_CreateTextureFromSurface(renderer, image);
    SDL_RenderCopy(renderer, texture, NULL, &image_rect);
    image_rect.w = 180;
    image_rect.h = 50;
    image_rect.x = width - image_rect.w;
    image_rect.y = height - image_rect.h;
    texture = SDL_CreateTextureFromSurface(renderer, logo);
    SDL_RenderCopy(renderer, texture, NULL, &image_rect);

    SDL_RenderPresent(renderer);
}

void lancer_interface_graphique()
{
        SDL_Init(SDL_INIT_VIDEO);
        TTF_Init();

        SDL_Window* Window = NULL;
        SDL_Event event;
        Window = SDL_CreateWindow("Crypteur d'image",SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,420,340,SDL_WINDOW_SHOWN);
        SDL_Renderer *renderer = SDL_CreateRenderer(Window,-1,SDL_RENDERER_ACCELERATED);
        mettre_en_place_onglet(renderer, "Glissez-deposez votre image BMP");


       int espacement = 5;
       SDL_EventState(SDL_DROPFILE, SDL_ENABLE);
       int quit = 0,fenetre = 1;
       char* nom_du_fichier_a_ouvrir, *nom_texte_a_crypter;
       FILE* texte_a_crypter = NULL;
       FILE* image_entree = NULL;

       while(!quit){
        while(SDL_PollEvent(&event))
        {
            if(event.type == SDL_QUIT) quit = 1;
            else if(event.type == SDL_DROPFILE)
            {
                    if(fenetre == 1){
                    nom_du_fichier_a_ouvrir = event.drop.file;
                    image_entree = fopen(nom_du_fichier_a_ouvrir, "r+");
                    if(image_entree != NULL && verifier_si_fichier_est_bmp(image_entree)){
                        int  debut = obtenir_information(image_entree,10,4);
                        if(verifier_signature(image_entree,debut) == 1){
                            decrypter(image_entree,NULL,espacement);
                        }
                        else{
                            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,"Message","Glissez-deposez un fichier texte contenant le message à crypter, attention uniquement des caractères ascii !",Window);
                            mettre_en_place_onglet(renderer, "Glissez-deposez un fichier texte.");
                            fenetre = 2;
                        }
                    }
                    }
                    else if(fenetre == 2)
                    {
                        nom_texte_a_crypter = event.drop.file;
                        texte_a_crypter = fopen(nom_texte_a_crypter,"r");
                        crypter(image_entree,texte_a_crypter,NULL,nom_du_fichier_a_ouvrir,espacement,"fichier_cryptee.bmp");
                    }
            }
        }
    }


        SDL_DestroyWindow(Window);
        TTF_Quit();
        SDL_Quit();

}