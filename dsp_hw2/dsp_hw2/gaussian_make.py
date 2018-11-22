import numpy as np
import sys
import re

if __name__=='__main__':
    mixture_number = int(sys.argv[1])
    
    r_file = open("lib/mix2_10.hed", "r")
    
    res = ""
    for i in range(11):
        if i != 10:
            line = r_file.readline().split()
            line[1] = str(mixture_number)
            res += line[0] + " " + line[1] + " " + line[2] + "\n"
        else:
            res += r_file.readline()

    w_file = open("lib/mix2_10.hed", "w")
    w_file.write(res)
    w_file.close()
    r_file.close()
