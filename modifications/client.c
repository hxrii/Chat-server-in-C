#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

#define BUFFER_SIZE 1024

void *receive_message(void *arg)
{
    int client_socket = *(int *)arg;

    char message[BUFFER_SIZE];
    int read_size;

    while ((read_size = recv(client_socket, message, BUFFER_SIZE, 0)) > 0)
    {
        message[read_size] = '\0';
        printf("\n%s\n", message);
    }

    return NULL;
}

int main(int argc, char *argv[])
{
    int client_socket;
    struct sockaddr_in server_address;

    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8888);
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
    {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    pthread_t receive_thread;

    if (pthread_create(&receive_thread, NULL, receive_message, &client_socket) != 0)
    {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }
    
   
    
    

    char message[BUFFER_SIZE];
     
    int message_len;
    printf("Enter your name: ");
    scanf("%s",message);
    message_len = strlen(message);
    send(client_socket, message, message_len, 0);
     
    

    while (fgets(message, BUFFER_SIZE, stdin) != NULL)
    {
        message_len = strlen(message);

        if (send(client_socket, message, message_len, 0) != message_len)
        {
            perror("send");
            exit(EXIT_FAILURE);
        }

        if (strcmp(message, "/exit\n") == 0 || strcmp(message, "/quit\n") == 0 || strcmp(message, "/part\n") == 0)
        {
            break;
        }
    }

    close(client_socket);

    return 0;
}
