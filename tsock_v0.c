#include <sys/types.h>
/* constantes relatives aux domaines, types et protocoles */
#include <sys/socket.h>
/* constantes et structures propres au domaine UNIX */
#include <sys/un.h>
/* constantes et structures propres au domaine INTERNET */
#include <netinet/in.h>
/* structures retournées par les fonctions de gestion de la base de
données du réseau */
#include <netdb.h>
/* pour les entrées/sorties */
#include <stdio.h>
/* pour la gestion des erreurs */
#include <errno.h>
#include <stdlib.h>
/*pour getopt*/
#include <unistd.h>


/*---------------------------------------------------------------Ceci est la première partie -----------------------------------------------------------*/



/*---------------creation de fonction-----------------------*/
/* ----------------------déclaration de fonctions ----------------------------*/ 

void construire_message2(char *message,char motif,int lg,int i);
void afficher_message(char *message, int lg);
void envoi_UDP(int port, int nb_message  , int lg_msg, char*dest);
void reception_UDP(int port, int nb_message , int lg_message);
void construire_message(char *message,char motif,int lg);
/*---Création du socket UDP et envoi et reception ---------*/

				      


/*--------------définition de fonction --------------------------------------------------*/
/*----------------fonction construction de message---------------------------------*/ 
void construire_message(char *message,char motif,int lg){
	int i ;
	for (i=0;i<lg;i++)
		message[i]=motif;
}

/*-----------------------------construction de messages selon le cahier de charge ---------------------------*/

void construire_message2(char *message,char motif,int lg,int i)
{
  int j;
  char alpha[]="abcdefghijklmnopqrstuvwxyz";
  if (i>=27)
  {
      if (i%26==0)
      
	motif='z';
      
      else
	
	
	motif=alpha[i%26 -1];
  }	
      else
	 motif=alpha[i-1];
  
  for(j=0; j<lg-5 ; j++) {
      *(message+j+5)=motif;
    }
}
/*------focntion de formattage msg------------------------------------------------------*/
void affichage2(int n , char*message)
{
	if (n>=10000)
    {
        printf("Trop de messages à send \n");
        exit(1);
    }

    if (n<10)
    {
        for (int i=0 ; i<4 ; i++)
        {
	  *(message+i)='-'; // si message pas trop grand <10 4 premier caractères sont des -
        }
        //char nb[1];      
        *(message+4)= n + '0';
    }
    if (n>=10 & n<100) // si 10<n<100 3 premier paramètre sont des - 
    {
    	char nb[2];

        for (int i=0 ; i<3 ; i++)
        {
            *(message+i)='-';
        }
        sprintf(nb,"%d",n);
        *(message+3)=*nb;
        n=n%10;
        sprintf(nb,"%d", n);
        *(message+4)=*nb;
    }
    if (n>=100 & n<1000) //si 100<n<1000 2 premier caractère sont des -
    {
        char nb[3];
        for (int i=0 ; i<2 ; i++)
        {
            *(message+i)='-';
        }
        sprintf(nb,"%d",n);
        *(message+2)=*nb;
        n=n%100;
        sprintf(nb,"%d", n);
        *(message+3)=*nb;
        n=n%10;
        sprintf(nb,"%d", n);
        *(message+4)=*nb;
    }
    if (n>=1000 & n<10000) // si 1000<n<100000 juste le 1 er caractère est un - 
    {
    	int j=1000;

        char nb[4];
        for (int i=0 ; i<2 ; i++)
        {
            *(message+i)='-';
        }

        sprintf(nb,"%d",n);
        *(message+1)=*nb;
        n=n%1000;
        sprintf(nb,"%d", n);
        *(message+2)=*nb;
        n=n%100;
        sprintf(nb,"%d", n);
        *(message+3)=*nb;
        n=n%10;
        sprintf(nb,"%d", n);
        *(message+4)=*nb;
    }
   
}



/*------------------fonction affichage  message ----------------------------------------*/ 
void afficher_message(char *message, int lg)

{
  	for (int i=0;i<lg;i++)
  	    printf("%c", *(message+i));
  	printf("]\n");
}

/*-----------------------fonction envoi sous protocole UDP -------------------------------*/ 

