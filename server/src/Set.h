#ifndef SET_H
#define SET_H

// necessary includes can go here
#include <stdexcept>
#include <iostream>
#include <set>

using namespace std;

template <class T>
class Set : public set<T> {
  public: 
    Set<T> setIntersection (const Set<T> & other) const
    {
      Set<T> temp;
      for(auto& it : *this) //Loops through this
      {
        if(other.find(it) != other.end()) { //If other contains the element, add to temp
          try {
            temp.insert(it);
          }
          catch (exception e) { }
        }
      }
      return temp;
    }

    Set<T> setUnion (const Set<T> & other) const
    {
      Set<T> temp(other); //Loads ALL the other things into temp
      for(auto& it : *this)
      {
        try { 
          temp.insert(it); //Add ALL the things!
        }
        catch (exception e) {}
      }
      return temp;
    }

  private:
};

#endif