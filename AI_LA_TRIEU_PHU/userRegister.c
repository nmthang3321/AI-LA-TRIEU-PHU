#include <stdio.h>
#define FILENAME "my_file.txt"

int Answer(char *AnswerClient, int QuestionNum){
	int i;
	int Checker=0;
  	char *StrBuff = NULL;
  	size_t StrBuffSize = 0;
  	ssize_t LineSize;
  	FILE *fp = fopen(FILENAME, "r");
  	for(i=1; i<=QuestionNum+1; i++){
      LineSize =getline(&StrBuff, &StrBuffSize, fp);
  	}
  	if(AnswerClient == StrBuff){
  		Checker= 1;
  	}
  	else{
  		Checker= 0; 
  	}
  	fclose(fp);
  	printf("%s\n",StrBuff );
  	return Checker;
}
int main(){
	printf("%d",Answer("Viet Nam\n", 1));
	return 0;

}
