#include "hmm.h"
#include <sys/stat.h>
#include <sys/types.h>

#ifndef MAX_LENGTH
#   define MAX_LENGTH 50
#endif

#ifndef MAX_NUMBER_OF_INPUT
#   define MAX_NUMBER_OF_INPUT 99999
#endif

double gamma_observe_k[MAX_NUMBER_OF_INPUT][MAX_STATE][MAX_OBSERV];

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
    int iteration;
    HMM hmm;

    iteration = atoi(argv[1]);
    loadHMM(&hmm, argv[2]);

    FILE *fp = open_or_die(argv[3], "r");
    //FILE *output_model = open_or_die(argv[4], "w");

    char alphabet_obs[MAX_LENGTH];  // input observation sequence
    int number_obs[MAX_LENGTH];

    double alpha[hmm.state_num][MAX_LENGTH];  // alpha: N * T
    double beta[hmm.state_num][MAX_LENGTH];  // beta: N * T
    double gamma[hmm.state_num][MAX_LENGTH];  // gamma: N * T
    double epsilon[MAX_LENGTH][hmm.state_num][hmm.state_num];  // epsilon: T-1 * N * N
    
    char dest[100];
    char num[10];
    char *model_name = argv[4];

    int i, j, t, k, iter, line;
    int number_of_sequence, length_of_observation;
    double temp;
    double summation_gamma, summation_epsilon;
    
    memset(alphabet_obs, '\0', sizeof(alphabet_obs));
    memset(number_obs, 0, sizeof(number_obs));
    
    for (iter = 0; iter < iteration; iter++) {

        memset(dest, '\0', sizeof(dest));
        memset(num, '\0', sizeof(num));

        strcat(dest, "./");
        sprintf(num, "%d", iter+1);
        strcat(dest, num);

        mkdir(dest, 0700);

        strcat(dest, "/");
        strcat(dest, model_name);
        
        FILE *output_model = open_or_die(dest, "w");

        //break;
        /* Initialization */
        number_of_sequence = 0;
        rewind(fp);

        memset(alpha, 0, sizeof(alpha));
        memset(beta, 0, sizeof(beta));
        memset(gamma, 0, sizeof(gamma));
        memset(epsilon, 0, sizeof(epsilon));
        memset(gamma_observe_k, 0, sizeof(gamma_observe_k));

        while (fscanf(fp, "%s ", alphabet_obs) != EOF) {

            length_of_observation = strlen(alphabet_obs);

            printf("Iteration %d Line: %d\n", iter, number_of_sequence);
            alphabet_to_number(alphabet_obs, number_obs);   
            
            //  Calculate alpha_0(i) (First column of alpha matrix)
            //  Calculate beta_T-1(i) (Last column of beta matrix)
            
            for (i = 0; i < hmm.state_num; i++) {
            
                alpha[i][0] = hmm.initial[i] * hmm.observation[number_obs[0]][i];
                beta[i][length_of_observation-1] = 1;

            }
       
            // Induction on alpha
            for (t = 0; t < length_of_observation-1; t++) {
                for (j = 0; j < hmm.state_num; j++) {
                    temp = 0;
                
                    for (i = 0; i < hmm.state_num; i++) {
                        temp += alpha[i][t] * hmm.transition[i][j];
                    }
                    alpha[j][t+1] = temp * hmm.observation[number_obs[t+1]][j];
                }
            }

            // Induction on beta
            for (t = length_of_observation-2; t >= 0; t--) {
                for (i = 0; i < hmm.state_num; i++) {
                    temp = 0;

                    for (j = 0; j < hmm.state_num; j++) {
                        temp += hmm.transition[i][j] * hmm.observation[number_obs[t+1]][j] * beta[j][t+1];
                    }

                    beta[i][t] = temp;
                }
            }

            // Calculate gamma
            for (t = 0; t < length_of_observation; t++) {

                temp = 0;             
                for (i = 0; i < hmm.state_num; i++)
                    temp += alpha[i][t] * beta[i][t];

                for (i = 0; i < hmm.state_num; i++) {                   
                    // Accumulate gamma
                    gamma_observe_k[number_of_sequence][i][number_obs[t]] += (alpha[i][t] * beta[i][t]) / temp;
                    gamma[i][t] += (alpha[i][t] * beta[i][t]) / temp;
                }
            }

            // Calculate epsilon
            for (t = 0; t < length_of_observation-1; t++) {
                
                temp = 0;
                for (i = 0; i < hmm.state_num; i++) {
                    for (j = 0; j < hmm.state_num; j++) {
                        temp += alpha[i][t] * hmm.transition[i][j] * hmm.observation[number_obs[t+1]][j] * beta[j][t+1];
                    }
                }

                for (i = 0; i < hmm.state_num; i++) {
                    for (j = 0; j < hmm.state_num; j++) {
                        // Accumulate epsilon
                        epsilon[t][i][j] += (alpha[i][t] * hmm.transition[i][j] * hmm.observation[number_obs[t+1]][j] * beta[j][t+1]) / temp;
                    }
                }
            }

            number_of_sequence++;
        }  // End of file reading
        

        // Update pi_i (initial)
        for (i = 0; i < hmm.state_num; i++)
            hmm.initial[i] = gamma[i][0] / number_of_sequence;

        // Update aij (transition)
        for (i = 0; i < hmm.state_num; i++) {

            summation_gamma = 0;
            for (t = 0; t < length_of_observation-1; t++)
                summation_gamma += gamma[i][t];

            for (j = 0; j < hmm.state_num; j++) {

                summation_epsilon = 0;
                for (t = 0; t < length_of_observation-1; t++)
                    summation_epsilon += epsilon[t][i][j];

                hmm.transition[i][j] = summation_epsilon / summation_gamma;
            }
        }
        
        // Update b_j(k) (observation)
        for (j = 0; j < hmm.state_num; j++) {

            summation_gamma = 0;
            for (t = 0; t < length_of_observation; t++)
                summation_gamma += gamma[j][t];

            for (k = 0; k < hmm.observ_num; k++) {

                temp = 0;
                for (line = 0; line < number_of_sequence; line++)
                    temp += gamma_observe_k[line][j][k];

                hmm.observation[k][j] = temp / summation_gamma;
            }
        }
        
        /* End of updating the model */
        dumpHMM(output_model, &hmm);
        fclose(output_model);
    }

    //dumpHMM(output_model, &hmm);

    return 0;
}
