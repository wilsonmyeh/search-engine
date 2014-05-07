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
#include <sstream>

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
#define MAXBUFFER 4096
#define HEADERLEN 10

using namespace std;

 struct comp_bid {
    string keyword;
    double bid;
    string company;
    int hits;
 };

struct AlphaStrComp {
 bool operator() (const string & lhs, const string & rhs) 
    { 
    	if(lhs<=rhs)
    	 return true;
        else
        	return false;
    }
 };

 struct RankComp {
	bool operator()(const WebPage* lhs, const WebPage* rhs)
	{
		return lhs->getRank() >= rhs->getRank();
	}
};

 struct SortBids {
 bool operator() (const comp_bid* lhs, const comp_bid* rhs) 
    { 
    	if(lhs->bid<=rhs->bid)
    	 return true;
      else if(lhs->bid>=rhs->bid)
        return false;
      else if(lhs->company<=rhs->company)
        return true;
      return false;
    }
 };

void calculateBill(ofstream & ofile);
void toLowerCase(string & s);
bool isValid(string & s);
string padlen(int len);
int packetlen(int sock);
list<string> getAds(Set<string> & input);

int search(int sock, int type);
void wordSearch(Set<string> & inputWords, Set<WebPage*> & results, string & query);
void orSearch(Set<string> & inputWords, Set<WebPage*> & results, string & query);
void andSearch(Set<string> & inputWords, Set<WebPage*> & results, string & query);
int sendWebPage(int sock);
int adClick(int sock);


void *get_in_addr(struct sockaddr *sa);
void *newClient(void* sockcli_fd);
void *displayGUI(void* serverip);

int sendall(int sock, const char* buf, int *len);
int recvall(string & s, int sock);

map<string,Set<WebPage*> > wordMap; //I'm sorry please forgive me
map<string,WebPage*> fileLookup;	//Ah the global vars keep happening
map<string,vector<comp_bid>* > compMap; //Map keywords to bids...O Gog I did it again

int main(int argc, char* argv[])
{
	if(argc < 4) {
		cout << "Arguments: <webpage index> <ad input> <ad output>.\n";
		return 1;
	}

	deque<WebPage> pages;
	
	//fileLookup global var
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

	//wordMap global var
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

    //compMap global var
    comp_bid cb; 
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
	
    calculateBill(ofile);
    return 0;
}

void *newClient(void* sockcli_fd)
{
	int sock = *((int*)sockcli_fd);
	char buf[MAXBUFFER];
	int numBytes;
	if((numBytes = recv(sock,buf,1,0)) == -1) { //First get the type of packet
			perror("recv");
			exit(1);
	}
	while(numBytes != 0)
	{
		//0 = word search, 1 = OR search, 2 = AND search, 3 = webpage request, 4 = adclick
		//Each packet has 10 byte header that indicates length
		//Some search types split into multiple packets
		switch(buf[0]) {
			case '0' : search(sock,0); break;
			case '1' : search(sock,1); break;
			case '2' : search(sock,2); break;
			case '3' : sendWebPage(sock); break;
			case '4' : adClick(sock); break;
		}

		if((numBytes = recv(sock,buf,1,0)) == -1) {
			perror("recv");
			exit(1);
		}
		
		
	}
	return 0;

}

int search(int sock, int type)
{
	string query;
	if(recvall(query,sock) == -1) {
		perror("recv");
		return 1;
	}

	Set<string> inputWords;
	Set<WebPage*> results;
	int len;

	switch(type)
	{
		case 0 : wordSearch(inputWords,results,query); break;
		case 1 : orSearch(inputWords,results,query); break;
		case 2 : andSearch(inputWords,results,query); break;
	}

	list<WebPage*> searchResults;
	for(auto & it : results)
		searchResults.push_back(it);

	pageRank(searchResults);
	RankComp comp;
	merge_sort(searchResults,comp);

	string output;
	for(auto & it : searchResults)
	{
		output += it->filename() + ":::"; //Separate items with :::
	}
	output = padlen(output.length()) + output;

	len = output.length();
	if(sendall(sock,output.c_str(),&len) == -1) {
		perror("send");
		return 1;
	}

	list<string> adlist = getAds(inputWords);
	output = "";
	for(auto & it : searchResults)
	{
		output += it->filename() + ":::";
	}
	output = padlen(output.length()) + output;

	len = output.length();
	if(sendall(sock,output.c_str(),&len) == -1) {
		perror("send");
		return 1;
	}
	return 0;
}

void wordSearch(Set<string> & inputWords, Set<WebPage*> & results, string & query)
{
	try{	results = wordMap.at(query);	}
	catch (exception e) { }

	inputWords.insert(query);
}

void orSearch(Set<string> & inputWords, Set<WebPage*> & results, string & query)
{
	stringstream ss;
	ss << query;
	string s;
	ss >> s;
	while(!ss.fail()) //setUnion every set mapped to each word. Combines every matching WebPage.
	{
		try {	results = results.setUnion(wordMap.at(s));		}
		catch(exception e) { }

		inputWords.insert(s);
		ss >> s;
	}
}

