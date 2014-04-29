#include <deque>
#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <QApplication>
#include "mainwin.h"

using namespace std;

int main(int argc, char* argv[])
{
	if(argc < 2) {
		cout << "Please input a filename.\n";
		return 1;
	}

	deque<WebPage> pages;
	map<string,WebPage*> fileLookup;

	ifstream ifile;
	ifile.open(argv[1]);
	if(ifile.fail()) {
		cout << "File not found.\n";
		return 1;
	}

	string n;
	ifile >> n;
	while(!ifile.fail()) {
		pages.push_front(WebPage(n));
		fileLookup.emplace(n, &(pages.front()));
		ifile >> n;
	}

	for(auto& it : pages) //Parses links from each page
		it.parse(fileLookup);

	map<string,Set<WebPage*> > wordMap;
	for(unsigned int x = 0;x < pages.size();x++) //Goes through each page, associating it with each of its words
	{
		Set<string> temp = pages[x].allWords();
		for(auto& it : temp)
		{
			try {
				wordMap.emplace(it,Set<WebPage*>()); //Initializes new set for keyword
			}
			catch(exception e) { }
			try {
					wordMap.at(it).emplace(&pages[x]); //Adds page to Set
			}
			catch(exception e) { }
		}
	}

	QApplication app(argc, argv);  

	MainWin window(wordMap,fileLookup);

	window.setWindowTitle("Search Engine");
	window.show();
	
	return app.exec();
}