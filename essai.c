#include<stdio.h>
#include<stdlib.h>
#include <string.h>


#define NOM_FICHIER "enregistrement.txt"
#define MAX_CHAR 512

typedef struct
{
	int NbClient;
	char * pseudo;
	char * IPclient;
	char *  portClient;
}T_Client;

/*
int nbLigne()
{
	int c, nbLigne = 0;
	FILE* fichier = NULL;
	fichier = fopen(NOM_FICHIER,"r");
	if (fichier != NULL)
	{
		while((c=fgetc(fichier)) != EOF)
		{
			if(c=='\n')
				nbLigne++;
		}
	
	}
return nbLigne;
}*/

void lireEnregistrement(T_Client *clt)
{
	int caractereActuel = 0;
	char chaine [MAX_CHAR],str[MAX_CHAR];
	int ligne =0;
	int compteur = 0;
	FILE* fichier = NULL;
	fichier = fopen(NOM_FICHIER,"r");
    
	if (fichier != NULL)
	{
		 while (!feof(fichier))
		{
			fgetc(fichier);
			compteur ++;
		}
		compteur = compteur - 1;
		printf("compteur %d \n",compteur);		
		fclose(fichier);// on ferme le fichier qui a été ouvert
	}
	else
	{
		printf("Impossible d'ouvrir le fichier %s \n",NOM_FICHIER);
	}
	FILE* fic = NULL;
	fic = fopen(NOM_FICHIER,"r");

	if (fic != NULL)
	{
		while(ligne < 10)
		{
			caractereActuel = fgetc(fic);
			if ( caractereActuel == "\n")
			{
				ligne++;
			}
		}

			fgets(chaine,compteur,fic);
			printf("fgets %s \n",chaine);
			clt->pseudo = strtok(chaine,":");
			printf("%s\n", clt->pseudo);
			if (clt->pseudo != NULL)
			{
				clt->IPclient = strtok(NULL,":");
				printf("%s\n", clt->IPclient);
	
				if (clt->pseudo != NULL)
				{
					clt->portClient = strtok(NULL,":");
					printf("%s\n", clt->portClient );
				}
			}
			else
			{
			printf("erreur Decoupage");
			} 	
 		fclose(fic);
	} 
}

void init (T_Client *clt,int NbClient)
{
	clt->NbClient=NbClient;
	printf("je suis le client n° %d\n",clt->NbClient);
}

void Affiche(T_Client *clt)
{
	printf("je suis le client n° %d\n",clt->NbClient);
	printf("j'ai le pseudo %s\n",clt->pseudo);
	printf("j'ai l'adresse n° %s\n",clt->IPclient);
	printf("j'ai le port n° %s\n",clt->portClient);

}

int main()
{
	T_Client C;
	init(&C,0);
	/*int NbLigne = nbLigne();
	printf("il y a : %d ligne(s)\n",NbLigne);*/
	lireEnregistrement(&C);
	Affiche(&C);
 

 return 0;
}



