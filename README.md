Stop-and-Wait Protocol
=============================================================================================================================
In this project, I designed and implemented a reliable block transfer protocol that implements the Stop-and-Wait Protocol. SWAP uses flow control as well as error control. Both the sender and the receiver uses a sliding window with a size of 1. The sender sends one packet at a time and waits for an ACK Frame. 

To detect corrupted packets, a checksum is added to each packet. To prevent duplicate frames, sequence numbers are used. 

![image](https://user-images.githubusercontent.com/94186009/213001327-4a9605c3-2f89-4692-89c0-9f8897e8e439.png#center)

SWAP Client Implementation
-----------------------------------------------------------------------------------------------------------------------------
![image](https://user-images.githubusercontent.com/94186009/213001501-8fa08b29-5df5-4a17-b268-bfc37ece4ee5.png)

SWAP Server Implementation
-----------------------------------------------------------------------------------------------------------------------------
![image](https://user-images.githubusercontent.com/94186009/213001549-6a848dbe-4c0d-4064-b493-34064d113b72.png)

Project Requirements
-----------------------------------------------------------------------------------------------------------------------------
Modify the source code files with the following requirements:

1. Complete the functionality of the swap_write() function in swap_client.c
2. Complete the functionality of the swap_read() function in swap_server.c
3. Implement error checking for DATA frames as well as ACK frames to provide reliable communications. Use checksum() function in checksum.c
4. Account for error frames and duplicate frames

Compile and Run
-----------------------------------------------------------------------------------------------------------------------------
1. Compile SWAP Server Program
    $ gcc test_swap_server.c swap_server.c sdp.c checksum.c -o server
   
2. Run SWAP Server with port number in the command line
    ./server 8899
    
3. Compile SWAP Client Program
     $ gcc test_swap_client.c swap_client.c sdp.c checksum.c -o test_swap_client
     
4. Run SWAP Client Program with IP Address and port number in the command line
     ./client 127.0.0.1 8899


    
 

