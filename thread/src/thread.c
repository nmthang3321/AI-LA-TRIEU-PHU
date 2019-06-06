
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#define FILENAME "my_file.txt"
char NickName[5][50];
struct sockaddr_in serverAddress;
struct User{
    char *Name;
    int CheckTrue;
    int Next;
};
struct User listUser[10];

//==========================================================
//SAVE USER NAME INTO ARRAY
void UserName (char *Name, int ClientAccept, int count){
    int j,i, Check=0, OK=1;
    char *str;
    for(i=0; i<strlen(Name); i++){
        NickName[count][i]=Name[i];
    }
    if(count>1){
    for(i=1; i<count; i++){
        for(j=0; j< strlen(Name); j++){
            if(NickName[count][j]==NickName[i][j]){
                Check++;
                if(Check==strlen(Name)){
                    str="User Name is exits please input other Name!\n";
                    send(ClientAccept, str, strlen(str), 0);
                    str="Input other UserName\n";
                    send(ClientAccept, str, strlen(str), 0);
                    memset(&NickName[count], '\0', sizeof(Name));
                    OK=0;
                }

            }
            else
                Check=0;
        }
    }
    }
    if(OK==1){
        str="Register Successfully!\n";
        send(ClientAccept, str, strlen(str),0);
        str="Please wait other player.....";
        send(ClientAccept, str, strlen(str),0);
        printf("There is a new player %s\n", inet_ntoa(serverAddress.sin_addr));
        printf("User Name: ");
        puts(NickName[count]);
    }

}

//===============================================================

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


//===============================================================
int main()
{
    memset(NickName, '\0', sizeof(NickName));
    int CountSocket=1;
    int Accept;
    int ServerSocket =  socket(AF_INET,SOCK_STREAM,0);
    if (ServerSocket == -1)
    {
        perror("CREATE SOCKET");
        exit(0);
    }
    int i = 1;
    int CheckElement = setsockopt(ServerSocket,SOL_SOCKET,SO_REUSEADDR,&i,sizeof(int));
    if (CheckElement == -1)
    {
    perror("Set reuse");
    }
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(5000);
    serverAddress.sin_addr.s_addr = inet_addr("192.168.81.12");
    socklen_t len = sizeof(struct sockaddr_in);
    CheckElement = bind(ServerSocket,(struct sockaddr*)&serverAddress,len);
    if (CheckElement == -1)
        {
            perror("BIND");
            exit(0);
        }
    CheckElement = listen(ServerSocket,10);
    if (CheckElement == -1)
    {
        perror("Listen");
        exit(1);
    }
    struct sockaddr_in ClientAddress;
    len = sizeof(struct sockaddr_in);
    fd_set readfds;
    fd_set masterfds;
    FD_ZERO(&masterfds);
    FD_ZERO(&readfds);
    FD_SET(ServerSocket,&masterfds);
    int MaxSocket = ServerSocket;
    int check=0;
    while(1){
    while(check<=3){
        for(i=0; i<5; i++){
            if(NickName[i][1]!='\0'){
                check++;
            }
        }
        memcpy(&readfds,&masterfds,sizeof(masterfds));
        int Select = select(MaxSocket + 1, &readfds,NULL,NULL,NULL);
        for (i = 0; i <= MaxSocket;i++){
            if (FD_ISSET(i,&readfds)){
                int close_fd = 0;
                    if (i == ServerSocket){
                        Accept = accept(ServerSocket,(struct sockaddr*)&ClientAddress,&len);
                        CountSocket++;
                        FD_SET(Accept,&masterfds);
                        if (Accept > MaxSocket ) MaxSocket = Accept;
                    }
                    else{
                        char *message;
                        int nrecv = recv(i,message,100,0);
                        if (nrecv != 0){
                           message[nrecv] =0;
                           UserName(message,i, CountSocket);
                        }

                   }

            }
        }
    }
    }

    return 0;
}