void envoi_UDP(int port ,int nb_mess , int lg_msg,char*dest)
{
	
  	int length_addr_dist;
  	length_addr_dist=sizeof(struct sockaddr_in);
 	 
  	int sock;
 	int sent;
  	struct sockaddr_in addr_distant;
  	struct hostent* hp;
  	char *message=malloc(sizeof(char)*lg_msg) ;

   	if((sock=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP))==-1) /* creation socket + test erreur ( -1 si le socket ne s'ouvre pas correctement) */
    {
      printf("pb création du socket\n");
      exit(1);
    }
  	memset((char*)&addr_distant,0,sizeof(addr_distant));
  	addr_distant.sin_family=AF_INET;  /*internet*/ 
  	addr_distant.sin_port=htons(port);  /*num de port */ 

  	if((hp=gethostbyname(dest))==NULL)
    {
      printf("Erreur du gesthostbyname _n");/*num de port */ 
      exit(1);
    }

  	memcpy((char*)&(addr_distant.sin_addr.s_addr), hp->h_addr, hp->h_length);

  	for (int i=1; i<=nb_mess ; i++)
    {

        affichage2(i,message);
	construire_message2(message,'a',lg_msg,i);
      	printf("SOURCE : Envoi n°%d (%d) : [", i, lg_msg);
      	

      	if((sent=sendto(sock,message,lg_msg,0,(struct sockaddr*)&addr_distant,sizeof(addr_distant)))==-1)   
      	{
      		printf("Erreur sendto\n");
      		exit(1);
      	}
      	afficher_message(message,sent);
    }

  	if(close(sock)==-1) //fermeture du socket + test 
    {
      printf("Echec à la destruction du socket\n");
      exit(1);
    }
}
/*------------------------------------------Reception UDP-------------------------------------------*/


void reception_UDP(int port, int nb_message, int lg_message)
{

  int sock;
  struct sockaddr_in addr_local,addr_distant;
  int recv;
  int lg_dist;
  char *message=malloc(sizeof(char)*lg_message) ;
  sock=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP); 
  if(sock==-1) /*creation du socket*/ 
    {
      printf("Erreur à la création du socket\n");
      exit(1);
    }
  
  	memset((char*)&addr_local,0,sizeof(addr_local));         /*reset de addr_local*/ 
 	addr_local.sin_family=AF_INET;              //attribution des différents attributs de addr_local
  	addr_local.sin_port=htons(port);
  	addr_local.sin_addr.s_addr=INADDR_ANY;     //par defaut n'imp quelle machine 

		
  	if ((bind(sock,(struct sockaddr*)&addr_local, sizeof(addr_local)))==-1) //bind de la réception 
    {
      	printf("Echec du Bind\n");
      	exit(1);
    }
	


  	lg_dist=sizeof(addr_distant);

    int i=1;
    while(recv!=0)
    {
        recv=recvfrom(sock,message,lg_message,0,(struct sockaddr*)&addr_distant,&lg_dist);
        if (recv==-1) // prcess de réception recvfrom natif à l'API socket, + test 
        {
            printf("Erreur receive from\n");
            exit(1);
        }

        if (recv!=0)
        {
	  printf("PUITS : Réception n°%d (%d) : [",i,lg_message);  //mise en forme 
          afficher_message(message,recv);
        }
        if (i==nb_message)
        {
            recv=0;
            printf("On a atteint le nombre de messages à recevoir\n");
        }

        i++;
    }
  
   if(close(sock)==-1) 
    {
      printf("Echec à la destruction du socket\n");
      exit(1);
    }
}

