#include <stdlib.h>
#include <stdio.h>
#include <string.h>


void main() {
 FILE *fp;
 int n, i;
 double y;
 char str[3],lel[9],hmm[20];
 fp = fopen("vec.txt","a+");
 printf("Digite o valor de n: \n");
 scanf("%d", &n);
 sprintf(lel,"%i",n);
 fputs(lel,fp);
 fputs("\n",fp);
 for ( i = 0; i < n;  i++) {
         sprintf(&lel[1],"%i",i);
         str[0] = lel[1];
         str[1] = *" ";
         fputs(str,fp);
         y = (2 * i)+ 0.5981 +((rand()%1000)*0.001f);
         snprintf(hmm,20,"%f",  y);  
         fputs(hmm,fp);
         fputs("\n",fp);
         }
 fclose(fp);    
}        









