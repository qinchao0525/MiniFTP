# MiniFTP
Declarationg:`This project just for learning network programming and first use of open source.`
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

<br>P1. C/S model <br>
### 1.6 working model
* active model<br>
client->server
* passive model<br>
server->client
## 2. Project Requirements
### 2.1 command
### 2.2 configure
### 2.3 leisure disconnect
### 2.4 rate limit
### 2.5 limit connect
### 2.6 breakpoint reconnect
## 3. system struct
### 3.1 sys logic struct
![image](https://github.com/qinchao0525/MiniFTP/blob/master/pictures/sysstruct.jpg)
<br>P2. The system struct.<br>
### 3.1 submodel
This is the summodel, we implement the function int the model.<br>
![submodel](https://github.com/qinchao0525/MiniFTP/blob/master/pictures/submodel.jpg)
<br>P3. summodel</br>


# MiniFTP Written with C++
## 1.Frame of sysmodel
![image](https://github.com/qinchao0525/MiniFTP/blob/master/pictures/2016112162409439.jpg)
## 2.Flow chart for C++ programming.
![image](https://github.com/qinchao0525/MiniFTP/blob/master/pictures/2016112162442621.jpg)<br>
`NOTE:` reference from http://www.jb51.net/article/96337.htm
## 3.Connection
  Connecting with TCP protocol. Server and client will create a socket by themselves. Server<br>
will wait for connection requirement and decide which client requirement will be accept. Connection<br>
will be made when server and client has suited port and both are free.<br>
  Server will continue to wait for connection when client require to break the link, but server only <br>
use the way that one client with one server with out multi-process.
## 4.Data format
