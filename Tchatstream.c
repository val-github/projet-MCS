/* ------------------------------------------------------------------------ */
/**
 *  \file       Tchatstream.c
 *  \brief      Programme utilisant les sockets pour réaliser un serveur multi client
 *
 *  \author     Valentine Petit et Valentin Blet Le3 TDA
 *
 *  \date       début : 08 janvier fin : 28 janvier
 *
 *	\version    3.0
 * *
 *	\remark		Le fichier doit être compiler avec -DSERVEUR pour être executer en temps que serveur
 *                      et -DCLIENT pour être executer en temps que client.
 *                      Il faut aussi ajouter -lpthread car nous utilisons les threads
 *
 */

/* ------------------------------------------------------------------------ */
/*		I N C L U D Es  S T A N D A R D S       		    */
/* ------------------------------------------------------------------------ */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>

/* ------------------------------------------------------------------------ */
/*		C O N S T A N T E S   S Y M B O L I Q U E S		    */
/* ------------------------------------------------------------------------ */
#define CHECK(sts, msg) if ((sts)==-1) {perror(msg); exit(-1);}
#define NBCLIENT 5
#define MAX_BUFF 512
#define PORT_SRV 15130
#define ADDR_SRV "127.0.0.24"
#define MAX_CHAR 512
#define NOM_FICHIER "enregistrement.txt"

/* ------------------------------------------------------------------------ */
/*		D E F I N I T I O N S    D E     T Y P E S		    */
/* ------------------------------------------------------------------------ */
typedef char message_t[MAX_BUFF];

typedef struct
{
	int NbClient;
	char * pseudo;
	char * IPclient;
	char *  portClient;
}T_Client;

/* ------------------------------------------------------------------------ */
/*		V A R I A B L E S   G L O B A L E S   			    */
/* ------------------------------------------------------------------------ */
pthread_t tid[NBCLIENT];
int socketEcoute; /*socket écoute*/
pid_t pid;


/* ------------------------------------------------------------------------ */
/*		P R O T O T Y P E S   D E   F O N C T I O N S		    */
/* ------------------------------------------------------------------------ */
void fermeture(void);
void dialClt2Srv(int sad);
char dialSrv2Clt(int sd, struct sockaddr_in *cltAdr);
void serveur (void);
int acceptClt(int socketEcoute, struct sockaddr_in *cltAdr);
void client ();




int main () {

	atexit(fermeture);

	#ifdef SERVEUR
	serveur();
	#endif

	#ifdef CLIENT
	client();
	#endif

	#ifdef FICHIER // test futur supp
	T_Client C;
	init(&C,0);
	lireEnregistrement(&C);
	Affiche(&C);
	#endif

	printf("Fin de l'application\n");

return 0;
}


/* ------------------------------------------------------------------------ */


/**
 *
 * \fn void fermeture(void)
 * \brief       Cette fonction permet de fermer la socket d'écoute global
 *
 * \param       aucun
 *
 * \return      rien
 *
 */
void fermeture(void)
{
	//Fermeture de la socket
	CHECK(close(socketEcoute),"--- PB : close()");
}

/**
 *
 * \fn void init (T_Client *clt,int NbClient)
 * \brief       Cette fonction permet d'initier un Client
 * \param       T_Client *clt un pointeur sur un client
 *
 * \param       int NbClient le numero du client
 *
 * \return      rien
 *
 */
void init (T_Client *clt,int NbClient)
{
	clt->NbClient=NbClient;
	printf("je suis le client n° %d\n",clt->NbClient);
}

/**
 *
 * \fn void Affiche(T_Client *clt)
 * \brief       Cette fonction permet d'afficher les differentes valeurs affecte au client entree en parametre 
 * \param       T_Client *clt un pointeur sur un client
 *
 * \return      rien
 *
 */
void Affiche(T_Client *clt)
{
	printf("je suis le client n° %d\n",clt->NbClient);
	printf("j'ai le pseudo %s\n",clt->pseudo);
	printf("j'ai l'adresse n° %s\n",clt->IPclient);
	printf("j'ai le port n° %s\n",clt->portClient);

}


/**
 *
 * \fn void ecrireFichierEnregistrement(char * pseudo, char * IpClient, int PortClient)
 * \brief       Cette fonction est appele une fois le client connecter au serveur,
 *       elle permet de renseigner dans le fichier enregistrement.txt, 
 *       le port du Client et l'Ip du client et son pseudo
 * \param       char * IpClient l'Ip d client accepte
 *
 * \param       int PortClient le port du client accepte
 *
 * \return      rien
 *
 */
