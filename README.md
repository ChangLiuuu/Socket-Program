# Socket-Program (UDP)
client sends 5 packets to server: the first one is normal pocket and other four pockets have different errors.

server check each five packet and show details of data packet: 

listener: got packet from 127.0.0.1
listener: packet is 11 bytes long
This is packet number 4 
start "0XFFFF"
client Id "0X1"
packet type "0XFFF1"
segment number "4"
expected segment number "2"
pre segment number "1"
length of payload "5"
expected length of payload "5"
Payload: "Hello"
Packet Ending  "0XFFFF"
Packet is out of sequence.

then server sends back ACK pocket or different REJECT pockets with different reject sub codes.

<h2>timeout test:</h2> 
When sending first packet, do not start the server. The client will start to wait for server's response for 3 seconds. if server has no response after 3 secs, client resend packet again. After 3 times of no responses, client exits:

Enter your payload:
Hello
This is a normal pocket!
Packet 1 . No response 1 time
Packet 1 . No response 2 time
Packet 1 . No response 3 time
No server response.
	
<h2>five packets' test cases</h2>

The first packet client sent is a normal packet without error, and server sends back ACK packtype: 0XFFF2. The other four packets with different errors and server send back corresponding REJECT sub code:

/*
0XFFF4: Packet out of sequence. 
0XFFF5 : Packet length does not match. 
0XFFF6: End of packet lost. 
0XFFF7: Duplicate packet. 
**/

Enter your payload:
a
This is a normal pocket!
From receving packet's type: "0XFFF2"
Enter your payload:
s
send a out-of-sequence pocket!
From receving packet's type: "0XFFF3"
Reject Error: Packet out of sequence. 0XFFF4 

Enter your payload:
d
This is a wrong payload length pocket!
From receving packet's type: "0XFFF3"
Reject Error: Packet length does not match. 0XFFF5 

Enter your payload:
f
This is a end-lost pocket!
From receving packet's type: "0XFFF3"
Reject Error: End of packet lost. 0XFFF6 

Enter your payload:
g
This is a duplicated pocket.
From receving packet's type: "0XFFF3"
Reject Error: Duplicate packet. 0XFFF7 

Socket programming II (UDP)

client sends three subscriber numbers to server. 

server checks the three numbers if they exist in the database. If one exists, server
checks they technology type(2G, 3G or 4G) and if users have paid for services.
Not exist: 0XFFFA
Not paid: 0XFFF9

If users have paid, they can access to network.
Access_OK: 0XFFFB

<h2>timeout test:</h2> 
When sending first packet, do not start the server. The client will start to wait for server's response for 3 seconds. if server has no response after 3 secs, client resend packet again. After 3 times of no responses, client exits.

<h2>test cases showed at client</h2>

1. Send subscriber number: 408-555-5555 (4G) Not exist.
Reject: Subscriber does not exist on database - 0XFFFA 

2. Send subscriber number: 408-554-6805 (4G) Paid
Succeed to access the network!
The technology type is 4G 

3. Send subscriber number: 408-666-8821 (3G) Not paid
Reject: Subscriber has not paid - 0XFFF9 

<h2>test cases showed at server</h2>

listener: waiting to recvfrom...
The subscriber number is 4085555555 
Subscriber does not exist in database. 

The subscriber number is 4085546805 
Subscriber technology is  4G 
Subscriber can access the network! 

The subscriber number is 4086668821 
Subscriber technology is  3G 
Subscriber has not paid! 