/*----------------------------Protocole TCP ------------------------------*/
/*-----------------------------------coté recepteur --- source=1 -----------------------*/
void recepteurTCP(int port , int nb_message, int lg_msg,char*dest)
{
  int sock;
  int sent;
  struct sockaddr_in addr_distant ;
  int lg_addr_distant=sizeof(addr_distant);
  struct hostent *hp;
  char motif;
  char * message=malloc(lg_msg*sizeof(char));
  int envoi=-1;
/*------------------------création du socket local  page 16 poly ----------------------------------*/
  sock=socket(AF_INET,SOCK_STREAM,0);
  if (sock==-1){
    printf("ouverture du socket impossible");
    exit(1);
    }
/*---------------------------adressage du socket  page 20 poly ----------------------------*/
  memset((char*)&addr_distant,0,sizeof(addr_distant));
  addr_distant.sin_family=AF_INET;  
  addr_distant.sin_port=htons(port);    
  if((hp=gethostbyname(dest))==NULL)
{
    printf("erreur gethostbyname\n");
    exit(1);
}

  memcpy((char*)&(addr_distant.sin_addr.s_addr), hp->h_addr , hp->h_length);
/*---------------------------- demande de connexion -----------------------------------*/
  if (connect(sock,(struct sockaddr *)&addr_distant,sizeof(addr_distant))==-1){
    printf("échec de connexion\n");
    exit(1);
}

/*-----------------------------------transfert de données----------------------------*/
  for (int i=1; i<=nb_message ; i++)
    {
      affichage2(i,message);
      construire_message2(message,'a',lg_msg,i);
      // construire_message(message,'a',lg_msg);
      printf("SOURCE : Envoi n°%d (%d) : [", i, lg_msg);
      afficher_message(message,lg_msg);
     
      
        
      if((sent=write(sock,message,lg_msg))==-1){
	printf("Erreur dans la fct write\n");
        exit(1);
      }
          
              
         
          

    }
/*------------------------------fermeture de connexion page 48 poly ---------------------------*/
if(shutdown(sock,2)==-1)   //le processus ne veut ni recevoir ni emmettre
    {
        printf("probleme dans la fermeture de connexion de tcp  \n");
        exit(1);
    }
/*----------------------destruction du socket page 14 poly ------------------------------------------------------------------------------*/
if (close(sock)==-1)
    {
        printf("Echec de la destrcution du socket local ");
        exit(1);
    }

    printf("Message envoyé \n");
}

/*--------------------------Coté serveur  source=0--------------------------------------------------------------------------------------*/
void serveurTCP(int port , int nb_message, int lg_msg)
{
/*-------------------------------------Déclaration des param----------------------------------------------------------------------------*/
  int sock ;  
  struct sockaddr* addr_distant;
  struct sockaddr_in addr_local;
  int lg_addr_distant=sizeof(addr_distant);
  int lg_addr_local=sizeof(addr_local);
  struct hostent *hp;
  char * message=malloc(lg_msg*sizeof(char));
  int sock2; //utilisé pour le connect (refus de connexion coté serveur)
  int lg_recv=-1;
/*----------------------Création du socket--------------------page 26 poly--------------------------------------------------------------*/
    

   if ((sock=socket(AF_INET,SOCK_STREAM,0))==-1)
    {
      printf("prob dans la création du socket locale \n");
      exit(1);
      
        
    }

   memset((char*)&addr_local, 0 , sizeof(addr_local));
   addr_local.sin_family=AF_INET;
   addr_local.sin_addr.s_addr=INADDR_ANY;     //n'importe quelle adresse IP
   addr_local.sin_port=htons(port);
    
 //association @ socket à la representation interne

   if (bind(sock,(struct sockaddr *)&addr_local, lg_addr_local)==-1)
    {
       printf("prob du bind \n");
       exit(1);
      
       
    }
    
/*-------------verification du nbr de connexions en attentes --------------------*/
   if (listen(sock,80)==-1){
     printf("trop de co en wainting \n");
     exit(1);
   }

    


/*------------cas de refus de connexion ------------------------------------*/
   if ((sock2 = accept(sock,(struct sockaddr*)&addr_distant,&lg_addr_distant))==-1)
    {
      printf("le serveur refuse de se connecter snif \n");
      exit(1);
        
    }
/*---------------reception de donnée pareil que udp ---------------------------*/
   int i=1;
   while(lg_recv!=0)
    {
      lg_recv=read(sock2,message,lg_msg);
      if (lg_recv==-1)
        {
	  printf("Erreur receive from lecture impo \n");
          exit(1);
	  
            
        }

      if (lg_recv!=0)
        {
	  printf("PUITS : Réception n°%d (%d) : [",i,lg_msg);  //mise en forme
          afficher_message(message,lg_recv);
      
        }
      if (i==nb_message)
        {
	  lg_recv=0;
          printf("On a atteint le nombre de messages à recevoir\n");
            
        }

      i++;
      
        
}
/*--------------fermeture du socket -------------------------------*/
    if (close(sock)==-1)
    {
      printf("Echec de fermeture du socket ");
      exit(1);
      
        
    }

    free(message); // à cause du malloc dans la def des para
    
}

/*-------------------------Partie 2 :BAL----------------------------------*/
/*-----------------------Emetteur Bal-------------------------------------*/

