import numpy as np
import sys

if __name__=='__main__':
    state_number = int(sys.argv[1])
    
    file = open("lib/proto", "w+")
    
    print("~o <VECSIZE> 39 <MFCC_Z_E_D_A>", file=file)
    print("~h \"proto\"", file=file)
    print("<BeginHMM>", file=file)
    print("<NumStates> %d" % state_number, file=file)

    for i in range(2, state_number):
        print("<State> %d" % i, file=file)
        print("<Mean> 39", file=file)
        print("0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0", file=file)
        print("<Variance> 39", file=file)
        print("1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0 1.0", file=file)

    print("<TransP> %d" % state_number, file=file)

    matrix = np.zeros([state_number, state_number])
    pos = 1

    matrix[0][1] = 1.0
    
    while pos+1 < state_number:
        matrix[pos][pos] = 0.5
        matrix[pos][pos+1] = 0.5
        pos += 1
            
    for i in range(state_number):
        for j in range(state_number):
            if j != state_number-1:
                print("%.1f " % matrix[i][j], end="", file=file)
            else:
                print("%.1f" % matrix[i][j], file=file)
        
    print("<ENDHMM>", file=file)
