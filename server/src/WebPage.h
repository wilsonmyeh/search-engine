#ifndef WEBPAGE_H
#define WEBPAGE_H

#include <string>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <fstream>
//#include <regex>
#include <map>
#include <list>
#include "Set.h"

using namespace std;

class WebPage {
  public:
    WebPage ();    // constructor that creates an empty page
    WebPage (string filen);
      /* constructor that initializes the page from the given file.
         Should throw an exception if the file does not exist
         or has other problems. */
    WebPage (const WebPage & other);
    ~WebPage ();   // destructor

    Set<string> allWords () const;
      /* Returns a set containing all individual words on the web page. */   

    friend ostream & operator<< (ostream & os, const WebPage & page);
      /* Declares an operator we can use to print the web page. */

    bool operator== (const WebPage & rhs);

    WebPage & operator= (const WebPage & other);
    void parse(map<string, WebPage*> & fileLookup);

    void setRank(double val);
    double getRank() const;

    string filename() const;
    Set<WebPage*> & allOutgoingLinks();
    Set<WebPage*> & allIncomingLinks();
    void addIncomingLink(WebPage* start);
    void addOutgoingLink(WebPage* target);

  private:
    string content;
    string fName;
    Set<WebPage*> outgoingLinks;
    Set<WebPage*> incomingLinks;
    list<WebPage*> searchResults;
    double pageRank;
    // you get to decide what goes here.
};

#endif