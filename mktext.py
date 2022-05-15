#!/bin/python3

from argparse import ArgumentParser, FileType, ArgumentDefaultsHelpFormatter
from pathlib import Path
import subprocess
from sys import byteorder
from xmlrpc.client import boolean
import numpy as np
from time import sleep
import serial
from serial.tools import list_ports
from unicodedata import east_asian_width

def isEastAsianChar(char):     #文字数を英数字1, 日本語2文字でカウント
    if east_asian_width(char) in 'FWA':
        return True
    else:
        return False
    

def select_port(baudrate=115200):
    ser = serial.Serial(writeTimeout = 200)
    ser.baudrate = baudrate    
    ser.timeout = 1       # タイムアウトの時間

    ports = list_ports.comports()    # ポートデータを取得
    devices = [info.device for info in ports]

    if len(devices) == 0:
        # シリアル通信できるデバイスが見つからなかった場合
        return None
    else:
        ser.port = devices[-1]
    
    # 開いてみる
    try:
        ser.open()
        return ser
    except:
        return None

def generate_char_matrix(char, fontpath):
    cmd = "./pbmfontgen.sh " + char + " " + str(fontpath)
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

def makedata(text, fontpath):

    out_txt = ""

    for char in text:
        mat = generate_char_matrix(char, fontpath)
        
        if(isEastAsianChar(char) == False):
            mat = mat[:4]

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

    return sndtxt, len(sndBuff)


def sendText(text, fontpath, jsonpath):

    sndtxt = makedata(text, fontpath)

    ser=select_port(115200)
    
    if(ser == None):
        print("Error: Serial Port Busy\n")
    
    sleep(2)

    ser.write(bytes(sndtxt, "ASCII"))


    ser.close()


def makeJson(text, fontpath, jsonpath):

    sndtxt, lenbuff = makedata(text, fontpath)
    json = "{\"mat_len\": " + str(lenbuff + 4 ) + ",\"data\": " + sndtxt + "}"
    with open(jsonpath, "w") as f:
        f.write(json)


if __name__ == "__main__":

    parser = ArgumentParser(formatter_class=ArgumentDefaultsHelpFormatter)
    parser.add_argument("-o", "--output", type=Path, default="data.json", required=False, help="json出力ファイル指定")
    parser.add_argument("-f", "--font", type=Path, default="./misakifont/misaki_gothic_2nd.ttf", required=False, help="フォントファイル指定")
    parser.add_argument('-s', "--serial", dest='accumulate', action='store_const', const=sendText, default=makeJson, help="シリアル通信モード")
    args = parser.parse_args()

    fontpath = args.font
    jsonpath = args.output

    text = str(input(""))

    args.accumulate(text,fontpath,jsonpath)
