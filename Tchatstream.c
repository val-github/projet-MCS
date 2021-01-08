#include<stdio.h>

#define CHECK(sts, msg) if ((sts)==-1) {perror(msg); exit(-1);}

#define PORT_SRV	15120
#define ADDR_SRV	"127.0.0.1"

int socketEcoute; /*socket écoute*/

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


void serveur (void) {
	int socketEcoute, socketDialogue /*socket dialogue*/;
	struct sockaddr_in cltAdr;

	// Mettre en place une socket d'écoute prête à la réception des connexions	
	se = sessionSrv();
	while (1) {
		// attente de connexion d'un client et création d'une socket de dialogue
		sd=acceptClt(se, &cltAdr);
		CHECK(pid=fork(), "PB-- fork()");
		if (pid == 0) {
			CHECK(close(se),"-- PB : close()"); // inutile pour le dialogue	
			// dialogue avec le client connecté
			dialSrv2Clt(sd, &cltAdr);
			// Fermeture de la socket de dialogue
			CHECK(close(sd),"-- PB : close()");
			exit(0);
		}
		// Fermeture de la socket de dialogue : intile pour le serveur
		CHECK(close(sd),"-- PB : close()");		
	} 
	// Fermeture de socket
	CHECK(close(se),"-- PB : close()");
}

#include<stdio.h>








void dialClt2Srv(int sad, const char * MSG) {
	struct sockaddr_in sadAdr;
	socklen_t lenSadAdr;
	message_t buff;

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

void client (const char * MSG) {
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