void ecrireFichierEnregistrement(char * pseudo, char * IpClient, int PortClient)
{


	FILE* fichier = NULL;
	fichier = fopen(NOM_FICHIER,"a");// test creer fic sinon r+
    
	if ( fichier != NULL)
	{
		// on lit et on écrit dans le fichier
		fseek(fichier, 0, SEEK_END);
		fprintf(fichier, "%s:%s:%d\n", pseudo, IpClient,PortClient);
		rewind(fichier);
		fclose(fichier);// on ferme le fichier qui a été ouvert
	}
	else
	{
		printf("Impossible d'ouvrir le fichier %s \n",NOM_FICHIER);
	}
}


/**
 *
 * \fn void lireEnregistrement(T_Client *clt,int nbLigne)
 * 
 * \brief  Cette fonction permet de lire la ligne voulu dans le fichier d'enregistrement 
 * et de renseigner la struture client passer en parametre
 *       
 * \param       T_Client *clt un pointeur sur un client
 *
 * \param       int nbLigne la ligne que l'on souhaite lire
 *
 * \return      rien
 *
 */
void lireEnregistrement(T_Client *clt,int nbLigne)
{
	char caractereActuel;
	char chaine [MAX_CHAR],str[MAX_CHAR];
	int c,ligne =0;
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
		while((c=fgetc(fichier)) != EOF)
		{
			if(c=='\n')
				ligne++;
			if (ligne == nbLigne)// nb ligne choisit
			{
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
			}
		}
			
 		fclose(fic);
	} 
}

/**
 *
 * \fn int tailleChaine(const char chaine[])
 * 
 * \brief  Cette fonction permet de calculer la taille de la chaine passer en parametre
 * 
 * \param       const char chaine[] table de char dont on veut calculer la longueur
 *
 * \return      i : la taille de la chaine
 *
 */
int tailleChaine(const char chaine[])
{
    int i=0;
    while (chaine[i]!='\0')
    {
        i++;
    }
    return i;
}


/**
 *
 * \fn int comparer(const char chaine1[], const char chaine2[])
 * 
 * \brief  Cette fonction permet de comparer les chaines passer en parametre
 * 
 * \param       const char chaine1[] table de char qu'on veut comparer
 *
 * \param       const char chaine2[] table de char qu'on veut comparer
 *
 * \return      1 si elles sont differentes et 0 si elles sont equivalente
 *
 */
int comparer(const char chaine1[], const char chaine2[])
{
    long k=0;
 
	if (tailleChaine(chaine1) == tailleChaine(chaine2))
	{
    for (k; k<tailleChaine(chaine1) && k<tailleChaine(chaine2); k++)
    {
        if (chaine1[k] != chaine2[k])
            return 1;
    }
        return 0;
	}
return 1;
}

/**
 *
 * \fn char * decoupeLire(char * chaine)
 * 
 * \brief  Cette fonction permet de comparer les chaines passer en parametre
 * 
 * \param       char * chaine table de char qu'on veut decouper
 *
 * \return      (PseudoClient, IpClient, PortClient)
 *
 */
// INUTILE MNT PUISQUE DANS LIRE

char * decoupeLire(char * chaine)
{
	printf("debut decoupe lire");
	char *PseudoClient = NULL;
	char *IpClient = NULL;
	char *PortClient = NULL;

	PseudoClient = strtok(chaine,":");
	printf("%s\n", PseudoClient);
	if (PseudoClient != NULL)
	{
		IpClient = strtok(NULL,":");
		printf("%s\n", IpClient);
	
		if (PseudoClient != NULL)
		{
			PortClient = strtok(NULL,":");
			printf("%s\n", PortClient);
		}
	}
	else
	{
		return NULL;
	}
return (PseudoClient, IpClient, PortClient);
}

/**
 *
 * \fn int sessionSrv()
 * 
 * \brief  Cette fonction permet d'initialiser le serveur
 * elle cree la socket d'ecoute puis l'associe a l'adresse ip et la met sur ecoute
 * 
 * \param       aucun
 *
 * \return      socketEcoute : la socket d'ecoute cree
 *
 */
int sessionSrv()
{

	int socketEcoute;

	struct sockaddr_in seAdr;

	// Création d’une socket famille : INET mode de communication : STREAM
	CHECK(socketEcoute = socket(PF_INET, SOCK_STREAM, 0),"-- PB : socket()");
	printf("[SERVEUR]:Création de la socket d'écoute [%d]\n", socketEcoute);

	// Préparation d’un adressage pour une socket INET
	seAdr.sin_family = PF_INET;
	seAdr.sin_port = htons(PORT_SRV); // htons() : network order	
	seAdr.sin_addr.s_addr = inet_addr(ADDR_SRV);	// adresse effectiveS
	memset(&(seAdr.sin_zero), 0, 8); // Cette fonction permet de remplir une zone mémoire, identifiée par son adresse et sa taille, avec une valeur précise.


	// Association de la socket d'écoute avec l’adresse d'écoute
	CHECK(bind(socketEcoute, (struct sockaddr *)&seAdr, sizeof(seAdr)),"-- PB : bind()");


	printf("[SERVEUR]:Association de la socket [%d] avec l'adresse [%s:%d]\n", socketEcoute, inet_ntoa(seAdr.sin_addr), ntohs(seAdr.sin_port));


	// Mise de la socket à l'écoute
	CHECK(listen(socketEcoute, NBCLIENT), "--PB : listen()"); // Cette fonction définit la taille de la file de connexions en attente pour votre socket .
	

// Boucle permanente (1 serveur est un daemon)
	printf("[SERVEUR]:Ecoute de demande de connexion (%d max) sur le canal [%d] d'adresse [%s:%d]\n", NBCLIENT, socketEcoute, inet_ntoa(seAdr.sin_addr), ntohs(seAdr.sin_port));
	return socketEcoute;


}

