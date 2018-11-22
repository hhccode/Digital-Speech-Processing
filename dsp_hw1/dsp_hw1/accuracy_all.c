#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

int main(int argc, char *argv[])
{
    FILE *fp2, *fp3;

    char result[100];
    char answer[100];
   
    fp2 = fopen("testing_answer.txt", "r");
    fp3 = fopen("acc_all.txt", "w");

    memset(result, '\0', sizeof(result));
    memset(answer, '\0', sizeof(answer));

    int iter;
    char folder[100];
    char num[10];

    int count = 0;
    int total = 0;
    double trash;


    for (iter = 1; iter <= atoi(argv[1]); iter++) {
        printf("%d\n", iter);
        memset(folder, '\0', sizeof(folder));
        memset(num, '\0', sizeof(num));

        strcat(folder, "./");
        sprintf(num, "%d", iter);
        strcat(folder, num);
        strcat(folder, "/result1.txt");

        FILE *fp1 = fopen(folder, "r");

        total = 0;
        count = 0;

        rewind(fp2);
        while (fscanf(fp1, "%s\t%le", result, &trash) != EOF && fscanf(fp2, "%s", answer) != EOF) {
            
            
            total++;

            //fprintf(fp3, "%s %s ", result, answer);
            if (!strcmp(result, answer))
                count++;
            //else
            //    fprintf(fp3, "\n");

            memset(result, '\0', sizeof(result));
            memset(answer, '\0', sizeof(answer));
            
        }

        double acc = (double) count / total;
        fclose(fp1);
        fprintf(fp3,"%d: acc = %lf\n", iter, acc);
    }
    return 0;
}
