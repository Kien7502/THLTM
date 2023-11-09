#include <stdio.h> 
#include <strings.h> 
#include <sys/types.h> 
#include <arpa/inet.h> 
#include <sys/socket.h> 
#include<netinet/in.h> 
#include <string.h>
#include <ctype.h>
#include<stdlib.h>
#define MAXLINE 1000 
#include <stdbool.h>
#include <stdio.h>
char isSignIn[100];
struct User
{
    char username[100];
    char password[100];
    int status;
    // 1: active, 0: blocked, 2: idle
    struct User *next;
};

struct User *readUserInfoFromFile()
{
    FILE *file = fopen("nguoidung.txt", "r");

    if (!file)
    {
        printf("Error opening file.\n");
        exit(EXIT_FAILURE);
    }

    struct User *head = NULL;
    struct User *tail = NULL;
    char username[100];
    char password[100];
    int status;

    while (fscanf(file, "%s %s %d", username, password, &status) == 3)
    {
        struct User *newUser = (struct User *)malloc(sizeof(struct User));
        if (!newUser)
        {
            printf("Memory allocation failed.\n");
            exit(EXIT_FAILURE);
        }
        strcpy(newUser->username, username);
        strcpy(newUser->password, password);
        newUser->status = status;
        newUser->next = NULL;

        if (head == NULL)
        {
            head = newUser;
            tail = newUser;
        }
        else
        {
            tail->next = newUser;
            tail = newUser;
        }
    }
    fclose(file);
    return head;
}

void updateFile(struct User *head)
{
    FILE *file = fopen("nguoidung.txt", "w");

    if (!file)
    {
        printf("Error opening file.\n");
        exit(EXIT_FAILURE);
    }

    struct User *current = head;
    while (current != NULL)
    {
        fprintf(file, "%s %s %d\n", current->username, current->password, current->status);
        current = current->next;
    }

    fclose(file);
}


int main(int argc, char *argv[]) 
{ 
	struct User *head = readUserInfoFromFile();
	if(argc==1){
		printf("Please input port number\n");
		return 0;
	}
	char *port_number = argv[1];
	int port = atoi(port_number);

	int listenfd, len; 
	struct sockaddr_in servaddr, cliaddr; 
	bzero(&servaddr, sizeof(servaddr)); 
	// Create a UDP Socket 
	listenfd = socket(AF_INET, SOCK_DGRAM, 0);		 
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
	servaddr.sin_port = htons(port); 
	servaddr.sin_family = AF_INET; 
	// bind server address to socket descriptor 
	bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)); 
	while(1) {
	// Read Input
	char buffer[100];
	char input[100];
	len = sizeof(cliaddr); 
	int n1 = recvfrom(listenfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&cliaddr,&len);
	buffer[n1] = '\0';
	strcpy(input,buffer);

	// Output
	char output[100];
	strcpy(output,"User: ");
	sendto(listenfd, output, MAXLINE, 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr)); 

	// Read Input
	len = sizeof(cliaddr); 
	n1 = recvfrom(listenfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&cliaddr,&len);
	buffer[n1] = '\0';
	strcpy(input,buffer);

	char username[100], password[100], nothing[100];
    strcpy(username ,input);
    struct User *current = head;
    while (current != NULL)
    {
        if (strcmp(current->username, username) == 0)
        {
            if (current->status == 0)
            {
                // Output
				strcpy(output,"Account is blocked.\n");
				sendto(listenfd, output, MAXLINE, 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));
				break;
            }
            int attempts = 0;
            while (attempts < 3)
            {
				// Output
				strcpy(output,"\nEnter password: ");
				sendto(listenfd, output, MAXLINE, 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr)); 
				// Read Input
				len = sizeof(cliaddr); 
				n1 = recvfrom(listenfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&cliaddr,&len);
				buffer[n1] = '\0';
				strcpy(password,buffer);
                if (strcmp(current->password, password) == 0)
                {
                    if (current->status == 1)
                    {
                        strcpy(isSignIn, username);
						// Output
						strcpy(output,"\nOK\n\nEnter new password or 'bye':");
						sendto(listenfd, output, MAXLINE, 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr)); 
						// Read Input
						len = sizeof(cliaddr); 
						n1 = recvfrom(listenfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&cliaddr,&len);
						buffer[n1] = '\0';
						strcpy(nothing,buffer);
						break;
					}
                    else
                    {
						// Output
						strcpy(output,"account not ready. \nEnter any character to exit:");
						sendto(listenfd, output, MAXLINE, 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr)); 
						// Read Input
						len = sizeof(cliaddr); 
						n1 = recvfrom(listenfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&cliaddr,&len);
						buffer[n1] = '\0';
						strcpy(nothing,buffer);
					}
                    break;
                }
                else
                {
                    attempts++;
					// Output
					strcpy(output,"Password is incorrect. \nEnter any character to continue:");
					sendto(listenfd, output, MAXLINE, 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr)); 
					// Read Input
					len = sizeof(cliaddr); 
					n1 = recvfrom(listenfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&cliaddr,&len);
					buffer[n1] = '\0';
					strcpy(nothing,buffer);
				}
            }
            if (attempts >= 3)
            {
                current->status = 0; // Set status to blocked
                updateFile(head);
				// Output
				strcpy(output,"Too many incorrect attempts. Account blocked");
				sendto(listenfd, output, MAXLINE, 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr)); 
                break;
            }
        }
        current = current->next;
    }

	if (strcmp(isSignIn, "") != 0)
    {
        if(strcmp(nothing, "bye") != 0){
			struct User *current = head;
			while (current != NULL)
			{
				if (strcmp(current->username, isSignIn) == 0)
				{
					int change = 0;

					char only_number[1000];
					char only_string[1000];
					strcpy(only_string,nothing);
					int x=0;
					strcpy(only_number,nothing);
					int y=0;

					int m = 0;
					for(int i=0; i<100 ;i++){
						char ch = only_number[i];
						if (ch == '\0') break;
						if(ch >= '0' && ch <= '9'){
							only_number[y] = ch;
							y++;
						}
						else if ((ch >= 'a' && ch <= 'z')||(ch == ' ')){
							only_string[x] = ch;
							x++;
						}
						else{
							sendto(listenfd, "Error", MAXLINE, 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));
							change = 1;
							break; 
						}
					}
					
					if (change != 1){
						only_number[y] = '\0'; 
						only_string[x] = '\0'; 

						char encode[] = "\n";
						strcat(encode , only_number);
						strcat(encode , "\n");
						strcat(encode , only_string);
						strcpy(output, encode);
						sendto(listenfd, output, MAXLINE, 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr)); 

						strcpy(current->password, nothing);
						updateFile(head);
						printf("Password changed successfully!\n");
					}
				
				}
				current = current->next;
			}
		}
    }
	char bye[] = "Goodbye ";
	// Output
	strcat(bye,isSignIn);
	strcpy(output, bye);
	sendto(listenfd, output, MAXLINE, 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr)); 
	}
} 
