/*
/	Etude d'un serveur ECHO
/	Code du serveur
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <strings.h>
#include <unistd.h>

struct sockaddr_in servaddr;

int main()
{
    printf("Server Online\n");
    printf("Navy Battle\n");

    char multi[5][2];

    int isSettingDone = 0;
    char str_player1[100];
    char str_player2[100];
    int listen_fd, conn_player1_fd, conn_player2_fd;


    listen_fd = socket(AF_INET, SOCK_STREAM, 0);

    //Settings server
    bzero( &servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htons(INADDR_ANY);
    servaddr.sin_port = htons(8088);





    //Binding
    bind(listen_fd, (struct sockaddr *) &servaddr, sizeof(servaddr));

    listen(listen_fd, 2);

    //conn_player1_fd = accept(listen_fd, (struct sockaddr*) NULL, NULL);



    while(1)
    {
        while (isSettingDone == 0)
        {
          conn_player1_fd = accept(listen_fd, (struct sockaddr*) NULL, NULL);
          //conn_player2_fd = accept(listen_fd, (struct sockaddr*) NULL, NULL);

          isSettingDone = 1;
        }
        printf("Waiting ...\n");


        //Reinitialise str
        bzero(str_player1, 100);
        bzero(str_player2, 100);

        read(conn_player1_fd, str_player1, 100);
        printf("Serveur listen player 1: %s\n", str_player1);
        write(conn_player1_fd, str_player1, strlen(str_player1) + 1);

        read(conn_player2_fd, str_player2, 100);
        printf("Serveur listen player 2: %s\n", str_player2);
        write(conn_player2_fd, str_player2, strlen(str_player2) + 1);
    }
}
