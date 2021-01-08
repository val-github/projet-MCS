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