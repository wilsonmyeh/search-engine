#include "mainwin.h"

using namespace std;

MainWin::MainWin(string & ip, QWidget *parent) : QMainWindow(parent)
{

	addr->setText("Server Address: " + QString::fromStdString(ip));
	addr->setReadOnly(true);

	connect(btnQuit,		SIGNAL(clicked()),	this, SLOT(doQuit()));

	addr->show();
	btnQuit->show();
}