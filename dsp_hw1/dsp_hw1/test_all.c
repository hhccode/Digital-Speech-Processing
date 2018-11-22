#include "hmm.h"

#ifndef MAX_LENGTH
# define MAX_LENGTH 50
#endif

void alphabet_to_number(char alphabet[], int number[])
{
    int i;
    for (i = 0; i < strlen(alphabet); i++)
        switch (alphabet[i]) {
            case 'A':
                number[i] = 0;
                break;
            case 'B':
                number[i] = 1;
                break;
            case 'C':
                number[i] = 2;
                break;
            case 'D':
                number[i] = 3;
                break;
            case 'E':
                number[i] = 4;
                break;
            case 'F':
                number[i] = 5;
                break;
        }
}

int main(int argc, char *argv[])
{
    HMM hmm[5];
    char dest[100];
    char num[10];
    char model_name[100];
    char v[100];
    char res_file[100];

    FILE *fp = open_or_die(argv[2], "r");
    
    char alphabet_obs[MAX_LENGTH];  // input observation sequence
    int number_obs[MAX_LENGTH];
    double delta[MAX_STATE][MAX_LENGTH];

    int i, j, t, k, model, iter;
    int length_of_observation, result_model;
    double maximum, result_value;


    for (iter = 1; iter <= atoi(argv[4]); iter++)  {
        
        printf("%d\n", iter);

        memset(dest, '\0', sizeof(dest));
        memset(num, '\0', sizeof(num));
        memset(v, '\0', sizeof(v));
        memset(res_file, '\0', sizeof(res_file));

        strcat(dest, "./");
        sprintf(num, "%d", iter);
        strcat(dest, num);
        strcpy(v, dest);

        strcat(res_file, dest);
        strcat(res_file, "/");

        for (i = 1; i <= 5; i++) {

            strcpy(dest, v);
            
            memset(model_name, '\0', sizeof(model_name));
            memset(num, '\0', sizeof(num));

            strcpy(model_name, "/model_0");
            sprintf(num, "%d", i);
            strcat(model_name, num);
            strcat(model_name, ".txt");
            strcat(dest, model_name);
            
            loadHMM(&hmm[i-1], dest);
        }

        strcat(res_file, argv[3]);
        
        FILE *result_file = open_or_die(res_file, "w");
        
        rewind(fp);
        memset(alphabet_obs, '\0', sizeof(alphabet_obs));
        while (fscanf(fp, "%s", alphabet_obs) != EOF) {
        
            length_of_observation = strlen(alphabet_obs);
            
            memset(number_obs, 0, sizeof(number_obs));
            alphabet_to_number(alphabet_obs, number_obs);

            for (model = 0, result_value = 0; model < 5; model++) {
                
                // Initialize delta
                memset(delta, 0, sizeof(delta));
                for (i = 0; i < hmm[model].state_num; i++)
                    delta[i][0] = hmm[model].initial[i] * hmm[model].observation[number_obs[0]][i];
                    
                // Induction on delta
                for (t = 1; t < length_of_observation; t++) {
                    for (j = 0; j < hmm[model].state_num; j++) {

                        maximum = 0;
                        for (i = 0; i < hmm[model].state_num; i++)
                            if (maximum < (delta[i][t-1] * hmm[model].transition[i][j]))
                                maximum = delta[i][t-1] * hmm[model].transition[i][j];

                        delta[j][t] = maximum * hmm[model].observation[number_obs[t]][j];
                    }
                }

                // Find P*
                maximum = 0;
                for (i = 0; i < hmm[model].state_num; i++)
                    if (maximum < delta[i][length_of_observation-1])
                        maximum = delta[i][length_of_observation-1];
                
                if (result_value < maximum) {
                    result_value = maximum;
                    result_model = model;
                }

            }
            fprintf(result_file, "model_0%d.txt\t%le\n", result_model+1, result_value);
            
            memset(alphabet_obs, '\0', sizeof(alphabet_obs));
            
        }
        fclose(result_file);     
    }
    return 0;
}
