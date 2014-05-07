#include <QApplication>

#include <deque>
#include <map>
#include <list>
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdexcept>
#include <cstdlib>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <signal.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "mainwin.h"
#include "msort.h"
#include "Set.h"
#include "WebPage.h"
#include "pagerank.h"

#define MYPORT "31803"
#define BACKLOG 20

using namespace std;

struct AlphaStrComp {
 bool operator() (const string & lhs, const string & rhs) 
    { 
    	if(lhs<=rhs)
    	 return true;
        else
        	return false;
    }
 };

 struct comp_bid {
    string keyword;
    double bid;
    string company;
    int hits;
};

void calculateBill(map<string,vector<comp_bid>* > & compMap, ofstream & ofile);
void toLowerCase(string & s);
bool isValid(string & s);

void *get_in_addr(struct sockaddr *sa);
void *newClient(void* sockcli_fd);
void *displayGUI(void* serverip);

int main(int argc, char* argv[])
{
	if(argc < 4) {
		cout << "Arguments: <webpage index> <ad input> <ad output>.\n";
		return 1;
	}

	deque<WebPage> pages;
	map<string,WebPage*> fileLookup;

	ifstream ifile;
	ifile.open(argv[1]);
	if(ifile.fail()) {
		cout << "File not found.\n";
		return 1;
	}

	string n;
	ifile >> n;
	while(!ifile.fail()) {
		pages.push_front(WebPage(n));
		fileLookup.emplace(n, &(pages.front()));
		ifile >> n;
	}

	for(auto& it : pages) //Parses links from each page
		it.parse(fileLookup);

	map<string,Set<WebPage*> > wordMap;
	for(unsigned int x = 0;x < pages.size();x++) //Goes through each page, associating it with each of its words
	{
		Set<string> temp = pages[x].allWords();
		for(auto& it : temp)
		{
			try {
				wordMap.emplace(it,Set<WebPage*>()); //Initializes new set for keyword
			}
			catch(exception e) { }
			try {
					wordMap.at(it).emplace(&pages[x]); //Adds page to Set
			}
			catch(exception e) { }
		}
	}

    ifile.close();
    ifile.open(argv[2]);
    if(ifile.fail()){
        cerr << "Couldn't open file" << endl;
        return 1;
    }

    //output file
    ofstream ofile(argv[3]);
    if(ofile.fail()){
        cerr << "Couldn't open file" << endl;
        return 1;
    }

    comp_bid cb; 
    map<string,vector<comp_bid>* > compMap;


    int count=0;
    int numLines;
    string line;

    //reads the file line by line, extracts every word and stores it in the struct object. Then push the struct obj into the vector comp
    while (getline(ifile, line))  //while their are lines on the page
    {
        stringstream ss;
        ss << line;
        if(count==0) {
            ss>>numLines;
        }
        else if(count>0 && count <=numLines) { 
            string key;
            ss >> key;
            toLowerCase(key);
            cb.keyword = key;
            ss>>cb.bid;
            ws(ss);
            getline(ss,cb.company);
            cb.hits = 0;
            if(compMap.find(key) == compMap.end()) {
                vector<comp_bid>* temp = new vector<comp_bid>();
                compMap[key] = temp;
            }
            compMap[key]->push_back(cb);
        }
        count ++;
    }
    ifile.close();

    //Done with Data Input, time for the networking!

    int socklis_fd; //listen() socket file descriptor
    struct addrinfo hints, *servinfo, *p;
    char s[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if(getaddrinfo(NULL, MYPORT, &hints, &servinfo) != 0) {
		perror("getaddrinfo");
		return 1;
	}

	//Bind to the first socket/port that works
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((socklis_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if (bind(socklis_fd, p->ai_addr, p->ai_addrlen) == -1) {
            close(socklis_fd);
            perror("server: bind");
            continue;
        }

        break;
	}
	
	if (p == NULL) {
		perror("bind");
		return 1;
	}

	
	inet_ntop(p->ai_family, get_in_addr((struct sockaddr*)&p), s, sizeof s);
	string serverip = s;

	freeaddrinfo(servinfo); //Binded and done!

	if(listen(socklis_fd,BACKLOG) == -1) {
		perror("listen");
		return 1;
	}

	int sockcli_fd; //client socket file descriptor
	socklen_t sin_size;
	struct sockaddr_storage client_addr;

	pthread_t gui;
	if(pthread_create(&gui,NULL,displayGUI,(void*)&serverip) < 0) {
		perror("gui thread");
		return 1;
	}


	vector<pthread_t> threads; //C++ doesn't have threadpools ;_;
	while(serverip != "NULL") { //accept() loop until mainwin closes
		cout << serverip << endl;
		sin_size = sizeof client_addr;
		sockcli_fd = accept(socklis_fd, (struct sockaddr*)&client_addr, &sin_size);
		if(sockcli_fd == -1) {
			perror("accept");
			continue;
		}
		inet_ntop(client_addr.ss_family, get_in_addr((struct sockaddr*)&client_addr), s, sizeof s);
		cout << "Connection from " << s << endl;
		pthread_t temp;
		if(pthread_create(&temp,NULL,newClient,(void*)&sockcli_fd) < 0) {
			perror("thread");
			continue;
		}

		threads.push_back(temp);

		
	}
	
    calculateBill(compMap,ofile);
    return 0;
}

void *newClient(void* sockcli_fd)
{
	int fd = *((int*)sockcli_fd);
	return 0;

}

void *displayGUI(void* serverip)
{
	int argc = 1;
	char* argv[1];
	argv[0] = new char[5];
	memset(argv[0],'a',5);
	
	QApplication app(argc, argv);  
	string s = *(string*)serverip;
	s = s + "::" MYPORT;
	MainWin window(s);

	window.setWindowTitle("Server Running...");
	window.show();

	app.exec();

	*(string*)serverip = "NULL"; //Flags exit of mainwin GUI

	return 0;
}

void calculateBill(map<string,vector<comp_bid>* > & compMap, ofstream & ofile)
{
	map<string,double> bill;
	list<string> compNames;
	for(auto & it : compMap)
	{
		vector<comp_bid> & temp = *it.second;
		for(unsigned int i = 0;i < temp.size();i++)
		{
			if(bill.find(temp[i].company) == bill.end()) {
				compNames.push_back(temp[i].company);
				bill[temp[i].company] = 0;
			}
			bill[temp[i].company] += (temp[i].hits * temp[i].bid);
		}
	}
	AlphaStrComp struc;
	merge_sort(compNames,struc);

	for(auto & it : compNames)
	{
		ofile << it << endl;
		ofile << "$" << std::fixed << std::setprecision(2) << bill[it] << endl << endl;
	}
	for(auto & it : compMap)
    	delete it.second;
}

void toLowerCase(string & s)
{
	for(unsigned int x = 0;x < s.length();x++)
		if(s[x] >= 'A' && s[x] <= 'Z')
			s[x] = (char)(s[x] - 'A' + 'a');
}

bool isValid(string & s)
{
	for(unsigned int x = 0;x < s.length();x++) //Checks that all characters are alphanumeric
	{
		if(!(s[x] == ' ' || (s[x] >= 'a' && s[x] <= 'z') || (s[x] >= '0' && s[x] <= '9')))
			return false;
	}
	return true;
}

//gets address in ipv4 or ipv6 format
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}