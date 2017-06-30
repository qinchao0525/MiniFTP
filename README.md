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

### 1.3 File struct
* File struct:                  
where there is no internal structure and the file is considered to be a continuous sequence of data bytes
* record struct:
where the file is made up of sequential records
* page struct:
where the file is made up of independent indexed pages.
### 1.4 Transfer model
* STREAM Model<br>
The data is transmitted as a stream of bytes.  There is no
         restriction on the representation type used; record structures
         are allowed.<br>
* BLOCK Model<br>
The file is transmitted as a series of data blocks preceded by
         one or more header bytes.<br>
         
### 1.5 The C/S model of MiniFTP server.

![image](https://github.com/qinchao0525/MiniFTP/blob/master/pictures/C_S.jpg)

<br>Note:C/S model <br>
## 2. Project Requirements


