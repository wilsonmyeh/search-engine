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

class MainWin : public QMainWindow
{
	Q_OBJECT

	public:
		MainWin(map<string,Set<WebPage*> > & wMap, map<string,WebPage*> & fMap, QWidget *parent = 0);
		~MainWin();
		void toLowerCase(string & s);
		bool isValid(string & s) const;

	private slots:
		void sortByRank();
		void sortByName();
		void showAbout() { QMessageBox::information(this, tr("About"), tr("Dummy search engine with hyperlink support.")); }
		void doQuit() { this->close(); }
		void doSearchWord();
		void doSearchOR();
		void doSearchAND();
		void openResult(QListWidgetItem* item);
		
	private:
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
		
		QGridLayout	* searchLayout;
		QWidget 	* window;

		map<string,WebPage*> & fileLookup;
		map<string,Set<WebPage*> > & wordMap;
		list<WebPage*> searchResults;

		PageWin 	* pWin;
};

#endif
