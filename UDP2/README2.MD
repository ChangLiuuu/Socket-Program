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

