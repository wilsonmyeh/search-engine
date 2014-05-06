#ifndef MAINWIN_H
#define MAINWIN_H

#include <QWidget>
#include <QMainWindow>
#include <QPushButton>
#include <QMessageBox>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QString>
#include <string>
#include <list>

#include "msort.h"
#include "pagewin.h"
#include "mergesortkevin.h"

struct comp_bid {
    string keyword;
    double bid;
    string company;
    int hits;
};

struct AlphaComp {
	bool operator()(const WebPage* lhs, const WebPage* rhs)
	{
		return lhs->filename().compare(rhs->filename()) <= 0;
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



class MainWin : public QMainWindow
{
	Q_OBJECT

	public:
		MainWin(map<string,Set<WebPage*> > & wMap, map<string,WebPage*> & fMap, map<string,vector<comp_bid>* > & cMap, QWidget *parent = 0);
		~MainWin();
		

	private slots:
		void sortByRank();
		void sortByName();
		void showAbout() { QMessageBox::information(this, tr("About"), tr("Dummy search engine with hyperlink support.")); }
		void doQuit() { this->close(); }
		void doSearchWord();
		void doSearchOR();
		void doSearchAND();
		void openResult(QListWidgetItem* item);
		void adClicked(QListWidgetItem* item);
		
	private:
		void toLowerCase(string & s);
		bool isValid(string & s) const;
		void displayAds(Set<string> & input);

		QPushButton * btnQuit;
		QPushButton * btnAbout;
		QPushButton * btnSearchWord;
		QPushButton * btnSearchOR;
		QPushButton * btnSearchAND;
		QPushButton * btnSortName;
		QPushButton * btnSortRank;

		QLineEdit	* txtWord;
		QLineEdit	* txtOR;
		QLineEdit	* txtAND;

		QListWidget * resultList;
		QListWidget * adList;
		
		QGridLayout	* searchLayout;
		QWidget 	* window;

		map<string,WebPage*> & fileLookup;
		map<string,Set<WebPage*> > & wordMap;
		list<WebPage*> searchResults;
		map<string,vector<comp_bid>* > compMap;

		map<string,comp_bid*> curAds;

		PageWin 	* pWin;
};

#endif
