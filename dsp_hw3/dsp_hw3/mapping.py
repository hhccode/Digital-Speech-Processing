# -*- coding: utf-8 -*-
# Environment: Python 3.7.1

from collections import OrderedDict
import sys

if __name__ == "__main__":
    
    mapping = OrderedDict()

    Rfile = open(sys.argv[1], "r", encoding="big5-hkscs")
    
    for line in Rfile:
        character, zhuyin = line.split()
        zhuyin = zhuyin.split("/")
        
        for i in range(len(zhuyin)):
            if zhuyin[i][0] not in mapping.keys():
                mapping[zhuyin[i][0]] = [character]
            else:
                if character not in mapping[zhuyin[i][0]]:
                    mapping[zhuyin[i][0]].append(character)
        
    Wfile = open(sys.argv[2], "w", encoding="big5-hkscs")
    
    for key in mapping.keys():
        print(key, end="", file=Wfile)
        for character in mapping[key]:
            print(" " + character, end="", file=Wfile)
        print("", file=Wfile)
        for character in mapping[key]:
            print(character + " " + character, file=Wfile)

    Rfile.close()
    Wfile.close()