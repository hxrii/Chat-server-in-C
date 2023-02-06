#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

int total=0;
int sf,sp,ia;

struct client_data
{
    int socket;
    char name[BUFFER_SIZE];
};
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;
struct client_data clients[MAX_CLIENTS];


void *handle_client(void *arg)
{   int flag=0;
    struct client_data *client = (struct client_data *)arg;

    char message[BUFFER_SIZE];
    int read_size;
    
    
    
    
    if(flag==0){
        int sin_family=sf,sin_port=sp,internet_Add=ia;
        read_size = recv(client->socket, message, BUFFER_SIZE, 0);
        message[read_size] = '\0';
        strcat(client->name,message);
        
        printf("%s has joined!!\n", client->name);
        printf("Port Number: %d\n", sin_port );
        printf("sin_family: %d\n", sin_family );
        printf("Internet Address: %d\n", internet_Add );
        
        
        
        
        
        total++;
        printf("Total # of active clients : %d\n\n", total);
        
        
        
     }
     flag++;



    while ((read_size = recv(client->socket, message, BUFFER_SIZE, 0)) > 0)
    {
        message[read_size] = '\0';
        
        
        
        
        
        
        
        
        
       

        if (strcmp(message, "/exit\n") == 0 || strcmp(message, "/quit\n") == 0 || strcmp(message, "/part\n") == 0)
        {
            break;
        }
        
        
        
        
        
        

        printf("%s: %s\n", client->name, message);
      //  printf("%s: %s\n", client->name, message);
        

        pthread_mutex_lock(&clients_mutex);

        char *message2 = malloc(strlen(client->name) + strlen(message) + 2);
        strcat(message2, client->name);
        strcat(message2, ": ");
        strcat(message2, message);

        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            if (clients[i].socket != 0 && clients[i].socket != client->socket)
            {
                send(clients[i].socket, message2, strlen(message2), 0);
            }
        }

        pthread_mutex_unlock(&clients_mutex);
    }

    pthread_mutex_lock(&clients_mutex);

    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i].socket == client->socket)
        {
            clients[i].socket = 0;
            break;
        }
    }

    pthread_mutex_unlock(&clients_mutex);

    printf("%s has left!!!\n", client->name);

    close(client->socket);
    total--;
    printf("Total # of active clients : %d\n\n", total);

    return NULL;
}

int main(int argc, char *argv[])
{
    printf("Chatroom server started Successfully...\n\n");
    int server_socket, client_socket;
    struct sockaddr_in server_address, client_address;
    socklen_t client_len;
    pthread_t client_thread;
    // printf("Starting chatroom server...\n");

    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8888);
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, MAX_CLIENTS) == -1)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    memset(clients, 0, sizeof(clients));

    int next_client = 0;

    while (1)
    {
        client_len = sizeof(client_address);
        if ((client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_len)) == -1)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        		
        		
        			sf = client_address.sin_family ;
        			sp =  client_address.sin_port;
        			ia = client_address.sin_addr.s_addr;
        			
        
                          // printf("Address: %d\n", client_address.sin_family );
                          // printf("Address: %d\n", client_address.sin_port );
                          // printf("Address: %d\n", client_address.sin_addr.s_addr );
                           
                           
                           
                           

        clients[next_client].socket = client_socket;
        
        
         char message[BUFFER_SIZE];
    	 int read_size;
        //sprintf(clients[next_client].name, "Client %d", next_client + 1);
       

        if (pthread_create(&client_thread, NULL, handle_client, &clients[next_client]) != 0)
        {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }

        next_client = (next_client + 1) % MAX_CLIENTS;
    }

    close(server_socket);

    return 0;
}
