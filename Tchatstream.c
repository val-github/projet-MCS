#include<stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define CHECK(sts, msg) if ((sts)==-1) {perror(msg); exit(-1);}
#define NBCLIENT 5
#define PORT_SRV 15120
#define ADDR_SRV "127.0.0.1"

// Prototype
void fermeture(void);
void serveur (void);


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

	struct sockaddr_in {
  	uint8_t         sin_len;       /* longueur totale      */
   	sa_family_t     sin_family;    /* famille : AF_INET     */
   	in_port_t       sin_port;      /* le numéro de port    */
   	struct in_addr  sin_addr;      /* l'adresse internet   */
  	unsigned char   sin_zero[8];   /* un champ de 8 zéros  */
	}seAdr;

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

void serveur (void)
{
	//Déclaration de socket d'écoute et dialogue
	int socketEcoute,socketDialogue;

	struct sockaddr_in cltAdr;


	// Mise en place d'une socket d'écoute prête à la réception des connexions	
	socketEcoute = sessionSrv();

	//Attente de connexion d'un client
	while (1)
	{
		// création d'une socket de dialogue
		//socketDialogue=acceptClt(socketEcoute, &cltAdr);
		CHECK(pid=fork(), "PB-- fork()");
		
		// dialogue avec le client connecté
		//dialSrv2Clt(socketDialogue, &cltAdr);


		// Fermeture de la socket de dialogue
		CHECK(close(socketDialogue),"-- PB : close()");
		exit(0);
		

	}
	// Fermeture de la socket de dialogue : intile pour le serveur
	CHECK(close(socketDialogue),"-- PB : close()");		

	
}









void dialClt2Srv(int sad) {
	struct sockaddr_in sadAdr;
	socklen_t lenSadAdr;
	message_t buff;
    char MSG = NULL;
    
    while (MSG != "stop"){
            //lecture du message ecrit par le client
        MSG = getchar();

	    // Dialogue du client avec le serveur : while(..) { envoiRequete(); attenteReponse();}
	    // Ici on va se contenter d'envoyer un message et de recevoir une réponse	
	    // Envoi d'un message à un destinaire avec \0
	    printf("\t[CLIENT]:Envoi d'une requête sur [%d]\n", sad);
	    CHECK(send(sad, MSG, strlen(MSG)+1, 0),"-- PB : send()");
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
	    CHECK(recv(sad, buff, MAX_BUFF, 0),"-- PB : recv()");
	    printf("\t[CLIENT]:Réception d'une réponse sur [%d]\n", sad);
	    printf("\t\t[CLIENT]:Réponse reçue : ##%s##\n", buff);
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

void client () {
	int sad /*socket appel et dialogue*/;

	// Mise en place du socket d'appel PF_INET/STREAM adressée ou non
	sad=sessionClt();

	// Connexion avec un serveur
	// la socket d'appel devient une socket de dialogue (connectée)
	connectSrv(sad);

	// Dialogue du client avec le serveur
	dialClt2Srv(sad, MSG);

	// Fermeture de la socket de dialogue
	getchar();
	CHECK(close(sad),"-- PB : close()");	
}
