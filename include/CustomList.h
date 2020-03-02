#ifndef CUSTOMLIST_H
#define CUSTOMLIST_H

#include <list>

template <typename T>
class List : public std::list<T> {
public:
    void insertInTheList(const T& item) {
        typename std::list<T>::iterator itr = this->begin();
        while ((itr != this->end()) && (itr->xmin < item.xmin)) itr++;
        this->insert(itr, item);
    }
};

#endif