#include <iostream>
#include <fstream>
#include <set>
#include <sstream>
#include <stdexcept>

using namespace std;

void crawl(ofstream & ofile, set<string> & pages, string filen);

int main(int argc, char* argv[])
{
	ifstream ifile;
	ifile.open(argv[1]);
	ofstream ofile;
	ofile.open(argv[2]);

	if(argc < 3)
	{
		cout << "Please enter 2 arguments: <Input> <Output>\n";
		return 1;
	}

	set<string> pages;

	if(ifile.fail()) {
		cout << "File not found.\n";
		return 1;
	}

	string n;
	ifile >> n;
	while(!ifile.fail()) {
		if(pages.insert(n).second) //If inserted successfully
		{
			ofile << n << endl;
			try {
				crawl(ofile,pages,n);
			}
			catch(exception e) {
				cout << e.what() << endl;
			}
		}
		ifile >> n;
	}

	return 0;
}

void crawl(ofstream & ofile, set<string> & pages, string filen)
{
	ifstream ifile;
	ifile.open(filen.c_str());
	if(ifile.fail()) {
		throw std::invalid_argument("Invalid link in file " + filen + ".\n");
	}

	string temp;
	stringstream rawPage;

	getline(ifile,temp);
	while(!ifile.fail()) {
		rawPage << temp << endl;
		getline(ifile,temp);
	}
	string content = rawPage.str();

	int anchorInd = -1;
	int anchorLen = -1;
	int filenameInd = -1;
	for(unsigned int x = 0;x < content.length();x++)
	{
		if(filenameInd != -1 && content[x] == ')')
		{
			string filename = content.substr(filenameInd, x - filenameInd);
			if(pages.insert(filename).second) //Insert Successful = No Duplicate
			{
				ofile << filename << endl;
				try {
					crawl(ofile,pages,filename);
				}
				catch(exception e) {
					cout << e.what() << endl;
				}
			}

			string anchor = content.substr(anchorInd, anchorLen);
			content.replace(anchorInd-1,x - anchorInd + 2,anchor); //Replace MD link with anchor
			x -= (filename.length() + 4); //Index is now at end of anchor

			anchorInd = -1;
			anchorLen = -1;
			filenameInd = -1;


		}

		if(anchorInd != -1 && content[x] == ']')
		{
			anchorLen = x - anchorInd;
			if(content[x+1] != '(') {
				anchorInd = -1;
				anchorLen = -1;
				filenameInd = -1;
			}
		}

		if(anchorLen != -1 && content[x] == '(')
		{
			filenameInd = x+1;
		}

		if(content[x] == '[')
		{
			anchorInd = x+1;
			anchorLen = -1;
			filenameInd = -1;
		}
	}
}