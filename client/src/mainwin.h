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

#include "msort.h"
#include "pagewin.h"

#define HEADERLEN 10
#define MAXBUFFER 4096

struct AlphaStrComp {
 bool operator() (const string & lhs, const string & rhs) 
    { 
    	if(lhs<=rhs)
    	 return true;
        else
        	return false;
    }
 };

class MainWin : public QMainWindow
{
	Q_OBJECT

	public:
		MainWin(int sock, QWidget *parent = 0);
		~MainWin();
		

	private slots:
		void sortByRank();
		void sortByName();
		void showAbout() { QMessageBox::information(this, tr("About"), tr("Client-Server Database search engine.")); }
		void doQuit() { this->close(); }
		void doSearchWord();
		void doSearchOR();
		void doSearchAND();
		void openResult(QListWidgetItem* item);
		void adClicked(QListWidgetItem* item);
		
	private:
		void toLowerCase(string & s);
		bool isValid(string & s) const;
		string padlen(int len);
		int packetlen(int sock);
		list<string> parseInput(string input);

		int sendall(int sock, const char* buf, int *len);
		int recvall(string & s, int sock, int len);

		int sockfd;
		list<string> searchResults;

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

		PageWin 	* pWin;
};

#endif
