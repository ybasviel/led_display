#!/bin/python3

import subprocess
import numpy as np

def generate_char_matrix(char):
    cmd = "./pbmfontgen.sh " + char + " " + "./misakifont/misaki_gothic_2nd.ttf"
    subprocess.run(cmd, shell=True)

    with open("./out/char.pbm") as f:
        pbm = f.readlines()
        
    del(pbm[:2])
    #print(pbm)
    
    matrix = []

    for line in pbm:
        line = line.replace(" ", "")
        
        newline = []

        for i in range(8):
            newline.append(line[i])
        
        matrix.append(newline)

    matrix = np.array(matrix)

    matrix = matrix.T
    matrix = matrix.tolist()
    return matrix

def makejson(text):

    out_txt = ""

    for char in text:
        mat = generate_char_matrix(char)
        
        for line in mat:
            line.reverse()

            for ch in line:
                out_txt += str(ch)
            
            out_txt += "\n"

        out_txt += "00000000\n"

    bitarry = out_txt.splitlines()

    sndBuff = []
    
    for bit in bitarry:
        sndBuff.append(int(bit, 2))



    sndtxt = "["
    for snd in sndBuff:
        sndtxt += str(snd) + ","
    
    sndtxt += "0,0,0,0]"

    #print("{\"mat_len\": ", end="")
    #print(str(len(sndBuff) + 4 ) + ",", end="")
    #print("\"data\": ", end="")
    #print(sndtxt, end="")
    #print("}", end="")

    json = "{\"mat_len\": " + str(len(sndBuff) + 4 ) + ",\"data\": " + sndtxt + "}"

    return json

def output_jsonfile(text):
    json = makejson(text)
    with open("data.json", "w") as f:
        f.write(json)

if __name__ == "__main__":

    text = str(input("> "))

    #print(makejson(text))
    output_jsonfile(text)
