#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    FILE *fp1, *fp2, *fp3;

    char result[100];
    char answer[100];
    
    fp1 = fopen("result1.txt", "r");
    fp2 = fopen("testing_answer.txt", "r");
    fp3 = fopen("acc.txt", "w");

    memset(result, '\0', sizeof(result));
    memset(answer, '\0', sizeof(answer));

    int count = 0;
    int total = 0;
    double trash;

    while (fscanf(fp1, "%s\t%le", result, &trash) != EOF && fscanf(fp2, "%s", answer) != EOF) {
        
        
        total++;

        if (!strcmp(result, answer))
            count++;

        memset(result, '\0', sizeof(result));
        memset(answer, '\0', sizeof(answer));
        
    }

    double acc = (double) count / total;
    
    fprintf(fp3, "%lf\n", acc);

    return 0;
}
