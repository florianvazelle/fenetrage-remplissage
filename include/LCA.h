#ifndef LCA_H
#define LCA_H

#include "Node.h"
#include "CustomList.h"

class LCA {
public:
    void build(const List<Node>& Element);
    void update(int y);
    void sort();

    const List<Node>& getList() const { return _list; }
    bool empty() const { return _list.empty(); }

private:
    List<Node> _list;
};

#endif