void EmetteurBAL(int port, char* dest, int nb_message, int lg_msg, int nBAL)
{
    //Déclarations
    int sock;
    struct sockaddr_in addr_distant ;
    int lg_addr_distant=sizeof(addr_distant);
    struct hostent *hp;
    char motif;
    char * message=malloc(lg_msg*sizeof(char));
    int envoi=-1;
    int lg_pdu=50;
    int lg_recv;
    char*pdu=malloc(lg_pdu*sizeof(char));

//---------------------------------------
//--------Etablissement connexion--------
//---------------------------------------
  

    sprintf(pdu,"0 %d %d %d",nBAL, nb_message,lg_msg);

    /*---------------------Création socket-----------------*/
    if((sock=socket(AF_INET,SOCK_STREAM,0))==-1)
    {
        printf("Erreur à l'ouverture du Socket Stream");
        exit(1);
    }
    /*--------------Construction adresse socket distant----------------*/
    memset((char*)&addr_distant,0,sizeof(addr_distant));
    addr_distant.sin_family=AF_INET;  //Internet
    addr_distant.sin_port=htons(port);       //Numéro de Port

    /*-----------------Affectation IP------------------------*/
    if((hp=gethostbyname(dest))==NULL)
    {
        printf("Erreur de requête IP.\n");
        exit(1);
    }

    memcpy((char*)&(addr_distant.sin_addr.s_addr), hp->h_addr , hp->h_length);

    /*----------------------Demande de connexion---------------------------*/

    if (connect(sock,(struct sockaddr *)&addr_distant,sizeof(addr_distant))==-1)
    {
        perror("Erreur lors de la connexion, en attente de la tentative suivante \n");
        exit(1);
    }
//-----------------------------------------
//----------------Envoi PDU----------------
//-----------------------------------------

    if ((envoi=write(sock,pdu,lg_pdu))==-1)
    {
        printf("Echec de l'envoi du PDU Emetteur (fonction write en défaut)\n");
        exit(1);
    }
//-----------------------------------------
//----------TRANSFERT DE DONNEES-----------
//-----------------------------------------



    for (int i=1; i<=nb_message;i++)
    {
        printf("SOURCE : lettre n°%d (%d) [", i,lg_msg);

        /*------------------Création du message---------------*/
        construire_message2(message,motif,lg_msg,i);
        affichage2(nBAL,message);
        afficher_message(message,lg_msg);

        /*------------------Envoi du message----------------------*/

        if ((envoi=write(sock,message,(lg_msg)))==-1)
        {
            printf("Echec de l'envoi du message (fonction write en défaut)\n");
            exit(1);
        }

    }



    /*-----------Fermeture connexion------------*/

    if(shutdown(sock,2)==-1)
    {
        printf("Erreur à la fermeture de la connexion TCP \n");
        exit(1);
    }

    /*------------------Fermeture Socket-----------------*/
    if (close(sock)==-1)
    {
        printf("Echec de la fermeture du socket distant");
        exit(1);
    }

    free(message);
    free(pdu);
    printf("Envoi effectué avec succès\n");
}



       
/*---------------------------------------------Recepteur BAL------------------------------------------------*/
void RecepteurBAL(int port, char* dest, int nBAL)
  
{
    /*---------- Déclarations--------- */
  
    
    int sock;
    struct sockaddr_in addr_distant;
    int lg_addr_distant = sizeof(addr_distant);
    struct hostent *hp;
    char *message; 
    int envoi = -1;
    int lg_pdu=50;
    int lg_recv=-1;
    int lg;
    int nb;
    char *pdu = malloc(lg_pdu*sizeof(char));

    /*-------Création du socket local à l’aide de la primitive socket(  ,  ,  )---------------------------*/

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        printf("Erreur à l'ouverture du Socket Stream");
        exit(1);
    }

/*------------------------Construction de l’adresse du socket serveur BAL -----------------------------------*/
    memset((char *) &addr_distant, 0, sizeof(addr_distant));
    addr_distant.sin_family = AF_INET;  //Internet
    addr_distant.sin_port = port;       //Numéro de Port

