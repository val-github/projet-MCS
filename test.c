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
#include <assert.h>

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

char message[MAX_BUFF]="";
char messageRecu[MAX_BUFF]="";
pthread_t tidSrv[NBCLIENT];
pthread_t tidClt[2];

void recevoir_message(int socket, char *buffer);
void envoyer_message(int socket, char *message);
int tailleChaine(const char chaine[]);
int comparer(const char chaine1[], const char chaine2[]);
int parse_message(char *buffer,const char *str, const char *delim, int position);

typedef struct
{
	char *idClient;
	char pseudo[MAX_CHAR];
	int socket_client;
	struct sockaddr_in sockaddr_client;
}T_Client;

T_Client T_Ens_Player[MAX_CHAR];

typedef void * (*pf_t) (void *);

void initialiser_string(char string[MAX_BUFF]){
	sprintf(string,"");
}

void initialiser_message(){
	sprintf(message," ");
}

void viderBuffer()
{
    int c = 0;
    while (c != '\n' && c != EOF){
        c = getchar();
    } 
}

void check_pseudo(T_Client client){//vérification de l'existence du pseudo
    FILE fclient=NULL;
    fclient=fopen(NOM_FICHIER,"r+");
    int flag=0;
    char pseudo[MAX_CHAR]="";

    //on lit dans notre socket socket_client et on met le message dans messageRecu
    //attente du pseudo du joueur
    recevoir_message(client->socket_client,messageRecu);
    printf("\t%s : Pseudo: %s\n", client->idClient, messageRecu);


    //LECTURE DANS LE FICHIER
    if(fclient != NULL ){
        while(fscanf(fclient,"%s",pseudo) != EOF){// On lit le fichier tant qu'on est pas arrivé en fin de fichier (EOF)
            if(strcmp(messageRecu,pseudo) == 0){
                flag=1;
                break;
            }
        }

        if(flag == 0){
            printf("\n\t%s est un nouveau client, pseudo ajouté !\n",messageRecu);
            strcpy(pseudo,messageRecu);
        }
        fclose(fclient);

        initialiser_string(message);
        sprintf(message,"%s", pseudo);
        envoyer_message(client->socket_client,message);

        strcpy(client->pseudo,pseudo);

    }
    else{
        printf("Impossible d'ouvrir le fichier %s",NOM_FICHIER);
    }
}


void serveur_multiple(T_Client *client, int socketClient, struct sockaddr_in clnt, int *pt_nb_client){
	char messageRecu[MAX_BUFF]="";
	char msg[MAX_BUFF];

	client=(T_Client*)malloc(sizeof(T_Client));
	assert(client != NULL);

	
	client->socket_client=socketClient;
	client->sockaddr_client=clnt;
	client->idClient=inet_ntoa((client->sockaddr_client).sin_addr);
		
	check_pseudo(client);
		
	for (int i = 0; i < NBCLIENT; i++)
	{
		//création threads serveur
		CHECK(pthread_create (&tidSrv[i], NULL, ThreadDialogue, socketEcoute[i]),
                "pthread_create()");
		while(1)
		{ 
			recevoir_message(client->socket_client,messageRecu);
			parse_message(msg,messageRecu,"-",0);
			if (comparer(messageRecu, "STOP") == 0)
			{//si non stop
				break;
			}

			envoyer_message(client->socket_client,messageRecu);
			printf("recus: %s \n", messageRecu);
		
		}

		printf("fin de l'écoute");
		close(client->socket_client);
	}
	
		
	exit(EXIT_SUCCESS);
}

void * ()

int start_server(const int port) {
	int socket_server=-1;
	struct sockaddr_in serv;

	CHECK(socket_server = socket(PF_INET, SOCK_STREAM, 0), "Erreur création socket serveur");
	serv.sin_family = PF_INET;

	/* PORT */
	if(port <= 0 || port >= 65536) {
		printf("[Reseau] : Port d'ecoute invalide\n");
		return -1;
	}
	serv.sin_port = htons(port); 		//num port serveur à assigner manuellement

	serv.sin_addr.s_addr = INADDR_ANY; 	//toutes les interfaces IP de la machine
	
	CHECK(bind(socket_server, (struct sockaddr*)&serv, sizeof(serv)), "PB -- bind()");
	memset(&serv.sin_zero, 0, 8);
	
	return socket_server;
}

int connexion(char **arguments) {
	int socket_client = -1;	
	struct sockaddr_in serv;
	int port=atoi(arguments[1]);
	char *adresse=arguments[2];

	printf("port: %d et adresse: %s\n", port,adresse);
	CHECK(socket_client = socket(PF_INET, SOCK_STREAM, 0), "Erreur création socket client");
	serv.sin_family = PF_INET;

	//PORT
	if(port <= 0 || port >= 65536) {
		printf("[Reseau] : Port de connexion invalide\n");
		return -1;
	}
	serv.sin_port = htons(port); /* Host to network short */

	//ADRESSE IP
	serv.sin_addr.s_addr = inet_addr(adresse); 		//toutes les interfaces IP de la machine
	memset(&serv.sin_zero, 0, 8); 			

	//CONNEXION 
	int servLen = sizeof(serv);
	CHECK(connect(socket_client, (struct sockaddr*)&serv, servLen), "echangeCS.c : Erreur de connexion au serveur");

	return socket_client;	
}

