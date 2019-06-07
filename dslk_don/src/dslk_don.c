
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
#include <time.h>
#define FILENAME "my_file.txt"
#define FILE_ANSER "dap_an.txt"

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
char *FileData (int Line, char *t){
  int i;
  char *StrBuff = NULL;
  size_t StrBuffSize = 0;
  ssize_t LineSize;
  FILE *fp = fopen(t, "r");
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

    addresServer.sin_port=htons(5001);
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
    int check123=0;
    ClearClient();

    while(1){
    while(check123<2){

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
                }

                while (CheckUserName(nickname)){
                    memset(str, '\0', 100);
                    str="User Name is exits please input user name again!:";
                    send(valueOfAccept, str, strlen(str), 0);

                    select(valueOfAccept+1, &listSocket, NULL, NULL, NULL);

                    if(FD_ISSET(valueOfAccept, &listSocket)){
                        memset(nickname, '\0', 30);
                        recv(valueOfAccept,nickname, sizeof(nickname), 0);
                    }
                }

                memset(str, '\0', 100);
                str="Register Successfully, please wait until full player!";
                send(valueOfAccept, str, strlen(str), 0);


                for (int j=0; j<sizeMaxClient; j++){
                    if (client[j]==0){
                        client[j]=valueOfAccept;
                        strcpy(listUser[j].Name, nickname);
                        puts(listUser[j].Name);
                        check123++;
                        break;
                    }

                }
            }
            else{
                printf("false\n");
            }



        }

    }

    for(int i=0; i<check123; i++){
        str="START GAME!\n";
        char *str1="SO NGUOI CHOI: 3\n";
        char *str2="LUOT CHOI CUA BAN:\n";
        char *str3="TOTAL QUESTIONS: 10\n";
        send(client[i], str, strlen(str), 0);
        send(client[i], str1, strlen(str1), 0);
        send(client[i], str2, strlen(str2), 0);
        send(client[i], str3, strlen(str3), 0);

    }


    char Answer[5];
    memset(Answer, '\0', sizeof(Answer));
    for(int j=0; j< check123; j++){
        for(int k=1; k<=6; k++){
            send(client[j], FileData(k, FILENAME), strlen(FileData(k, FILENAME)), 0);

        }
        int LineAnswer=1;
        recv(client[j], Answer, 1, 0);
//        puts(Answer);
        if(strcmp(Answer,FileData(LineAnswer, FILE_ANSER))==0){
            str="exactly! please wait other player's answer................";
            send(client[j], str, strlen(str), 0);
        }

//        if(Answer == FileData(j+1, FILE_ANSER)){
//            memset(str, '\0', sizeof(str));
//            str="exactly! please wait other player's answer................";
//            send(client[j], str, strlen(str), 0);
//        }
////        else{
////            memset(str, '\0', sizeof(str));
////            str="Wrong! You are failed";
////            send(client[j], str, strlen(str), 0);
////            close(client[j]);
////        }
////

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
