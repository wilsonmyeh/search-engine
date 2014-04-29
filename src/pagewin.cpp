#include "pagewin.h"

using namespace std;

PageWin::PageWin(map<string,WebPage*> & fMap, WebPage & page, QWidget *parent) :
				QMainWindow(parent), fileLookup(fMap) 
{
	setWindowTitle(QString::fromStdString(page.filename()));

	stringstream ss;
	ss << page;
	pageText = new QTextEdit(QString::fromStdString(ss.str()));
	inLabel = new QLabel(QString::fromStdString("Incoming"));
	outLabel = new QLabel(QString::fromStdString("Outgoing"));
	inList = new QListWidget();
	outList = new QListWidget();

	Set<WebPage*> & iList = page.allIncomingLinks();
	for(auto & it : iList)
		inList->addItem(QString::fromStdString(it->filename()));
	Set<WebPage*> & oList = page.allOutgoingLinks();
	for(auto & it : oList)
		outList->addItem(QString::fromStdString(it->filename()));

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
	WebPage* page = fileLookup.at(item->text().toStdString());
	stringstream ss;
	ss << *page;
	pageText->setText(QString::fromStdString(ss.str()));
	setWindowTitle(QString::fromStdString(page->filename()));

	inList->clear();
	outList->clear();
	Set<WebPage*> & iList = page->allIncomingLinks();
	for(auto & it : iList) 
		inList->addItem(QString::fromStdString(it->filename()));
	Set<WebPage*> & oList = page->allOutgoingLinks();
	for(auto & it : oList) 
		outList->addItem(QString::fromStdString(it->filename()));
}