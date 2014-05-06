#include "mainwin.h"
#include "pagerank.h"

using namespace std;

MainWin::MainWin(map<string,Set<WebPage*> > & wMap, map<string,WebPage*> & fMap, map<string,vector<comp_bid>* > & cMap, QWidget *parent) :
				QMainWindow(parent), fileLookup(fMap),wordMap(wMap),compMap(cMap),pWin(NULL)
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
	adList->setFixedSize(250,250);


	searchLayout->addWidget(txtWord,0,0,Qt::AlignLeft);
	searchLayout->addWidget(btnSearchWord,0,0,Qt::AlignRight);
	searchLayout->addWidget(txtOR,1,0,Qt::AlignLeft);
	searchLayout->addWidget(btnSearchOR,1,0,Qt::AlignRight);
	searchLayout->addWidget(txtAND,2,0,Qt::AlignLeft);
	searchLayout->addWidget(btnSearchAND,2,0,Qt::AlignRight);

	searchLayout->addWidget(btnSortName,3,0,Qt::AlignLeft);
	searchLayout->addWidget(btnSortRank,3,0,Qt::AlignRight);
	searchLayout->addWidget(resultList,4,0,Qt::AlignJustify);
	searchLayout->addWidget(adList,5,0,Qt::AlignLeft);

	searchLayout->addWidget(btnAbout,5,0,Qt::AlignRight);
	searchLayout->addWidget(btnQuit,5,0,Qt::AlignRight);

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
	Set<string> inputWords;

	Set<WebPage*> results;
	string query = txtWord->text().toStdString();
	toLowerCase(query);
	if(!isValid(query))
	{
		QMessageBox::information(this, tr("Error"), tr("Please use only spaces and alphanumeric characters."));
		return;
	}

	try{	results = wordMap.at(query);	}
	catch (exception e) { }

	if(results.empty())
		QMessageBox::information(this, tr("Warui"), tr("No matches found."));
	else
	{
		resultList->clear();
		searchResults.clear();
		for(auto& it : results) {
			resultList->addItem(QString::fromStdString(it->filename()));
			searchResults.push_back(it);
		}
	}

	inputWords.insert(query);
	displayAds(inputWords);
}

void MainWin::doSearchOR()
{
	Set<string> inputWords;

	Set<WebPage*> results;
	string query = txtOR->text().toStdString();
	toLowerCase(query);
	if(!isValid(query))
	{
		QMessageBox::information(this, tr("Error"), tr("Please use only spaces and alphanumeric characters."));
		return;
	}

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

	if(results.empty())
		QMessageBox::information(this, tr("Warui"), tr("No matches found."));
	else
	{
		resultList->clear();
		searchResults.clear();
		for(auto& it : results) {
			resultList->addItem(QString::fromStdString(it->filename()));
			searchResults.push_back(it);
		}
	}

	displayAds(inputWords);
}

void MainWin::doSearchAND()
{
	Set<string> inputWords;

	Set<WebPage*> results;
	string query = txtAND->text().toStdString();
	toLowerCase(query);
	if(!isValid(query))
	{
		QMessageBox::information(this, tr("Error"), tr("Please use only spaces and alphanumeric characters."));
		return;
	}
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

	if(results.empty())
		QMessageBox::information(this, tr("Warui"), tr("No matches found."));
	else
	{
		resultList->clear();
		for(auto& it : results) {
			searchResults.clear();
			resultList->addItem(QString::fromStdString(it->filename()));
			searchResults.push_back(it);
		}
	}

	displayAds(inputWords);

}

void MainWin::displayAds(Set<string> & input)
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

	adList->clear();
	curAds.clear();

	SortBids struc;
	merge_sort(bids,struc);
	Set<string> compNames;
	for(auto & it : bids)
	{
		if(compNames.insert(it->company).second)
		{
			curAds[it->company] = it;
			adList->addItem(QString::fromStdString(it->company));
		}
	}

}

void MainWin::sortByRank()
{
	pageRank(searchResults);

	RankComp comp;
	merge_sort(searchResults,comp);
	resultList->clear();
	for(auto& it : searchResults) { //TODO: Replace "???" with sorted list
		resultList->addItem(QString::fromStdString(it->filename()));
	}

}

void MainWin::sortByName()
{
	AlphaComp comp;
	merge_sort(searchResults,comp);
	resultList->clear();
	for(auto& it : searchResults) {
		resultList->addItem(QString::fromStdString(it->filename()));
	}
}

void MainWin::openResult(QListWidgetItem* item)
{
	if(pWin != NULL)
		delete pWin;
	string page = item->text().toStdString();
	pWin = new PageWin(fileLookup,*fileLookup.at(page));
	pWin->show();
}

void MainWin::adClicked(QListWidgetItem* item)
{
	string ad = item->text().toStdString();
	QMessageBox::information(this, QString::fromStdString(ad), QString::fromStdString("Welcome to " + ad + "!"));
	curAds[ad]->hits++; 
	
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
