# MiniFTP write with C++<br>
## 1.Model figure<br>

## 2.Connect<br>
Connecting with TCP protocol. Sever and client will create a socket by themselves.<br> Server will wait for connection requirement and decide which client requirement<br> will be accept. Connection will be made when sever and client has suited prot and<br> both are free.<br>
<br>
Server will continue to wait for connection when client require to break the link,<br> but server only use the  way that one client with one server with out multi-process<br>
## 3.PDU<br>
In this project, we use binary file for transfer. For the limit of buffer, we can<br> not transfer all file once in the socket. The package will be cut into small pices<br> 
In the transfer process, we need one signal to tell us when to stop. This is the <br> signal measure.<br>

