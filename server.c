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
 
int main()
{
    printf("SERVEUR SERVEUR");
    char str[100];
    int listen_fd, conn_fd;
 
    struct sockaddr_in servaddr;
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
 
    bzero( &servaddr, sizeof(servaddr));
 
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htons(INADDR_ANY);
    servaddr.sin_port = htons(8081);
 
    bind(listen_fd, (struct sockaddr *) &servaddr, sizeof(servaddr));
    
    listen(listen_fd, 10);
 
    conn_fd = accept(listen_fd, (struct sockaddr*) NULL, NULL);
    
    while(1)
    {
        bzero( str, 100);
        read(conn_fd,str,100);
        printf("Echoing back - %s",str);
        write(conn_fd, str, strlen(str)+1);
    }
}
