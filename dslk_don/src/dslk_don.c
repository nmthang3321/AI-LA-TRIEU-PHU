
#include <sys/socket.h>
#include <sys/select.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <malloc.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <errno.h>
#define FILENAME "my_file.txt"
char NickName[5][50];
char *str;
struct sockaddr_in serverAddress;
struct User{
    char Name[10];
    int CheckTrue;
    int Next;
};
struct User listUser[10];



void InitListUser(){
    int i;
    for(i=0;i<10; i++ ){
        memset(listUser[i].Name, '\0', sizeof(listUser[i].Name));
        listUser[i].CheckTrue=0;
        listUser[i].Next=0;
    }

}
//===============================================================
int CheckUserName(char *Name){
    int i;
    for(i=0; i<10; i++){
        if(strcmp(listUser[i].Name, Name)==0){
            return 1;
        }
    }
    return 0;
}



//================================================================
//READ FILE
char *FileData (int Line){
  int i;
  char *StrBuff = NULL;
  size_t StrBuffSize = 0;
  ssize_t LineSize;
  FILE *fp = fopen(FILENAME, "r");
  for(i=1;i<=Line;i++){
      LineSize =getline(&StrBuff, &StrBuffSize, fp);
  }
  fclose(fp);
  return (StrBuff);
}
//===============================================
int client[10], sizeMaxClient=10;

void ClearClient(){
    int i;
    for (i=0; i<sizeMaxClient; i++){
        client[i]=0;
    }
}

int main(){
    int check=0;
    InitListUser();
    struct sockaddr_in addresServer;
    struct sockaddr_in addressClient;

    int valueOfSocket;
    int valueOfBind;
    int valueOfListen;
    int valueOfAccept;
    int valueOfShutdown;

    socklen_t addressClient_Len;

    fd_set listSocket;
    int sizeFD;

    valueOfSocket=socket(AF_INET, SOCK_STREAM, 0);
    if (valueOfSocket<0){
        printf("Error create server\n");
        return -1;
    }
    else{
        printf("Create server successful\n");
    }
    memset(&addresServer, '\0', sizeof(addresServer));

    addresServer.sin_port=htons(5000);
    addresServer.sin_family=AF_INET;
    inet_aton("192.168.81.12", &addresServer.sin_addr.s_addr);

    valueOfBind=bind(valueOfSocket,(struct sockaddr*)&addresServer, sizeof(addresServer));
    if (valueOfBind==0){
        printf("Bind is successful\n");
    }
    else {
        printf("Error bind\n");
        perror("BIND: \n");
        goto CLOSE;
    }

    valueOfListen=listen(valueOfSocket, 3);
    if (valueOfListen==0){
        printf("Listening...\n");
    }
    else {
        printf("false\n");
        goto CLOSE;
    }

    int i;
    ClearClient();
    while(1){

        FD_ZERO(&listSocket);
        FD_SET(valueOfSocket, &listSocket);
        sizeFD=valueOfSocket;

        for (i=0; i<sizeMaxClient; i++){

            if (client[i]!=0){
                FD_SET(client[i], &listSocket);
            }

            if (sizeFD < client[i]){
                sizeFD=client[i];
            }
        }

        select(sizeFD+1, &listSocket, NULL, NULL, NULL);

        if (FD_ISSET(valueOfSocket, &listSocket)){

            addressClient_Len=sizeof(struct sockaddr_in);
            valueOfAccept=accept(valueOfSocket, (struct sockaddr*)&addressClient, &addressClient_Len);

            if (valueOfAccept > 0){
                printf("Accept successfully IP: %s, Socket=%d\n", inet_ntoa(addressClient.sin_addr),valueOfAccept);
                FD_SET(valueOfAccept, &listSocket);
                char nickname[30];
                memset(nickname, '\0', 30);
                str="Input User Name:\n";
                send(valueOfAccept, str, strlen(str), 0);

                select(valueOfAccept+1, &listSocket, NULL, NULL, NULL);
                if(FD_ISSET(valueOfAccept, &listSocket)){
                    str=(char*)malloc(sizeof(char)*100);
                    memset(str, '\0', 100);
                    recv(valueOfAccept, nickname, sizeof(nickname), 0);
                    CheckUserName(nickname);
                }

                while (CheckUserName(nickname)){
                    memset(str, '\0', 100);
                    str="Input username again:";
                    send(valueOfAccept, str, strlen(str), 0);

                    select(valueOfAccept+1, &listSocket, NULL, NULL, NULL);

                    if(FD_ISSET(valueOfAccept, &listSocket)){
                        memset(nickname, '\0', 30);
                        recv(valueOfAccept,nickname, sizeof(nickname), 0);
                        puts(nickname);
                    }
                }

                memset(str, '\0', 100);
                str="Register Successfully";
                send(valueOfAccept, str, strlen(str), 0);


                for (int j=0; j<sizeMaxClient; j++){
                    if (client[j]==0){
                        printf("ssssss");
                        client[j]=valueOfAccept;
                        strcpy(listUser[j].Name, nickname);
                        puts(listUser[j].Name);
                        break;
                    }

                }
            }
            else{
                printf("false\n");
            }



        }

    }

    valueOfShutdown=shutdown(valueOfSocket, SHUT_RDWR);

    if (valueOfShutdown==0){
        printf("Shutdown successful\n");
    }
    else{
        printf("shutdown false\n");
    }
    CLOSE:
        close(valueOfSocket);
        return -1;
    return 0;
}
