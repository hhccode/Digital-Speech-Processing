import re

if __name__=='__main__':

    f = open("result/accuracy", "r")

    content = f.read()
    
    state = re.findall(r"state: (\d*)", content)
    gaussian = re.findall(r"gaussian: (\d*)", content)
    acc = re.findall(r"Acc=([0-9]*\.[0-9]+)", content)

    for i in range(len(state)):
        print(state[i] + " " + gaussian[i] + " " + acc[i]);
