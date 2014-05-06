#include <deque>
#include <map>
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdexcept>
#include <QApplication>
#include "mainwin.h"

using namespace std;

void toLowerCase(string & s)
{
	for(unsigned int x = 0;x < s.length();x++)
		if(s[x] >= 'A' && s[x] <= 'Z')
			s[x] = (char)(s[x] - 'A' + 'a');
}

struct AlphaStrComp {
 bool operator() (const string & lhs, const string & rhs) 
    { 
    	if(lhs<=rhs)
    	 return true;
        else
        	return false;
    }
 };

int main(int argc, char* argv[])
{
	if(argc < 4) {
		cout << "Arguments: <webpage index> <ad input> <ad output>.\n";
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

    ifile.close();
    ifile.open(argv[2]);
    if(ifile.fail()){
        cerr << "Couldn't open file" << endl;
        return 1;
    }

    //output file
    ofstream ofile(argv[3]);
    if(ofile.fail()){
        cerr << "Couldn't open file" << endl;
        return 1;
    }

    comp_bid cb; 
    map<string,vector<comp_bid>* > compMap;


    int count=0;
    int numLines;
    string line;

    //reads the file line by line, extracts every word and stores it in the struct object. Then push the struct obj into the vector comp
    while (getline(ifile, line))  //while their are lines on the page
    {
        stringstream ss;
        ss << line;
        if(count==0) {
            ss>>numLines;
        }
        else if(count>0 && count <=numLines) { 
            string key;
            ss >> key;
            toLowerCase(key);
            cb.keyword = key;
            ss>>cb.bid;
            ws(ss);
            getline(ss,cb.company);
            cb.hits = 0;
            if(compMap.find(key) == compMap.end()) {
                vector<comp_bid>* temp = new vector<comp_bid>();
                compMap[key] = temp;
            }
            compMap[key]->push_back(cb);
        }

        count ++;
    }


     ifile.close();

	QApplication app(argc, argv);  

	MainWin window(wordMap,fileLookup,compMap);

	window.setWindowTitle("Search Engine");
	window.show();
	
	int execResult = app.exec();
	map<string,double> bill;
	list<string> compNames;
	for(auto & it : compMap)
	{
		vector<comp_bid> & temp = *it.second;
		for(unsigned int i = 0;i < temp.size();i++)
		{
			if(bill.find(temp[i].company) == bill.end()) {
				compNames.push_back(temp[i].company);
				bill[temp[i].company] = 0;
			}
			bill[temp[i].company] += (temp[i].hits * temp[i].bid);
		}
	}
	AlphaStrComp struc;
	merge_sort(compNames,struc);

	for(auto & it : compNames)
	{
		ofile << it << endl;
		ofile << "$" << std::fixed << std::setprecision(2) << bill[it] << endl << endl;
	}
	for(auto & it : compMap)
    	delete it.second;

    return execResult;
}