//fonction des threads de dialogue
void *ThreadDialogue (int socketEcoute)
{
	struct sockaddr_in cltAdr;
	int socketDialogue;
	char msg;
	//création d'une socket de dialogue
	socketDialogue=acceptClt(socketEcoute, &cltAdr);
	while(msg != "true"){
		
		CHECK(pid=fork(), "PB-- fork()");
		msg = dialSrv2Clt(socketDialogue, &cltAdr);
		// dialogue avec le client connecté
		



	}
	// Fermeture de la socket de dialogue
	CHECK(close(socketDialogue),"-- PB : close()");
	
}

// fonction qui gére l'envoi des messages à tout les clients
// (récupére les infos des différents clients et leur transmet msg)
void dialClt2Clt(char msg){
	char fichier[NBCLIENT]; 
	char pseudo, ip, port;
	int i = 0;

	//fichier = lireEnregistrement();
	while (1){
		if (decoupeLire(fichier[i]) == NULL){
			break;
		}
		//(pseudo, ip, port) = decoupeLire(fichier[i]);
		struct sockaddr_in srvAdr;
		int sad;
		// Création d’une socket INET/STREAM d'appel et de dialogue
		CHECK(sad = socket(PF_INET, SOCK_STREAM, 0),"-- PB : socket()");
		
		srvAdr.sin_family = PF_INET;
		srvAdr.sin_port = htons(port);		
		srvAdr.sin_addr.s_addr = inet_addr(ip);
		memset(&(srvAdr.sin_zero), 0, 8);

		// demande connexion 
		CHECK(connect(sad, (struct sockaddr *)&srvAdr, sizeof(srvAdr)),"-- PB : connect()");
		printf("message transmis au client %d [%s:%d] par le canal [%d]\n", i,
					inet_ntoa(srvAdr.sin_addr), ntohs(srvAdr.sin_port), sad);
		CHECK(send(sad, msg, strlen(msg)+1, 0),"-pb d envois du message");
		i++;
	}
}


void serveur(void)
{
	printf("début serveur\n");
	//Déclaration de socket d'écoute et dialogue
	int socketEcoute;
	//ecrireFichierEnregistrement();
	//char chaine = lireEnregistrement();
	//decoupeLire(chaine);

	// Mise en place d'une socket d'écoute prête à la réception des connexions	
	socketEcoute = sessionSrv();
 	
	int I=0;
	//Attente de connexion d'un client
	for (int i = 0; i < NBCLIENT; i++)
	{
		printf("creation thread + %d\n", i);
		//création du thread qui gérera le dialogue avec le client
		CHECK(pthread_create (&tid[i], NULL, ThreadDialogue, socketEcoute),
                "pthread_create()");
	}

	for (int i = 0; i < NBCLIENT; i++)
        CHECK(pthread_join (tid[i], NULL),"pthread_join()");
    return EXIT_SUCCESS;

	// Fermeture de la socket d'écoute : inutile pour le serveur
	printf("fin de lecture\n");
	CHECK(close(socketEcoute),"-- PB : close()");
}




char dialSrv2Clt(int socketDialogue, struct sockaddr_in *cltAdr) {
	// Dialogue avec le client
	// Ici, lecture d'une requête et envoi du fichier
	message_t buff;
	char req;

	memset(buff, 0, MAX_BUFF);
	printf("\t[SERVER]:Attente de réception d'une requête\n");
	CHECK (recv(socketDialogue, buff, MAX_BUFF, 0), "PB-- recv()");

	printf("\t[SERVER]:Requête reçue : ##%s##\n", buff);
	printf("\t\t[SERVER]:du client d'adresse [%s:%d]\n",
			inet_ntoa(cltAdr->sin_addr), ntohs(cltAdr->sin_port));
	req = comparer(buff,"stop");
	if (req == 0)
	{
		printf("BUT ALORS YOU ARE FRENCH !");
		return "true";
		
	}
	// si client demande le fichier on lui lit enregistrement puis on l'envoie
	CHECK(shutdown(socketDialogue, SHUT_WR),"-- PB : shutdown()");
	return "false";
	sleep(1);
}