/*------------------------- Affectation IP-------------------------------------- */
    if ((hp = gethostbyname(dest)) == NULL) {
      printf("Erreur de requête IP.\n");
      exit(1);
    }
    memcpy((char *) &(addr_distant.sin_addr.s_addr), hp->h_addr, hp->h_length);

 /*--------Établissement de la connexion TCP avec le serveur du BAL à l’aide de la primitive connect( )--------------*/

    if (connect(sock, (struct sockaddr *) &addr_distant, sizeof(addr_distant)) == -1) {
      printf("Erreur lors de la connexion, en attente de la tentative suivante \n");
      exit(1);
    }

    /*--------------Envoi du PDU-------------------------*/ 

    if ((envoi = write(sock, pdu, lg_pdu)) == -1) 
    {
        printf("Echec de l'envoi du PDU (fonction write en défaut)\n");
        exit(1);
    }
    /*-------------Réception des données-------------------*/

    char *lgmsg=malloc(maxsize* sizeof(char));
    nb=10;
    int n=1;
    lg_recv=1;
    printf(" PUITS : Réception du contenu de la BAL n°%d\n",nBAL);
   
    while(n<=nb)
    {
        if ((lg_recv=read(sock,lgmsg,lg_pdu))==-1)
        {
            printf("Erreur à la réception du PDU de longueur de message\n");
            exit(1);
        }
        sscanf(lgmsg,"%d %d", &lg , &nb);
        if (lg==-1)
        {
            printf("ATTENTION : Pas de courrier à récupérer dans la BAL n°%d\n\n",nBAL);
            exit(0);
        }

        message=malloc(lg*sizeof(char));
        if ((lg_recv=read(sock,message,lg))==-1)
        {
            printf("Erreur à la réception du message\n");
            exit(1);
        }
        printf("PUITS : Réception de la lettre n°%d : [",n);
        afficher_message(message,lg);
        n++;
    }


/*-------------------------------- Fermeture socket-------------------------- */ 

    if(close(sock)==-1)
    {
        printf("Impossible de fermer le socket");
        exit(1);
    }
}
  
/*------------------------------------------Service boite à lettres -------------------------------------------------------*/
//Variable qui donne la longueur max d'un message
int maxsize=9999;
//Définitions des types

typedef struct BAL 
{
    int num ;
    int nb;
    struct LETTRE * l_first ;
    struct LETTRE * l_last ;
    struct LETTRE * l_current ;
    struct BAL * suiv ;
}BAL ;

typedef struct LETTRE 
{
    int num ;
    int lg;
    char *message ;
    struct LETTRE * suiv ;
}LETTRE ;

typedef struct LISTE_BAL
{
    struct BAL * first ;
    struct BAL * last ;
    struct BAL * current ;
    int nb;
}LISTE_BAL;
/*---------------------Création du socket local à l’aide de la primitive socket()--------------------------------*/

    if ((sock=socket(AF_INET,SOCK_STREAM,0))==-1)
    {
        printf("Echec de la création d'un socket local\n");
        exit(1);
    }
/*-------------------------Création d’une adresse locale-------------------------------------------------------------*/ 

    memset((char*)&addr_local, 0 , sizeof(addr_local));
    addr_local.sin_family=AF_INET;
    addr_local.sin_addr.s_addr=INADDR_ANY;
    addr_local.sin_port=port;

/*-----------------Relier cette adresse au socket à l’aide de la primitive bind()-------------------------------------------*/ 

    if (bind(sock,(struct sockaddr *)&addr_local, lg_addr_local)==-1)
    {
        printf("Echec du bind.\n");
        exit(1);
    }
/*----------------Accepter les requêtes de connexion entrantes à l’aide de la primitive accept()----------------------------*/

    if (listen(sock,100)<0)
    {
        printf("Trop de connexions en attentes, échec de la demande\n");
        exit(1);
    }



