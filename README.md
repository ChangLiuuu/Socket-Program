# Socket-Program
client sends 5 pockets to server: the first one is normal pocket and other four pockets have different errors.
server check the pockets and sends back ACK pocket or different REJECT pockets.

<h2>ACK timeout test:</h2> 
Do not run the server, client begins to send packets to the server. The client will try to resend 3 times if the server does not respond ack message for 3 seconds. After 3 times trying, the client prints out: "Server does not respond".
	
 <h2 color ="red">Error packet test:</h2>
	0xFFF4: reject out of sequence
  Client sends packet number 1 first, and then send packet number 5.dfd
	
	0xFFF5: reject length mismatch
  Client sends packet with wrong packet length.

	0xFFF6: End of packet missing
  Client sends packet without end of packet 0xFFFF.

		
	0xFFF7: duplicate packet(same sequence number with the last packet)
	Client sends packet number 1 first, and then send packet number 1 again.

2 Compile command
	gcc -o client2 client2.c
	gcc -o server2 server2.c


4 Testing cases
 a. Ack timeout
    Run the client2 program only.

 b. Subscriber permitted to access the network.
    Run the server2 program first;
    Run the client2 program;
    Input the subscriber number on the database which has paid.

 c. Subscriber does not exist on the database.
   Run the server2 program first;
   Run the client2 program;
   Input the subscriber number does not exist on the database.

 d. Subscriber has not paid.
   Run the server2 program first;
   Run the client2 program;
   Input the subscriber number on the database which has not paid.