///////////////////////////////////////////////////////////////////////////////
//partie client

int acceptClt(int socketEcoute, struct sockaddr_in *cltAdr)
{
	//Déclaration de socket de dialogue
	int socketDialogue;
	socklen_t lenCltAdr=sizeof(*cltAdr);

	// Attente d'une connexion client : accept() côté serveur & connect côté client
	printf("[SERVEUR]:Attente d'une connexion client\n");

	//Le serveur accepte une connection (TCP) et alloue une socket de réponse.
	CHECK(socketDialogue=accept(socketEcoute, (struct sockaddr *)cltAdr, &lenCltAdr),"-- PB : accept()");

	printf("[SERVEUR]:Acceptation de connexion du client [%s:%d]\n", inet_ntoa(cltAdr->sin_addr), ntohs(cltAdr->sin_port));

	//on enregistre l'arrivé du client
	ecrireFichierEnregistrement("ok",inet_ntoa(cltAdr->sin_addr), ntohs(cltAdr->sin_port));


	return socketDialogue;
}

void dialClt2Srv(int sad)
{
	struct sockaddr_in sadAdr;
	socklen_t lenSadAdr;
	message_t buff;
	char MSG[MAX_CHAR] = "NULL";
    
	while (MSG != "stop"){
        //lecture du message ecrit par le client

		fgets(MSG, MAX_CHAR, stdin);
		MSG[strlen(MSG)-1]='\0';

	    // Dialogue du client avec le serveur : while(..) { envoiRequete(); attenteReponse();}
	    printf("\t[CLIENT]:Envoi du message sur [%d]\n", sad);
	    CHECK(send(sad, MSG, strlen(MSG)+1, 0),"-pb d envois du message");
	    printf("\t\t[CLIENT]:requête envoyée : ##%s##\n", MSG);

	    // La socket client n'a pas éte bindée càd non adressée
	    // l'appel send a réalisé un bind (OS) : càd attribuer une adresse à la socket dyn
	    // getsockname permet de lire l'adressage de la socket
	    lenSadAdr = sizeof(sadAdr);
	    CHECK(getsockname(sad, (struct sockaddr *)&sadAdr, &lenSadAdr),"-- PB : bind()");
	    printf("\t\t[CLIENT]: avec l'adresse [%s:%d]\n",
				    inet_ntoa(sadAdr.sin_addr), ntohs(sadAdr.sin_port));

	    // Attente d'une réponse
	    memset(buff, 0, MAX_BUFF);
		while (buff == "NULL"){
			CHECK(recv(sad, buff, MAX_BUFF, 0),"-pb reception message serveur");
	    	printf("\t[CLIENT]:Réception d'une réponse sur [%d]\n", sad);
	    	printf("\t\t[CLIENT]:Réponse reçue : ##%s##\n", buff);
		}
	    
        //test d'arret de la discussion
        if (buff == "stop"){
            printf("fin de la discussion");
            break;
        }
    }
}

int sessionClt(void) {
	int sad;
	// Création d’une socket INET/STREAM d'appel et de dialogue
	CHECK(sad = socket(PF_INET, SOCK_STREAM, 0),"-- PB : socket()");
	printf("[CLIENT]:Création de la socket d'appel et de dialogue [%d]\n", sad);		
	// Le client n'a pas besoin d'avoir une adresse mais il peut
	// ICI, Pas de préparation ni d'association
	return sad;
}

void connectSrv(int sad) {
	struct sockaddr_in srvAdr;

	// le client doit fournir l'adresse du serveur
	srvAdr.sin_family = PF_INET;
	srvAdr.sin_port = htons(PORT_SRV);		
	srvAdr.sin_addr.s_addr = inet_addr(ADDR_SRV);
	memset(&(srvAdr.sin_zero), 0, 8);

	// demande connexion 
	CHECK(connect(sad, (struct sockaddr *)&srvAdr, sizeof(srvAdr)),"-- PB : connect()");
	printf("[CLIENT]:Connexion effectuée avec le serveur [%s:%d] par le canal [%d]\n",
				inet_ntoa(srvAdr.sin_addr), ntohs(srvAdr.sin_port), sad);	
}

void client() {
	printf ("lancement client");
	int sad /*socket appel et dialogue*/;

	// Mise en place du socket d'appel PF_INET/STREAM adressée ou non
	sad=sessionClt();

	// Connexion avec un serveur
	// la socket d'appel devient une socket de dialogue (connectée)
	connectSrv(sad);

	// Dialogue du client avec le serveur
	dialClt2Srv(sad);

	// Fermeture de la socket de dialogue
	getchar();
	CHECK(close(sad),"-- PB : close()");	
}