void main (int argc, char **argv)
{
/* ---------------------------declaration des paramètre-------------------------------------------------------------- */ 
	int c;
	extern char *optarg;
	extern int optind;
	int nb_message = -1; /* Nb de messages à envoyer ou à recevoir, par défaut : 10 en émission, infini en réception */
	int source = -1 ; /* 0=puits, 1=source  , 2=emetteur_bal , 3=recepteur_bal */
	char *dest ;
	int port ;
	int tcp=1 ; /* -ici si tcp=1 on est en tcp et si tcp=0 on est en udp ------- par defaut en tcp */ 
	int lg =30; /* longueur du message par défaut */
	int recep=-1;
	int bal=-1;
	int nBAL=-1;
	
/*-----------------------------------------------Debut---------------------------------------------------------*/ 
	while ((c = getopt(argc, argv, "pn:sul:e:br:")) != -1) {
		switch (c) {
		case 'p':
			if (source == 1) {
				printf("Warning :\n On ne peut être source et puits en même temps, Please enlever une des deux options -s ou -p\n\n");
				exit(1);
			}
			source = 0;
			break;

		case 's':
			if (source == 0) {
				printf("Warning :\n On ne peut être source et puits en même temps, Please enlever une des deux options -s ou -p\n\n");
				exit(1) ;
			}
			source = 1;
			break;
			
		case 'u':
			tcp=0;
			printf("l'envoi se fait via udp\n");
			break;
				       
		case 'n':
			nb_message = atoi(optarg);
			printf("le nombre  de  messages est : %d \n ",nb_message);
			break;
		
		/* case 'b': */
		/*   if (source!=-1) */
		/* 	   { */
                /*                printf("\n On peut pas  utiliser le service de Boite aux lettres et etre dans le puits|source au même time.\n Please veuillez  retirer une des options -e ou -s|-p\n\n"); */
                /*                exit(1); */
		/* 	   } */
		/*   source=3; */
		  
		  
				  
		case 'l':
		        lg=atoi(optarg);
		        printf("la longueur du message est : %d \n ",lg);
			break;
			
	       case 'e' :
		        if (source!=-1)
			   {
                               printf("\n On peut pas  utiliser le service de Boite aux lettres et etre dans le puits|source au même time.\n Please veuillez  retirer une des options -e ou -s|-p\n\n");
                               exit(1);
			   }
			  
               
			if (recep==1)
			  {
			    printf("On ne peut pas être émetteur et récepteur, -r ou -e, pas les deux\n");
			    exit(1);
			  }
			source=2; 
			bal=1;
			recep=0;
			nBAL = atoi(optarg);
			break;

			
		case 'r':
		       if (source!=-1)
			   {
                               printf("\n On peut pas  utiliser le service de Boite aux lettres et etre dans le puits|source au même time.\n Please veuillez  retirer une des options -e ou -s|-p\n\n");
                               exit(1);
			   }
			  
               
			if (recep==0)
			  {
			    printf("On ne peut pas être émetteur et récepteur, -r ou -e, pas les deux\n");
			    exit(1);
			  }
			source=3; 
			bal=1;
			recep=1;
			nBAL = atoi(optarg);
			break;
		      

		default:
			printf("usage: ./tsock [-p|-s][-n ##]\n");
			break;
		}
	       
	}


	if (source == -1) {
		printf("usage: ./tsock [-p|-s][-n ##]\n");
		exit(1) ;
	}
	

	if (source == 1){
		printf("On est dans le source\n");
	}
	else if (source==0) {
		printf("On est dans le puits\n");
	}
	else if (source==2) {
		printf("On est émetteur dans le bal \n");
	}
	if (tcp==1)
	  printf("Protocole de transport : TCP | ");
      
        else
            printf("Protocole de transport : UDP | ");
				       
	if (nb_message != -1) {
		if (source == 1)
			printf("nb de tampons à envoyer : %d\n", nb_message);
		else
			printf("nb de tampons à recevoir : %d\n", nb_message);
	} else {
		if (source == 1) {
			nb_message = 10 ;
			printf("nb de tampons à envoyer = 10 par défaut\n");
		} else
		printf("nb de tampons à envoyer = infini\n");

	}
	

	dest=argv[argc-2];
	if (source==1) {
	  printf("le nom de la machine est: %s \n",dest);
	}
	  
	
        port=atoi(argv[argc-1]);
	printf("le numéro de port est : %d \n ",port);
    

/*------------------------Envoi du message en UDP ----------------------------------*/				       
if (source==1 & tcp ==0){
       envoi_UDP(port,nb_message,lg,dest);
 }
 
/*------------------------Reception UDP---------------------*/
 else if (source==0 & tcp==0) {
    reception_UDP(port,nb_message,lg);
 }
 
 /*------------------------Envoi du message en TCP ----------------------------------*/
 else if (source==1 & tcp==1) {
   recepteurTCP(port,nb_message,lg,dest);
 }
 
/*--------------------------Reception via TCP ------------------------------------*/
 else if (source==0 & tcp==1) {
   serveurTCP(port,nb_message,lg);
}
 
/*-------------------------------Second part ---------------------------------*/
/*-------------------------------Bal EMETTEUR ------------------------------*/
 else if (bal==1 & recep==0) {
   EmetteurBAL(port, dest,nb_message,lg, nBAL); 
}
/*-------------------------------Bal RECEPTEUR ------------------------------*/
 else if (bal==1 & recep==1) {
  RecepteurBAL(port, dest, nBAL);
 
}
  
}
