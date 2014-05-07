#include "mainwin.h"

using namespace std;

MainWin::MainWin(int sock, QWidget *parent) : QMainWindow(parent), sockfd(sock), pWin(NULL)
{
	btnQuit			= new QPushButton("&Quit");
	btnAbout		= new QPushButton("&About");
	btnSearchWord	= new QPushButton("Search Word");
	btnSearchOR 	= new QPushButton("Search OR");
	btnSearchAND 	= new QPushButton("Search AND");
	btnSortName		= new QPushButton("Sort Name");
	btnSortRank		= new QPushButton("Sort Rank");
	
	txtWord 	= new QLineEdit;
	txtOR 		= new QLineEdit;
	txtAND 		= new QLineEdit;

	resultList = new QListWidget;
	adList = new QListWidget;

	searchLayout = new QGridLayout;

	window = new QWidget(this);
	window->setLayout(searchLayout);
	this->setCentralWidget(window);

	txtWord->setFixedSize(400,25);
	txtOR->setFixedSize(400,25);
	txtAND->setFixedSize(400,25);
	btnSearchWord->setFixedSize(100,25);
	btnSearchOR->setFixedSize(100,25);
	btnSearchAND->setFixedSize(100,25);

	btnSortName->setFixedSize(250,25);
	btnSortRank->setFixedSize(250,25);

	resultList->setFixedSize(500,300);
	adList->setFixedSize(500,100);


	searchLayout->addWidget(txtWord,0,0,Qt::AlignLeft);
	searchLayout->addWidget(btnSearchWord,0,0,Qt::AlignRight);
	searchLayout->addWidget(txtOR,1,0,Qt::AlignLeft);
	searchLayout->addWidget(btnSearchOR,1,0,Qt::AlignRight);
	searchLayout->addWidget(txtAND,2,0,Qt::AlignLeft);
	searchLayout->addWidget(btnSearchAND,2,0,Qt::AlignRight);

	searchLayout->addWidget(btnSortName,3,0,Qt::AlignLeft);
	searchLayout->addWidget(btnSortRank,3,0,Qt::AlignRight);
	searchLayout->addWidget(resultList,4,0,Qt::AlignJustify);
	searchLayout->addWidget(adList,5,0,Qt::AlignJustify);

	searchLayout->addWidget(btnAbout,6,0,Qt::AlignLeft);
	searchLayout->addWidget(btnQuit,6,0,Qt::AlignRight);

	connect(btnAbout,		SIGNAL(clicked()),	this, SLOT(showAbout()));
	connect(btnQuit,		SIGNAL(clicked()),	this, SLOT(doQuit()));
	connect(btnSearchWord,	SIGNAL(clicked()),	this, SLOT(doSearchWord()));
	connect(btnSearchOR,	SIGNAL(clicked()),	this, SLOT(doSearchOR()));
	connect(btnSearchAND,	SIGNAL(clicked()),	this, SLOT(doSearchAND()));
	connect(btnSortRank,	SIGNAL(clicked()),	this, SLOT(sortByRank()));
	connect(btnSortName,	SIGNAL(clicked()),	this, SLOT(sortByName()));

	connect(resultList,		SIGNAL(itemDoubleClicked(QListWidgetItem*)),	this,	SLOT(openResult(QListWidgetItem*)));
	connect(adList,			SIGNAL(itemClicked(QListWidgetItem*)),			this,	SLOT(adClicked(QListWidgetItem*)));
}

MainWin::~MainWin()
{
	/*
	delete btnQuit;
	delete btnAbout;
	delete btnSearchWord;
	delete btnSearchOR;
	delete btnSearchAND;

	delete txtWord;
	delete txtOR;
	delete txtAND;

	delete resultList;
	delete window;
	
	delete searchLayout;

	delete pWin;
	*/
}

void MainWin::doSearchWord()
{
	
	string query = txtWord->text().toStdString();
	toLowerCase(query);
	if(!isValid(query))
	{
		QMessageBox::information(this, tr("Error"), tr("Please use only spaces and alphanumeric characters."));
		return;
	}

	string output = "0" + padlen(query.length()) + query;
	int len = output.length();
	if(sendall(sockfd,output.c_str(),&len) == -1)
	{
		perror("send");
		return;
	}

	string input;

	if(recvall(input,sockfd,packetlen(sockfd)) == -1)
	{
		perror("recv");
		return;
	}

	if(input.empty())
		QMessageBox::information(this, tr("Warui"), tr("No matches found."));
	else
	{
		searchResults = parseInput(input);
		resultList->clear();
		for(auto& it : searchResults)
			resultList->addItem(QString::fromStdString(it));
	}

	if(recvall(input,sockfd,packetlen(sockfd)) == -1)
	{
		perror("ad recv");
		return;
	}
	list<string> ads = parseInput(input);
	for(auto & it : ads)
		adList->addItem(QString::fromStdString(it));
}

