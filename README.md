# MiniFTP

## 1. Function and Introduction:
My blog: http://blog.csdn.net/qq_36196684<br>
<br>
Implement of MiniFTP server.<br>
### 1.1 Introduction
FTP(File Transfer Protocal), working in appliction-layer of TCP/IP whose transfer-layer is using TCP.<br>
FTP is based on C/S model.<br>
see more:https://www.ietf.org/rfc/rfc959.txt
### 1.2 File Type
FTP have four file type:<br>
*  `ASCII file`: basic file of FTP<br>
  Formed by ASCII chars. Each char have 7bit, and high bit is 0.<br>
*  `EBCDIC file`: <br>
*  `Image file`: binary file<br>
  High bit is o/1.<br>
*  `Local file`:<br>

windows:\r\n Linux:\n mac:\r<br>
Transfer with `ASCII`:<br>
windows->linux: \r\n->\n<br>
linux->windows: \n->\r\n<br>
Transfer with `Binary`:<br>
Without any change, keeping.<br>
## 2. The C/S model of MiniFTP server.

![image](https://github.com/qinchao0525/MiniFTP/blob/master/pictures/C_S.jpg)

<br>Note:C/S model <br>


