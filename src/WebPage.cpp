#include "WebPage.h"

WebPage::WebPage() { }

WebPage::WebPage(string filen)
{
	ifstream ifile;
	ifile.open(filen);
	if(ifile.fail())
		throw std::invalid_argument("File not found.");

	string temp;
	stringstream rawPage;

	getline(ifile,temp);
	while(!ifile.fail()) {
		rawPage << temp << "\n";
		getline(ifile,temp);
	}
	content = rawPage.str();
	fName = filen;
	pageRank = -1;
}
WebPage::WebPage (const WebPage & other)
{
	this->content = other.content;
	this->fName = other.fName;
	this->pageRank = other.pageRank;
	this->outgoingLinks = other.outgoingLinks;
	this->incomingLinks = other.incomingLinks;
	this->searchResults = other.searchResults;
}

WebPage::~WebPage() { }

Set<string> WebPage::allWords() const
{
	Set<string> words;
	stringstream ss;
	/*	Still no proper regex support...
	regex reg("[\\W_]",regex::ECMAScript);
	string repl = " ";

	cout << "Got Here\n";

	string s = std::regex_replace(rawPage.str(),reg,repl);
	*/
	string s = content;
	for(unsigned int x = 0;x < s.length();x++) //toLowerCase, replace all [\\W_] with whitespace.
	{
		if(s[x] >= 'A' && s[x] <= 'Z')
			s[x] = (char)(s[x] - 'A' + 'a');
		if(!((s[x] >= 'a' && s[x] <= 'z') || (s[x] >= '0' && s[x] <= '9')))
			s[x] = ' ';
	}
	
	ss << s;

	string temp;

	ss >> temp;
	while(!ss.fail())
	{
		try {	words.insert(temp);	} //Extracts each word, separated by whitespace.
		catch (exception e) {}
		ss >> temp;
	}
	return words;
}

ostream & operator<< (ostream & os, const WebPage & page)
{
	os << page.content;
	return os;
}

bool WebPage::operator== (const WebPage & rhs)
{
	return this->content == rhs.content;
}

WebPage & WebPage::operator= (const WebPage & other)
{
	this->content = other.content;
	return *this;
}

void WebPage::parse(map<string, WebPage*> & fileLookup)
{
	int anchorInd = -1;
	int anchorLen = -1;
	int filenameInd = -1;
	for(unsigned int x = 0;x < content.length();x++)
	{
		if(filenameInd != -1 && content[x] == ')')
		{
			string filename = content.substr(filenameInd, x - filenameInd);
			WebPage* other = fileLookup.at(filename);
			this->addOutgoingLink(other);
			other->addIncomingLink(this);

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

void WebPage::setRank(double val) { pageRank = val; }

double WebPage::getRank() const { return pageRank; }

string WebPage::filename() const { return fName; }

Set<WebPage*> & WebPage::allOutgoingLinks() { return outgoingLinks; }

Set<WebPage*> & WebPage::allIncomingLinks() { return incomingLinks; }

void WebPage::addIncomingLink(WebPage* start) { incomingLinks.insert(start); }

void WebPage::addOutgoingLink(WebPage* target) { outgoingLinks.insert(target); }