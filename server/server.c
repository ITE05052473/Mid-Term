#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

int guess_num[4] = {0,0,0,0};
int answer[4] = {0,0,0,0};
int recv_message[4] = {-1,-1,-1,-1};
int result[2] = {0,0};

void GuessNum()
{  
    int i,j;
    int flag = 0;
    srand(time(NULL));
    for (i = 0;i < 4;i ++)
    {
        guess_num[i] = rand() % 10;
    }
}

void messageToAnswer(){
    int i;
    for (i = 0;i < 4;i ++)
    {
        answer[i] = (char)recv_message[i]-'0';
    }
}

int isSuccess(){
    int flag = 0;
    if(result[0] == 4){
        flag = 1;
    } 
    return flag;
}

void getResult(){
    result[0] = 0;
    result[1] = 0;
    for(int i = 0;i<4;i++){
        if(answer[i] == guess_num[i]){
            result[0]++;
            continue;
        }
        for(int j = 0;j<4;j++){
            if(answer[i] == guess_num[j]){
                result[1] ++;
                break;
            }
        }
    }
}

int main(int atgc, char *argv[])
{
    int socket_desc, new_socket, c, pid;
    struct sockaddr_in server, client;
    char *reply_message;
    char recv_message2[4];
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1)
    {
        printf("Connot create socket");
    }
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(8888);
    if (bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        puts("bind failed");
        return 1;
    }
    puts("bind done");
    listen(socket_desc, 3);
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);

    while ((new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t *)&c)))
    {
        if (new_socket < 0)
        {
            perror("accept failed");
            return 1;
        }
        puts("Connection accepted");

	pid=fork();
	if(pid<0)
	{
	    perror("Error on pork");
	    exit(1);
	}
	if(pid==0)
	{
	    close(socket_desc);
	    printf("New connection, my process id is %d\n", getpid()); 
	    
	    guess:while (1)
        {   
            GuessNum();
            puts("number:");
            for(int i = 0;i < 4;i++){
                printf("%d",guess_num[i]);
            }
            printf("\n");
            if (recv(new_socket, recv_message2, 4, 0) < 0)
            {
                perror("recv error");
                break;
            }
            else
            {
		int i;
		for (i = 0;i < 4;i ++)
		{
		    recv_message[i] = recv_message2[i];
		}
                puts("Data Received:");
                messageToAnswer();
                puts("theAnswer:");
                for(int i = 0; i < 4;i++){
                    printf("%d",answer[i]);
                }
                printf("\n");
                getResult();
                if(isSuccess())
                {
                    reply_message = "success";
                    write(new_socket,reply_message,strlen(reply_message));
                    puts("success");
                    goto guess;
                }
                else{
                    reply_message = (char *)malloc(sizeof(char)*4);
                    sprintf(reply_message,"%dA%dB",result[0],result[1]);
                    write(new_socket,reply_message,4);
                    puts("theResult:");
                    puts(reply_message);
                    free(reply_message);
                }
            }
        }

	    exit(0); 
	}
	else
	{
	     close(new_socket);
	}


    }
    return 0;
}
