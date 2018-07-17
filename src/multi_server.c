#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <strings.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

pthread_t gamingThreads[3];
struct sockaddr_in servaddr;
int list_socket[10];

// Tableau 2 lignes, 3 colonnes
int maxPartyServeur = 3;

int multi[2][3/*maxPartyServeur*/];

//---------------Jeu----------------//
//Print le tableau des essaies du joueur en cours
void printTab(int size, int tab[size][size], int fd, int joueur) {
    printf("TAB joueur %d\n", joueur);
    char string[100] = "";
    char integer[5] = "";
    sprintf(string, "%s%s", string ,"\n");
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            sprintf(integer, "%d", tab[j][i]);
            printf("%d", tab[j][i]);
            sprintf(string, "%s%s", string ,integer);
        }
        printf("\n");
        sprintf(string, "%s%s", string ,"\n");
    }
    write(fd, string, strlen(string)+1);
}

//Verifie si le joueur touche un bateau pendant son tour
void verifyData(int size, int tab[2][2][size][size], int x, int y, int joueur, int fd) {
    if (tab[(joueur+1)%2][0][y][x] == 1) {
        tab[(joueur+1)%2][0][y][x] = 0;
        tab[joueur][1][y][x] = 1;
        write(fd, "Touché\n", strlen("Touché") + 1);
    } else {
        tab[joueur][1][y][x] = -1;
        write(fd, "Rien\n", strlen("Rien") + 1);
    }
    printTab(size, tab[joueur][1], fd, joueur);
}

//Verifie si tout les bateau d'un joueur on été touché
int emptymap(int size, int tab[size][size]) {
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            if (tab[i][j] == 1) {
                return 0;
            }
        }
    }
    return 1;
}

//Thread principal du jeu
void *thread_gaming(void *arg) {
    int party = *((int*)arg);
    printf("Nous sommes dans le thread.\n");
    write(multi[0][party], "Hi player 1.\n", strlen("Hi player 1.") + 1);
    write(multi[1][party], "Hi player 2.\n", strlen("Hi player 2.") + 1);
    int size = 5;
    int tab[2][2][size][size];
    char coordonnex[100];
    char coordonney[100];
    int cpt = 0;
    int currentplayer = 0;
    //Remplissage des grilles de jeu
    //Tab[0] j1    tab[1] j2
    //tab[x][0] bateaux tab[x][1] essaies
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            tab[0][0][i][j] = (rand()%2) ? 0 : 1;
            tab[1][0][i][j] = (rand()%2) ? 0 : 1;
            tab[0][1][i][j] = 0;
            tab[1][1][i][j] = 0;
        }
    }

    while (!emptymap(size, tab[0][0]) && !emptymap(size, tab[1][0])) {
      bzero(coordonnex, 100);
      bzero(coordonney, 100);
      currentplayer = cpt%2;
      write(multi[currentplayer][party], "OTP- Tappez x\n", strlen("OTP- Tappez x") + 1);
      read(multi[currentplayer][party], coordonnex, 100);
      write(multi[currentplayer][party], "OTP- Tappez y\n", strlen("OTP- Tappez y") + 1);
      read(multi[currentplayer][party], coordonney, 100);
      verifyData(size, tab, atoi(coordonnex), atoi(coordonney), currentplayer, multi[currentplayer][party]);
      cpt++;
    }

    write(emptymap(size, tab[0][0]) ? multi[0][party] : multi[1][party], "Vous avez perdu\n", strlen("Vous avez perdu") + 1);
    write(!emptymap(size, tab[1][0]) ? multi[0][party] : multi[1][party], "Vous avez gagne\n", strlen("Vous avez gagne") + 1);

    multi[0][party] = 0;
    multi[1][party] = 0;

    pthread_exit(NULL);
}
//----------------PUJEU------------//

//----------------SERV-------------//
void addConnection(int fd_client) {
  for (int i = 0; i < 10; i++) {
    if (list_socket[i] == 0) {
      list_socket[i] = fd_client;
      break;
    }
  }
}

int checkAlreadyConnection(int fd) {
  for (int i = 0; i < 10; i++) {
    if (list_socket[i] == fd) {
      return 1;
    }
  }
  return 0;
}

void *setUpList() {
  for (int i = 0; i < 10; i++) {
    list_socket[i] = 0;
  }
}

void *setUpMulti() {
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 3; j++) {
      multi[i][j] = 0;
    }
  }
}

int myAtoi(char *str) {
  if (strcmp(str, "1\0") == 0)
    return 1;
  else if (strcmp(str, "2\0") == 0)
    return 2;
  else if (strcmp(str, "3\0") == 0)
    return 3;
  else
    return 0;
}

int isPartyFull(int numParty) {
  if (multi[0][numParty] != 0 && multi[1][numParty] != 0)
    return 1;
  else
    return 0;
}


int main() {
    printf("Server Online\n");
    printf("Navy Battle\n");

    setUpList();
    setUpMulti();

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

    while(1) {
        printf("LOOP While\n");
        printf("Multi[0][0]: %d\n", multi[0][0]);
        printf("Multi[1][0]: %d\n", multi[1][0]);

        printf("Multi[0][1]: %d\n", multi[0][1]);
        printf("Multi[1][1]: %d\n", multi[1][1]);

        printf("Multi[0][2]: %d\n", multi[0][2]);
        printf("Multi[1][2]: %d\n", multi[1][2]);

        int conn_player_fd = 0;
        if (select(FD_SETSIZE, &rdfs, NULL, NULL, NULL) > 0) {
          printf("New Client Connected\n");
          conn_player_fd = accept(listen_fd, (struct sockaddr*) NULL, NULL);
          if (checkAlreadyConnection(conn_player_fd) == 0) {
            addConnection(conn_player_fd);
          }

          //Premiere discussion du client avec le serveur
          //Reinitialise str

          write(conn_player_fd, "OTP- Choose a party plz.\n", strlen("OTP- Choose a party plz.") + 1);

          int isInMenu = 0;
          while (isInMenu == 0) {
            bzero(str_player1, 100);
            read(conn_player_fd, str_player1, 100);

            printf("Player choose party: %s\n", str_player1);
            int numParty = atoi(str_player1) - 1;

            if (numParty >= 0 && numParty < maxPartyServeur) {
              //Connection des client vers les salles de partie
              if (multi[0][numParty] == 0) {
                multi[0][numParty] = conn_player_fd;
                isInMenu = 1;
              } else if(multi[1][numParty] == 0) {
                multi[1][numParty] = conn_player_fd;
                isInMenu = 1;
              } else {
                printf("Error: PARTY FULL\n");

                write(conn_player_fd, "OTP- Party full. Please choose another party.\n", strlen("OTP- Party full. Please choose another party.") + 1);
              }

              if (isPartyFull(numParty) == 1) {
                //TO DO Si la salle contient 2 joueurs => lancement d'un thread du jeux

                // Les FileDescriptor des clients sont dans multi[][]
                // TO DO: effacer les filedescriptor de multi
                void *pparty = &numParty;
                if(pthread_create(&gamingThreads[numParty], NULL, thread_gaming, pparty) == -1) {
	                   perror("pthread_create");
                } else {
                  printf("GAME START\n");
                }
              }
              else {
                write(conn_player_fd, "Waiting for another player.\n", strlen("Waiting for another player.") + 1);
              }

            } else {
              printf("Error this party doesn't exist\n");
              write(conn_player_fd, "OTP- Error this party doesn't exist, please select another party.\n", strlen("OTP- Error this party doesn't exist, please select another party.") + 1);
            }
          }
        }
    }
}
//----------------PUSERV-----------//
