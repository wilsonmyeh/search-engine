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
#include <string>
#include <list>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <signal.h>
#include <arpa/inet.h>
#include <pthread.h>

#define HEADERLEN 10
#define MAXBUFFER 4096

using namespace std;

class PageWin : public QMainWindow
{
	Q_OBJECT

	public:
		PageWin(int sock, string & t, string & b, list<string> & out, list<string> & in, QWidget *parent = 0);
		~PageWin();

	private slots:
		void openResult(QListWidgetItem* item);

	private:
		int packetlen(int sock);
		string padlen(int len);
		list<string> parseInput(string input);
		
		int sendall(int sock, const char* buf, int *len);
		int recvall(string & s, int sock, int len);

		QTextEdit	* pageText;
		QLabel		* inLabel;
		QLabel		* outLabel;
		QListWidget	* inList;
		QListWidget	* outList;

		QGridLayout	* pageLayout;
		QWidget 	* window;

		int sockfd;

};

#endif