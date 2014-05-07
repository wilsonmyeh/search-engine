##CSCI 104 Team Project Repository

###Student Information
**Developer 1**:
  + Name: Kevin Koeller
  + USC Email: koeller@usc.edu
  + USC ID: 6934131011

**Developer 2**:
  + Name: Wilson Yeh
  + USC Email: wilsonye@usc.edu
  + USC ID: 4780838615

##Compile Instructions
+ Open up two terminals. Navigate one to 'server' directory and one to 'client' directory. In each:
	
	$ qmake
	$ make

##IMPORTANT
+ Due to (presumably) the dynamic nature of USC Wireless's IP addresses, use the loopback IP (127.0.0.1) to test this program for now.
+ Terminating the server UI does not kill the process. This is because of blocking caused by the accept() command. I've yet to find a decent workaround, so please terminate the UI and then reconnect with another client in order to properly end the server and test the ad billing feature.

##Run Instructions
**Server Terminal**

	$ bin/server <input file> <ad data input file> <ad data billing output file>

+ Example

	$ bin/server data100/index.txt adData/input.in adData/output.out

+ A window should pop up with the host's IP Address and Port.

**Client Terminal**

	$ bin/client <Server IP/Hostname> <Port>

+ Note: Use 127.0.0.1 instead of server displayed IP. Use server displayed port.
+ Example

	$ bin/client 127.0.0.1 31803
	
##Client Interface
+ To search a single word, enter the word into the "Search Word" textbox and click the "Search Word" button.
To search using multiple words, enter into either the "Search OR" or "Search AND" textboxes and use their associated buttons.

+ Words consist only of digits and letters and are separated only by whitespace. Any punctuation, even apostrophes (e.g. don't), will render an input invalid.

+ Double-click any search result to open it.
Single-clicking an ad creates a pop up to symbolize visiting the adsite.

##Miscellaneous
+ Exiting and immediately restarting the server will likely result in a "Address already in use" error. Just wait a minute for it to free itself up.
