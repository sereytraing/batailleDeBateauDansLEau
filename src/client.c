/*
/	Etude d'un serveur ECHO
/	Code du client
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <strings.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc,char **argv)
{
    int sockfd, n;
    char sendline[100];
    char recvline[100];
    struct sockaddr_in servaddr;

    printf("Welcome Client\n");
    printf("Hello, welcome to Navy Battle Royal\n");


    sockfd=socket(AF_INET,SOCK_STREAM,0);
    bzero(&servaddr,sizeof servaddr);

    servaddr.sin_family=AF_INET;
    servaddr.sin_port=htons(8088);

    //TO Set ip address
    inet_pton(AF_INET,"127.0.0.1",&(servaddr.sin_addr));

    connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));

    while(1)
    {
        bzero( sendline, 100);
        bzero( recvline, 100);

        if (read(sockfd,recvline,100) != -1) {
          printf("Server say: %s\n",recvline);
        }

        //Si le message contient OPT- c'est que le serveur attend un output de la part du client
        if (strstr(recvline, "OTP-") != NULL) {
          printf("%s", "Waiting for input: ");
          fgets(sendline,100,stdin); /*stdin pour l'input standard*/
          write(sockfd,sendline,strlen(sendline)+1);
        }
    }

}
