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

#define CHECK(sts, msg) if ((sts)==-1) {perror(msg); exit(-1);}
#define NBCLIENT 5
#define MAX_BUFF 512
#define PORT_SRV 15130
#define ADDR_SRV "127.0.0.24"
#define MAX_CHAR 512
#define NOM_FICHIER "enregistrement.txt"
typedef char message_t[MAX_BUFF];

pthread_t tid[NBCLIENT];

// Prototype
void fermeture(void);
void dialClt2Srv(int sad);
char dialSrv2Clt(int sd, struct sockaddr_in *cltAdr);
void serveur (void);
int acceptClt(int socketEcoute, struct sockaddr_in *cltAdr);
void client ();

// Variable global
int socketEcoute; /*socket écoute*/
pid_t pid;


int main () {

	atexit(fermeture);

	#ifdef SERVEUR
	serveur();
	#endif

	#ifdef CLIENT
	client();
	#endif

	printf("Fin de l'application\n");

return 0;
}

void fermeture(void)
{
	//Fermeture de la socket
	CHECK(close(socketEcoute),"--- PB : close()");
}


int sessionSrv(void) {

int socketEcoute; /*socket écoute*/

	struct sockaddr_in seAdr;

	// Création d’une socket famille : INET mode de communication : STREAM
	CHECK(socketEcoute = socket(PF_INET, SOCK_STREAM, 0),"-- PB : socket()");
	printf("[SERVEUR]:Création de la socket d'écoute [%d]\n", socketEcoute);

	// Préparation d’un adressage pour une socket INET
	seAdr.sin_family = PF_INET;
	seAdr.sin_port = htons(PORT_SRV);				// htons() : network order	
	seAdr.sin_addr.s_addr = inet_addr(ADDR_SRV);	// adresse effectiveS
	memset(&(seAdr.sin_zero), 0, 8); // Cette fonction permet de remplir une zone mémoire, identifiée par son adresse et sa taille, avec une valeur précise.


	// Association de la socket d'écoute avec l’adresse d'écoute
	CHECK(bind(socketEcoute, (struct sockaddr *)&seAdr, sizeof(seAdr)),"-- PB : bind()");

/*
inet_aton() convertit l'adresse Internet de l'hôte cp depuis la notation IPv4 décimale pointée vers une forme binaire (dans l'ordre d'octet du réseau), et la stocke dans la structure pointée par inp.
htol() === HOST TO LONG ( HOST parce que ca vient de MA machine ). Et si mon programme m'a envoi un type plus grand qu'un char
ntoh() == NETWORK TO HOST ( NETWORK parce que ca vient d'autre part que de MA machine ).
*/

	printf("[SERVEUR]:Association de la socket [%d] avec l'adresse [%s:%d]\n", socketEcoute, inet_ntoa(seAdr.sin_addr), ntohs(seAdr.sin_port));


	// Mise de la socket à l'écoute
	CHECK(listen(socketEcoute, NBCLIENT), "--PB : listen()"); // Cette fonction définit la taille de la file de connexions en attente pour votre socket .
	

// Boucle permanente (1 serveur est un daemon)
	printf("[SERVEUR]:Ecoute de demande de connexion (%d max) sur le canal [%d] d'adresse [%s:%d]\n", NBCLIENT, socketEcoute, inet_ntoa(seAdr.sin_addr), ntohs(seAdr.sin_port));
	return socketEcoute;


}

// ecrire Pseudo + IP CLIENT + PORT CLIENT
void ecrireFichierEnregistrement(char * IpClient, int PortClient)
{
char * PseudoClient = "passage04";

	FILE* fichier = NULL;
	fichier = fopen(NOM_FICHIER,"r+");
    
	if ( fichier != NULL)
	{
		// on lit et on écrit dans le fichier
		fprintf(fichier, "%s:%s:%d", PseudoClient, IpClient,PortClient);
		fclose(fichier);// on ferme le fichier qui a été ouvert
	}
	else
	{
		printf("Impossible d'ouvrir le fichier %s \n",NOM_FICHIER);
	}
//pb
}

// lire pseudo + IP CLIENT + PORT CLIENT
// fonctionnement: récupére toutes les lignes du fichier et les stocke dans une
// liste (une ligne = un élément)
// a modifer
char lireEnregistrement()
{
	int caractereActuel = 0;
	char chaine[MAX_CHAR] = "";
	int compteur = 0;
	FILE* fichier = NULL;
	fichier = fopen(NOM_FICHIER,"r");
    
	if (fichier != NULL)
	{
		// on lit caractères par caractère
		 while (!feof(fichier)) // Jusqu'a fin fichier
		{
			//chaine.add(fgetc(fichier));
			compteur ++;
		}
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
 		fgets(chaine,compteur-1,fic);
		printf("fgets %s \n",chaine);	
		fclose(fic);
	} 
	return chaine;
}

// lire pseudo + IP CLIENT + PORT CLIENT
char* decoupeLire(char * chaine)
{
	
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
		printf("erreur Decoupage");
	}
   return (PseudoClient, IpClient, PortClient);
}

//fonction des threads de dialogue
void *ThreadDialogue (int socketEcoute)
{
	struct sockaddr_in cltAdr;
	int socketDialogue;
	char msg;
	//création d'une socket de dialogue
	socketDialogue=acceptClt(socketEcoute, &cltAdr);
	while(msg != "stop"){
		
		CHECK(pid=fork(), "PB-- fork()");
		
		// dialogue avec le client connecté
		msg = dialSrv2Clt(socketDialogue, &cltAdr);



	}
	// Fermeture de la socket de dialogue
	CHECK(close(socketDialogue),"-- PB : close()");
	
}

// fonction qui gére l'envoi des messages à tout les clients
// (récupére les infos des différents clients et leur transmet msg)
void dialClt2Clt(char msg){
	char[NBCLIENT] fichier; 
	char pseudo, ip, port;
	int i = 0;

	fichier = lireEnregistrement();
	while (1){
		if (decoupeLire(fichier[i]) == NULL){
			break
		}
		(pseudo, ip, port) = decoupeLire(fichier[i]);

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


char dialSrv2Clt(int sd, struct sockaddr_in *cltAdr) {
	// Dialogue avec le client
	// Ici, lecture d'une requête et envoi du fichier
	message_t buff;
	char req;

	memset(buff, 0, MAX_BUFF);
	printf("\t[SERVER]:Attente de réception d'une requête\n");
	CHECK (recv(sd, buff, MAX_BUFF, 0), "PB-- recv()");	

	printf("\t[SERVER]:Requête reçue : ##%s##\n", buff);
	printf("\t\t[SERVER]:du client d'adresse [%s:%d]\n",
			inet_ntoa(cltAdr->sin_addr), ntohs(cltAdr->sin_port));
	sscanf(buff,"%s",&req);
	
	return (req);
	// si client demande le fichier on lui lit enregistrement puis on l'envoie
	CHECK(shutdown(sd, SHUT_WR),"-- PB : shutdown()");
	
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

	// on enregistre l'arrivé du client
	ecrireFichierEnregistrement(inet_ntoa(cltAdr->sin_addr), ntohs(cltAdr->sin_port));


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