int parse_message(char *buffer,const char *str, const char *delim, int position)
{
	char *next = NULL;
	int i = 0,j=0;

	
	if(str == NULL || delim == NULL || position <0)
		return EXIT_FAILURE;		

	//creation d'une copie de la chaine à spliter
	strcpy(buffer,str);

	//recuperation du premier mot avant le delimiteur
	//stocké dans next ET dans buffer
	next = strtok(buffer, delim);

	//on parcourt le mot selon la position que l'on souhaite, par ex: position0-position1-position2
	for(i=0;(i<=position) && (next != NULL);i++){
		//si on est sur la bonne position le mot extrait (resultat du strtock) est stocké dans le buffer
		if(i == position){
			for(j=0;j<strlen(next);j++){
				buffer[j]=next[j];
			}
			buffer[j]='\0';
			return EXIT_SUCCESS;
		}
		//sinon on continue d'extraire 1 par 1 les mots avant un délimiteur
		next = strtok(NULL, delim);
	}

	return EXIT_FAILURE;
}

//fonction pour comparer les messages a d autres chaines de caractéres
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

int tailleChaine(const char chaine[])
{
    int i=0;
    while (chaine[i]!='\0')
    {
        i++;
    }
    return i;
}

void envoyer_message(int socket, char *message) {
	CHECK(send(socket, message, strlen(message)+1, 0), "echangeCS.c -> Erreur send");
	printf("envoie message: %s\n", message);
}

void recevoir_message(int socket, char *buffer) {
	//strcpy(buffer,"");
	memset(buffer, 0, MAX_BUFF);
	CHECK(read(socket, buffer, MAX_BUFF), "echangeCS.c -> Erreur read");
	printf("reception message %s\n", buffer);
}

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

void bienvenue(int socket_client, char **arguments){
    char pseudo[MAX_BUFF];

    inialiser_message();
    printf("\n\n\t\tBienvenue sur le tchat, écrivez votre pseudo: ");
    scanf("%s",message);

    //envoi du pseudo
    envoyer_message(socket_client,message);

    //bienvenue de la part du serveur avec le nombre de points totalisé
    recevoir_message(socket_client,messageRecu);
    parse_message(pseudo,messageRecu,"-",0);

    printf("\n\t\tBonjour %s, vous avez %s en pseudo !\n", pseudo,pseudo);

    ecrireFichierEnregistrement(pseudo,arguments[2],atoi(arguments[1]));
}

void threadEnvoi( void* socket_clientV){
	printf("début thread envoie\n");
	int socket_client = *((int*) socket_clientV);
	char msg[MAX_BUFF];
	while(1){
		initialiser_message();
		// lecture message tapé par le client
		scanf("%s",message);
		parse_message(msg,message,"-",0);
		if (comparer(msg,"STOP") == 0){
			envoyer_message(socket_client, msg);
			break;
		}
		if (msg != NULL){
			// envoi du message
			envoyer_message(socket_client, msg);
		}	
	}
	printf("déconnection\n");
}

void threadReception( void* socket_clientV){
	printf("debut thread récéption\n");
	int socket_client = *((int*) socket_clientV);
	char msg[MAX_BUFF];
	char messageRecu[MAX_BUFF];
	while(1){
		// lecture message envoyé par le serveur
		recevoir_message(socket_client,messageRecu);
		parse_message(msg,messageRecu,"-",0);
		
		if (comparer(msg,"STOP") == 0){
			printf("arret du serveur");
			break;
		}
		//affichage message
		printf("recu: %s", msg);
	}
}

void dialogue(int socket_client){
	printf("début dialogue\n");

	//thread d'envoi des messages
	CHECK(pthread_create(&tidClt[0], NULL, (pf_t) threadEnvoi, (void *) &socket_client),
                "pthread_create() threadEnvoi");

	//thread d affichage des messages			
	CHECK(pthread_create(&tidClt[1], NULL, (pf_t) threadReception, (void *) &socket_client),
                "pthread_create() threadReception");
	
	//attente de la fin des threads
	CHECK(pthread_join (tidClt[0], NULL),"pthread_join()");
	CHECK(pthread_join (tidClt[1], NULL),"pthread_join()");
}

int main(int argc, char **argv)
{
	
#ifdef CLIENT
int socket_client;

	if(argc<3){ // port puis adresse ip
		printf("Argument manquant, réessayez...\n");
	}
	else{
		//Le client se connecte sur un port
		socket_client=connexion(argv);

		bienvenue(socket_client,argv);
		dialogue(socket_client);
		printf("end\n");
	}


#endif

#ifdef SERVEUR
	int s,clntLen,newsock;
	struct sockaddr_in clnt;
	T_Client *client=NULL;
	int nb_client=0;
	int port=-1;
	if(argc <2){
		printf("Argument manquant, réessayez...\n");
	}
	else{
		port=atoi(argv[1]);
		s=start_server(port);

		printf("\n\t\t** tchat ouvert! **\n\n");

		while(1)
		{
			clntLen = sizeof(clnt);
			CHECK(listen(s, 13), "Erreur listen"); //chiffre quelconque 
			CHECK(newsock = accept(s, (struct sockaddr*)&clnt, (socklen_t*)&clntLen), "PB -- accept()");	//on accepte la connexion entrante,
																										//créé une nouvelle socket (newsock)
																										//et récupère les infos du client
			serveur_multiple(client,newsock,clnt,&nb_client);
		}
		//close(s);
	}
#endif
	return 0;
}
