#ifndef MAINWIN_H
#define MAINWIN_H

#include <QWidget>
#include <QMainWindow>
#include <QPushButton>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QGridLayout>
#include <QString>
#include <string>
#include <list>

class MainWin : public QMainWindow
{
	Q_OBJECT

	public:
		MainWin(std::string & ip, QWidget *parent = 0);
		~MainWin() { };

	private slots:
		void doQuit() { this->close(); }

	private:
		QPushButton * btnQuit;
		QLineEdit	* addr;
		QWidget 	* window;
		QGridLayout * layout;
};

#endif