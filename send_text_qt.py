#!/bin/python3

import sys
from PyQt5.QtWidgets import QApplication, QWidget, QPushButton, QMessageBox
from PyQt5.QtGui import QIcon

from PyQt5.QtWidgets import (QApplication, QWidget, 
                             QPushButton, QCheckBox, 
                             QSlider, QLineEdit, 
                             QCalendarWidget, QProgressBar)

import mktext
 
class MainWindow(QWidget):
    def __init__(self, parent=None):
        super(MainWindow, self).__init__(parent) # 初期化
        self.initUI() # UIの初期化

    def initUI(self): # UIの初期化をするメソッド
        self.resize(450, 100) # ウィンドウの大きさの設定(横幅, 縦幅)
        self.move(200, 100) # ウィンドウを表示する場所の設定(横, 縦)
        self.setWindowTitle('send text Qt') # ウィンドウのタイトルの設定
        #self.setWindowIcon(QIcon('xxxx.jpg')) # ウィンドウ右上のアイコンの設定

        btn = QPushButton('Send', self) # ボタンウィジェット作成
        btn.resize(btn.sizeHint()) # ボタンのサイズの自動設定
        btn.move(300, 50) # ボタンの位置設定(ボタンの左上の座標)

        btn.clicked.connect(self.sendbtn)

        # テキストボックス
        self.textbox = QLineEdit(self)
        self.textbox.setGeometry(80, 50, 200, 20)

    def sendbtn(self):
        QMessageBox.information(self, 'Sending', "Send?", QMessageBox.NoButton)
        text = self.textbox.text()
        mktext.sendText(text,"./misakifont/misaki_gothic_2nd.ttf", "")
        self.textbox.setText("")
        
 
if __name__ == '__main__':
    app = QApplication(sys.argv)
    main_window = MainWindow()
    main_window.show()
    sys.exit(app.exec_())