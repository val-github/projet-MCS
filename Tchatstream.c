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
