#include "mainwin.h"

using namespace std;

MainWin::MainWin(string & ip, QWidget *parent) : QMainWindow(parent)
{
	btnQuit = new QPushButton("Terminate");
	addr = new QLineEdit();

	addr->setText("Server Address: " + QString::fromStdString(ip));
	addr->setReadOnly(true);
	addr->setFixedSize(300,25);

	connect(btnQuit,		SIGNAL(clicked()),	this, SLOT(doQuit()));

	layout = new QGridLayout;
	window = new QWidget(this);
	this->setCentralWidget(window);
	window->setLayout(layout);

	layout->addWidget(addr,0,0,Qt::AlignJustify);
	layout->addWidget(btnQuit,1,0,Qt::AlignJustify);
	
}