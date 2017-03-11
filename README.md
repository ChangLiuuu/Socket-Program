<h1># Socket-Program (UDP)</h1>
client sends 5 packets to server: the first one is normal pocket and other four pockets have different errors.<br>

server check each five packet and show details of data packet: <br>

listener: got packet from 127.0.0.1<br>
listener: packet is 11 bytes long<br>
This is packet number 4 <br>
start "0XFFFF"<br>
client Id "0X1"<br>
packet type "0XFFF1"<br>
segment number "4"<br>
expected segment number "2"<br>
pre segment number "1"<br>
length of payload "5"<br>
expected length of payload "5"<br>
Payload: "Hello"<br>
Packet Ending  "0XFFFF"<br>
Packet is out of sequence.<br>

then server sends back ACK pocket or different REJECT pockets with different reject sub codes.<br>

<h2>timeout test:</h2> 
When sending first packet, do not start the server. The client will start to wait for server's response for 3 seconds. if server has no response after 3 secs, client resend packet again. After 3 times of no responses, client exits:

Enter your payload:<br>
Hello<br>
This is a normal pocket!<br>
Packet 1 . No response 1 time<br>
Packet 1 . No response 2 time<br>
Packet 1 . No response 3 time<br>
No server response.<br>
	
<h2>five packets' test cases</h2>

The first packet client sent is a normal packet without error, and server sends back ACK packtype: 0XFFF2. The other four packets with different errors and server send back corresponding REJECT sub code:

/*<br>
0XFFF4: Packet out of sequence. <br>
0XFFF5 : Packet length does not match. <br>
0XFFF6: End of packet lost. <br>
0XFFF7: Duplicate packet. <br>
**/<br>

Enter your payload:<br>
a<br>
This is a normal pocket!<br>
From receving packet's type: "0XFFF2"<br>
Enter your payload:<br>
s<br>
send a out-of-sequence pocket!<br>
From receving packet's type: "0XFFF3"<br>
Reject Error: Packet out of sequence. 0XFFF4 <br>

Enter your payload:<br>
d<br>
This is a wrong payload length pocket!<br>
From receving packet's type: "0XFFF3"<br>
Reject Error: Packet length does not match. 0XFFF5 <br>

Enter your payload:<br>
f<br>
This is a end-lost pocket!<br>
From receving packet's type: "0XFFF3"<br>
Reject Error: End of packet lost. 0XFFF6 <br>
<br>
Enter your payload:<br>
g<br>
This is a duplicated pocket.<br>
From receving packet's type: "0XFFF3"<br>
Reject Error: Duplicate packet. 0XFFF7 <br>

<h1>Socket programming II (UDP)</h1>

client sends three subscriber numbers to server. <br>

server checks the three numbers if they exist in the database. If one exists, server<br>
checks they technology type(2G, 3G or 4G) and if users have paid for services.<br>
Not exist: 0XFFFA<br>
Not paid: 0XFFF9<br>

If users have paid, they can access to network.<br>
Access_OK: 0XFFFB<br>

<h2>timeout test:</h2> 
When sending first packet, do not start the server. The client will start to wait for server's response for 3 seconds. if<br> server has no response after 3 secs, client resend packet again. After 3 times of no responses, client exits.<br>

<h2>test cases showed at client</h2>

<font color = "red">1. Send subscriber number: 408-555-5555 (4G) Not exist.</front> <br>
Reject: Subscriber does not exist on database - 0XFFFA <br>

2. Send subscriber number: 408-554-6805 (4G) Paid<br>
Succeed to access the network!<br>
The technology type is 4G <br>

3. Send subscriber number: 408-666-8821 (3G) Not paid<br>
Reject: Subscriber has not paid - 0XFFF9 <br>

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


