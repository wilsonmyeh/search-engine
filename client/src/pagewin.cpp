#include "pagewin.h"

using namespace std;
PageWin::PageWin(int sock, string & t, string & b, list<string> & out, list<string> & in, QWidget *parent) 
					: QMainWindow(parent), sockfd(sock) 
{
	setWindowTitle(QString::fromStdString(t));

	pageText = new QTextEdit(QString::fromStdString(b));
	inLabel = new QLabel(QString::fromStdString("Incoming"));
	outLabel = new QLabel(QString::fromStdString("Outgoing"));
	inList = new QListWidget();
	outList = new QListWidget();

	for(auto & it : in)
		inList->addItem(QString::fromStdString(it));
	for(auto & it : out)
		outList->addItem(QString::fromStdString(it));

	pageLayout = new QGridLayout;

	window = new QWidget(this);
	window->setLayout(pageLayout);
	this->setCentralWidget(window);

	pageText->setFixedSize(600,300);
	pageLayout->addWidget(pageText,0,0,1,1,Qt::AlignJustify);
	pageLayout->addWidget(inLabel,1,0,Qt::AlignLeft);
	pageLayout->addWidget(inList,2,0,Qt::AlignLeft);
	pageLayout->addWidget(outLabel,1,0,Qt::AlignRight);
	pageLayout->addWidget(outList,2,0,Qt::AlignRight);

	connect(inList,	SIGNAL(itemDoubleClicked(QListWidgetItem*)), this,	SLOT(openResult(QListWidgetItem*)));
	connect(outList,SIGNAL(itemDoubleClicked(QListWidgetItem*)), this,	SLOT(openResult(QListWidgetItem*)));
}

PageWin::~PageWin()
{
	/*
	delete pageText;
	delete inLabel;
	delete outLabel;
	delete inList;
	delete outList;
	
	delete pageLayout;
	delete window;
	*/
}

void PageWin::openResult(QListWidgetItem* item)
{
	string page = item->text().toStdString();
	setWindowTitle(QString::fromStdString(page));

	string output = "3" + padlen(page.length()) + page;
	int len = output.length();
	if(sendall(sockfd,output.c_str(),&len) == -1)
	{
		perror("send");
		return;
	}
	
	string body;
	if(recvall(body,sockfd,packetlen(sockfd)) == -1)
	{
		perror("recv");
		return;
	}

	string input;
	if(recvall(input,sockfd,packetlen(sockfd)) == -1)
	{
		perror("recv");
		return;
	}
	list<string> outlinks = parseInput(input);


	if(recvall(input,sockfd,packetlen(sockfd)) == -1)
	{
		perror("recv");
		return;
	}
	list<string> inlinks = parseInput(input);

	inList->clear();
	outList->clear();
	for(auto & it : inlinks) 
		inList->addItem(QString::fromStdString(it));
	for(auto & it : outlinks) 
		outList->addItem(QString::fromStdString(it));
}

int PageWin::packetlen(int sock)
{
	char len[HEADERLEN];
	int numBytes;
	if((numBytes = recv(sock,len,HEADERLEN,0)) == -1) {
		perror("recv");
		exit(1);
	}
	return atoi(len);
}

string PageWin::padlen(int len)
{
	string s = to_string(len);
	while(s.length() < 10)
		s = "0" + s;
	return s;
}

list<string> PageWin::parseInput(string input)
{
	list<string> inlist;
	int sInd = 0;
	for(unsigned int i = 0;i < input.length()-3;i++)
	{
		if(input.substr(i,3) == ":::")
		{
			inlist.push_back(input.substr(sInd,i));
			sInd = i+3;
			i = i+3;
		}
	}
	inlist.push_back(input.substr(sInd));
	return inlist;
}

int PageWin::sendall(int sock, const char* buf, int *len)
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

int PageWin::recvall(string & s, int sock, int len)
{
	s = "";
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