#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <strings.h>
#include <unistd.h>
#include <stdlib.h>

struct sockaddr_in servaddr;
int list_socket[10];

// Tableau 2 lignes, 3 colonnes
int maxPartyServeur = 3;



int multi[2][3/*maxPartyServeur*/];


void addConnection(int fd_client)
{
  for (int i = 0; i < 10; i++)
  {
    if (list_socket[i] == 0)
    {
      list_socket[i] = fd_client;
      break;
    }
  }
}

int checkAlreadyConnection(int fd)
{
  for (int i = 0; i < 10; i++)
  {
    if (list_socket[i] == fd)
    {
      return 1;
    }
  }
  return 0;
}

void *setUpList()
{
  for (int i = 0; i < 10; i++)
  {
    list_socket[i] = 0;
  }
}

void *setUpMulti()
{
  for (int i = 0; i < 2; i++)
  {
    for (int j = 0; j < 3; j++)
    {
      multi[i][j] = 0;
    }
  }
}

int myAtoi(char *str)
{
  if (strcmp(str, "1\0") == 0)
    return 1;
  else if (strcmp(str, "2\0") == 0)
    return 2;
  else if (strcmp(str, "3\0") == 0)
    return 3;
  else
    return 0;
}

int isPartyFull(int numParty)
{
  if (multi[0][numParty] != 0 && multi[1][numParty] != 0)
    return 1;
  else
    return 0;
}


int main()
{
    printf("Server Online\n");
    printf("Navy Battle\n");


    setUpList();
    setUpMulti();

    /*printf("Set Up ListSocket:\n");
    printf("ListSocket[0]: %d\n", list_socket[0]);
    printf("ListSocket[1]: %d\n", list_socket[1]);
    printf("ListSocket[2]: %d\n", list_socket[2]);*/

    int isSettingDone = 0;
    char str_player1[100];
    char str_player2[100];
    int listen_fd;
    fd_set rdfs;


    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    list_socket[0] = listen_fd;

    //Settings server
    bzero( &servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htons(INADDR_ANY);
    servaddr.sin_port = htons(8088);

    FD_ZERO(&rdfs);
    FD_SET (listen_fd, &rdfs);


    //Binding
    bind(listen_fd, (struct sockaddr *) &servaddr, sizeof(servaddr));

    listen(listen_fd, 20);

    //conn_player1_fd = accept(listen_fd, (struct sockaddr*) NULL, NULL);

    printf("Waiting ...\n");

    while(1)
    {
        printf("LOOP While\n");
        printf("Multi[0][0]: %d\n", multi[0][0]);
        printf("Multi[1][0]: %d\n", multi[1][0]);

        printf("Multi[0][1]: %d\n", multi[0][1]);
        printf("Multi[1][1]: %d\n", multi[1][1]);

        printf("Multi[0][2]: %d\n", multi[0][2]);
        printf("Multi[1][2]: %d\n", multi[1][2]);


        int conn_player_fd = 0;
        if (select(FD_SETSIZE, &rdfs, NULL, NULL, NULL) > 0)
        {
          printf("New Client Connected\n");
          conn_player_fd = accept(listen_fd, (struct sockaddr*) NULL, NULL);
          if (checkAlreadyConnection(conn_player_fd) == 0)
          {
            addConnection(conn_player_fd);
          }

          //Premiere discussion du client avec le serveur
          //Reinitialise str

          write(conn_player_fd, "Choose a party plz.\n", strlen("Choose a party plz.") + 1);


          int isInMenu = 0;
          while (isInMenu == 0)
          {
            bzero(str_player1, 100);
            read(conn_player_fd, str_player1, 100);

            printf("Player choose party: %s\n", str_player1);
            int numParty = atoi(str_player1) - 1;

            if (numParty >= 0 && numParty < maxPartyServeur)
            {
              //Connection des client vers les salles de partie
              if (multi[0][numParty] == 0)
              {
                multi[0][numParty] = conn_player_fd;
                isInMenu = 1;
              }
              else if(multi[1][numParty] == 0)
              {
                multi[1][numParty] = conn_player_fd;
                isInMenu = 1;
              }
              else
              {
                printf("Error: PARTY FULL\n");

                write(conn_player_fd, "Party full. Please choose another party.\n", strlen("Party full. Please choose another party.") + 1);
              }

              if (isPartyFull(numParty) == 1)
              {
                //TO DO Si la salle contient 2 joueurs => lancement d'un thread du jeux

                // Les FileDescriptor des clients sont dans multi[][]
                // TO DO: effacer les filedescriptor de multi
                printf("GAME START\n");

                multi[0][numParty] = 0;
                multi[1][numParty] = 0;
              }
              else
              {
                write(conn_player_fd, "Waiting for another player.\n", strlen("Waiting for another player.") + 1);
              }

            }
            else
            {
              printf("Error this party doesn't exist\n");
              write(conn_player_fd, "Error this party doesn't exist, please select another party.\n", strlen("Error this party doesn't exist, please select another party.") + 1);
            }
          }



        }

        printf("END Select\n");
        printf("ListSocket[0]: %d\n", list_socket[0]);
        printf("ListSocket[1]: %d\n", list_socket[1]);
        printf("ListSocket[2]: %d\n", list_socket[2]);

        //conn_player2_fd = accept(listen_fd, (struct sockaddr*) NULL, NULL);




        //bzero(str_player2, 100);

        /*read(conn_player2_fd, str_player2, 100);
        printf("Serveur listen player 2: %s\n", str_player2);
        write(conn_player2_fd, str_player2, strlen(str_player2) + 1);*/
    }
}
