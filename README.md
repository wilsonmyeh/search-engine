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
**Main Project**:

	$ qmake
	$ make

**Webcrawler**:

	$ cd webcrawler
	$ make


##Run Instructions
**Main Project**:
	From "team_project_11" directory:

	$ bin/team_project11 <seed file>

	Example:

	$ bin/team_project11 data/index.txt

**Webcrawler**:
	From "team_project_11" directory:

	$ webcrawler/bin/webcrawler <seed file> <output file>

	Example:

	$ webcrawler/bin/webcrawler data/seed.txt data/index.txt

###Interface
	To search a single word, enter the word into the "Search Word" textbox and click the "Search Word" button.
	To search using multiple words, enter into either the "Search OR" or "Search AND" textboxes and use their associated buttons.

	Words consist only of digits and letters and are separated only by whitespace. Any punctuation, even apostrophes (e.g. don't), will render an input invalid.