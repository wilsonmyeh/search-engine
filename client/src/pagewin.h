#ifndef PAGEWIN_H
#define PAGEWIN_H

#include <QWidget>
#include <QMainWindow>
#include <QString>
#include <QTextEdit>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <sstream>
#include <string>

#include "WebPage.h"

class PageWin : public QMainWindow
{
	Q_OBJECT

	public:
		PageWin(map<string,WebPage*> & fMap, WebPage & page, QWidget *parent = 0);
		~PageWin();

	private slots:
		void openResult(QListWidgetItem* item);

	private:
		QTextEdit	* pageText;
		QLabel		* inLabel;
		QLabel		* outLabel;
		QListWidget	* inList;
		QListWidget	* outList;

		QGridLayout	* pageLayout;
		QWidget 	* window;

		map<string,WebPage*> & fileLookup;
};

#endif