void andSearch(Set<string> & inputWords, Set<WebPage*> & results, string & query)
{
	stringstream ss;
	ss << query;
	string s;
	ss >> s;
	try {	results = wordMap.at(s);	} //Every webpage that doesn't contain each word is culled
	catch(exception e) {	results = Set<WebPage*>();	} //If input not found, then automatically no matches
		
	while(!ss.fail() && !results.empty()) //Performs setIntersection operation with every set mapped to each word
	{
		try {	results = results.setIntersection(wordMap.at(s));	} 
		catch(exception e) {	results = Set<WebPage*>();	}

		inputWords.insert(s);
		ss >> s;
	}
}

int sendWebPage(int sock) //3 Packets: Body, Outgoing, Incoming
{
	string name;
	if(recvall(name,sock) == -1) {
		perror("recv");
		return 1;
	}

	int len;
	WebPage* page = fileLookup[name];
	stringstream body;
	body << *page;
	string output = padlen(body.str().length()) + body.str();
	len = output.length();

	if(sendall(sock,output.c_str(),&len) == -1) {
		perror("send");
		return 1;
	}

	Set<WebPage*> outLinks = page->allOutgoingLinks();
	string outl;
	for(auto & it : outLinks)
	{
		outl += it->filename() + ":::";
	}
	outl = padlen(outl.length()) + outl;

	len = outl.length();
	if(sendall(sock,outl.c_str(),&len) == -1) {
		perror("send");
		return 1;
	}

	Set<WebPage*> inLinks = page->allIncomingLinks();
	string inl;
	for(auto & it : inLinks)
	{
		inl += it->filename() + ":::";
	}
	inl = padlen(inl.length()) + inl;

	len = inl.length();
	if(sendall(sock,inl.c_str(),&len) == -1) {
		perror("send");
		return 1;
	}
	return 0;
}

int adClick(int sock)
{
	string name, keyword;

	if(recvall(name,sock) == -1) {
		perror("recv");
		return 1;
	}

	if(recvall(keyword,sock) == -1) {
		perror("recv");
		return 1;
	}
	
	vector<comp_bid>* temp = compMap[keyword]; //Find bid, increment hits
	for(auto & it : *temp)
	{
		if(it.company == name) {
			it.hits++;
			break;
		}
	}

	return 0;
}

list<string> getAds(Set<string> & input)
{
	list<comp_bid*> bids; 
	for(auto & it : input)
	{
		if(compMap.find(it) == compMap.end())
			continue;
		vector<comp_bid>* temp = compMap[it];
		for(auto & comp : *temp)
			bids.push_back(&comp);
	}


	SortBids struc;
	merge_sort(bids,struc);
	Set<string> compNames;
	list<string> finalList;
	for(auto & it : bids)
	{
		if(compNames.insert(it->company).second) //If company is new, push to list
		{
			finalList.push_back(it->company);
		}
	}
	return finalList;
}

int packetlen(int sock)
{
	char len[HEADERLEN];
	int numBytes;
	if((numBytes = recv(sock,len,HEADERLEN,0)) == -1) {
		perror("recv");
		exit(1);
	}
	return atoi(len);
}

string padlen(int len)
{
	string s = to_string(len);
	while(s.length() < 10)
		s = "0" + s;
	return s;
}

void *displayGUI(void* serverip)
{
	int argc = 1;
	char* argv[1];
	argv[0] = new char[5];
	memset(argv[0],'a',5);
	
	QApplication app(argc, argv);  
	string s = *(string*)serverip;
	s = s + " " MYPORT;
	MainWin window(s);

	window.setWindowTitle("Server Running...");
	window.show();

	app.exec();

	*(string*)serverip = "NULL"; //Flags exit of mainwin GUI

	return 0;
}

void calculateBill(ofstream & ofile)
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

int sendall(int sock, const char* buf, int *len)
{
	int sent = 0; //bytes sent
	int n;
	while(sent < *len) {
		n = send(sock,buf+sent,(*len)-sent,0);
		if(n == -1) break;
		sent += n;
	}
	*len = sent; //save # bytes sent

	return (n == -1) ? -1 : 0; //-1 failure, 0 success
}

int recvall(string & s, int sock)
{
	s = "";
	int len = packetlen(sock);
	int rec = 0;
	char buf[MAXBUFFER];
	int n; //bytes received per iteration
	while(rec < len) {
		if(len-rec < MAXBUFFER-1) //If remaining packet fits in buffer
			n = recv(sock,buf,len-rec,0);
		else n = recv(sock,buf,MAXBUFFER-1,0);
		if(n == -1) break;
		rec += n;
		buf[n] = '\0'; //Appends null character at end of buffer
		s += buf;
	}

	return (n == -1) ? -1 : 0; //-1 failure, 0 success
}