void MainWin::doSearchOR()
{
	string query = txtOR->text().toStdString();
	toLowerCase(query);
	if(!isValid(query))
	{
		QMessageBox::information(this, tr("Error"), tr("Please use only spaces and alphanumeric characters."));
		return;
	}

	string output = "1" + padlen(query.length()) + query;
	int len = output.length();
	if(sendall(sockfd,output.c_str(),&len) == -1)
	{
		perror("send");
		return;
	}

	string input;

	if(recvall(input,sockfd,packetlen(sockfd)) == -1)
	{
		perror("recv");
		return;
	}

	if(input.empty())
		QMessageBox::information(this, tr("Warui"), tr("No matches found."));
	else
	{
		searchResults = parseInput(input);
		resultList->clear();
		for(auto& it : searchResults)
			resultList->addItem(QString::fromStdString(it));
	}

	if(recvall(input,sockfd,packetlen(sockfd)) == -1)
	{
		perror("ad recv");
		return;
	}
	list<string> ads = parseInput(input);
	for(auto & it : ads)
		adList->addItem(QString::fromStdString(it));
}

void MainWin::doSearchAND()
{
	string query = txtAND->text().toStdString();
	toLowerCase(query);
	if(!isValid(query))
	{
		QMessageBox::information(this, tr("Error"), tr("Please use only spaces and alphanumeric characters."));
		return;
	}

	string output = "2" + padlen(query.length()) + query;
	int len = output.length();
	if(sendall(sockfd,output.c_str(),&len) == -1)
	{
		perror("send");
		return;
	}

	string input;

	if(recvall(input,sockfd,packetlen(sockfd)) == -1)
	{
		perror("recv");
		return;
	}

	if(input.empty())
		QMessageBox::information(this, tr("Warui"), tr("No matches found."));
	else
	{
		searchResults = parseInput(input);
		resultList->clear();
		for(auto& it : searchResults)
			resultList->addItem(QString::fromStdString(it));
	}

	if(recvall(input,sockfd,packetlen(sockfd)) == -1)
	{
		perror("ad recv");
		return;
	}
	list<string> ads = parseInput(input);
	for(auto & it : ads)
		adList->addItem(QString::fromStdString(it));

}

void MainWin::sortByRank()
{
	resultList->clear();
	for(auto& it : searchResults) { //TODO: Replace "???" with sorted list
		resultList->addItem(QString::fromStdString(it));
	}

}

void MainWin::sortByName()
{
	list<string> temp = searchResults;
	AlphaStrComp comp;
	merge_sort(temp,comp);
	resultList->clear();
	for(auto& it : temp) {
		resultList->addItem(QString::fromStdString(it));
	}
}

void MainWin::openResult(QListWidgetItem* item)
{
	if(pWin != NULL)
		delete pWin;
	string page = item->text().toStdString();

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

	pWin = new PageWin(sockfd,page,body,outlinks,inlinks);
	pWin->show();
}

void MainWin::adClicked(QListWidgetItem* item)
{
	string ad = item->text().toStdString();
	QMessageBox::information(this, QString::fromStdString(ad), QString::fromStdString("Welcome to " + ad + "!"));
	
	string output = "4" + padlen(ad.length()) + ad;
	int len = output.length();
	if(sendall(sockfd,output.c_str(),&len) == -1)
	{
		perror("send");
		return;
	}
	
}

int MainWin::packetlen(int sock)
{
	char len[HEADERLEN];
	int numBytes;
	if((numBytes = recv(sock,len,HEADERLEN,0)) == -1) {
		perror("recv");
		exit(1);
	}
	return atoi(len);
}

string MainWin::padlen(int len)
{
	string s = to_string(len);
	while(s.length() < 10)
		s = "0" + s;
	return s;
}

void MainWin::toLowerCase(string & s)
{
	for(unsigned int x = 0;x < s.length();x++)
		if(s[x] >= 'A' && s[x] <= 'Z')
			s[x] = (char)(s[x] - 'A' + 'a');
}

bool MainWin::isValid(string & s) const
{
	for(unsigned int x = 0;x < s.length();x++) //Checks that all characters are alphanumeric
	{
		if(!(s[x] == ' ' || (s[x] >= 'a' && s[x] <= 'z') || (s[x] >= '0' && s[x] <= '9')))
			return false;
	}
	return true;
}

list<string> MainWin::parseInput(string input)
{
	list<string> inlist;
	int sInd = 0;
	for(unsigned int i = 0;i < input.length()-3;i++)
	{
		if(input.substr(i,3) == ":::")
		{
			inlist.push_back(input.substr(sInd,i-sInd));
			sInd = i+3;
			i = i+3;
		}
	}
	inlist.push_back(input.substr(sInd,input.length()-sInd-3)); //Get rid of last :::
	return inlist;
}

int MainWin::sendall(int sock, const char* buf, int *len)
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

int MainWin::recvall(string & s, int sock, int len)
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