#!/usr/bin/python3

from argparse import ArgumentParser, FileType, ArgumentDefaultsHelpFormatter
from pathlib import Path
import subprocess
from time import sleep
import serial
from serial.tools import list_ports
from unicodedata import east_asian_width
from PIL import Image
import json

def isEastAsianChar(char): #半角英字or全角日本語
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

    if(isEastAsianChar(char)):
        charwidth = 8
    else:
        charwidth = 4

    im = Image.open("./out/char.png")
    matrix = [0] * charwidth
    for x in range(charwidth):
        for y in range(8):
            if(im.getpixel((x,y))[3] >= 128):
                matrix[x] += 2**y
            else:
                pass

    return matrix

def makedata(text, fontpath):

    sndmatrix = []

    for char in text:
        mat = generate_char_matrix(char, fontpath)

        sndmatrix += mat
        sndmatrix.append(0)

    sndmatrix += [0,0,0,0]

    return sndmatrix, len(sndmatrix)


def sendText(text, fontpath, jsonpath):

    sndmatrix = makedata(text, fontpath)[0]

    sndtxt = str(sndmatrix)

    ser=select_port(115200)
    
    if(ser == None):
        print("Error: Serial Port Busy\n")
    
    sleep(2)

    ser.write(bytes(sndtxt, "ASCII"))
    ser.close()


def makeJson(text, fontpath, jsonpath):

    sndmatrix, lenbuff = makedata(text, fontpath)
    jsondata = {"mat_len": lenbuff + 4, "data": sndmatrix }
    with open(jsonpath, "w") as f:
        json.dump(jsondata, f)


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
