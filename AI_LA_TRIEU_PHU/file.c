#include <stdlib.h>
#include <stdio.h>  
#define FILENAME "my_file.txt"

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

int main(void)
{
  printf("%s",FileData(4));
  return 0;
  
}