#include <QApplication>

#include "mainwin.h"

#define MAXBUFFER 4096
#define HEADERLEN 10

using namespace std;

//gets address in ipv4 or ipv6 format
void* get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char* argv[])
{
	if(argc < 3) {
		cout << "Arguments: <IP> <Port>.\n";
		return 1;
	}

    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    char s[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if(getaddrinfo(argv[1], argv[2], &hints, &servinfo) != 0) {
		perror("Bad IP/Port");
		return 1;
	}

	//Bind to the first socket/port that works
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
        }

        break;
	}
	
	if (p == NULL) {
		cout << ("Failed to connect") << endl;
		return 1;
	}

	
	inet_ntop(p->ai_family, get_in_addr((struct sockaddr*)&p), s, sizeof s);
	cout << "Connected to " << s;

	freeaddrinfo(servinfo); //Connected and done!

	QApplication app(argc, argv);
	MainWin window(sockfd);

	window.setWindowTitle("Search Engine");
	window.show();

    return app.exec();
}

