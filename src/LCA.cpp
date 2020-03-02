#include "LCA.h"

void LCA::build(const List<Node>& Element) {
    List<Node>::const_iterator iter = Element.begin();
    iter++;
        
    for (List<Node>::const_iterator it = iter; it != Element.end(); ++it) {
        _list.insertInTheList(*it);
    }
}

void LCA::update(int y) {
    List<Node>::iterator it = _list.begin();
    while (it != _list.end()) {
        if (y >= it->ymax) {
            it = _list.erase(it);
        } else {
            it->xmin += it->invertCoef;
            ++it;
        }
    }
}

void LCA::sort() {
    List<Node> tmp;
    List<Node>::iterator it = _list.begin();
    while (it != _list.end()) {
        tmp.insertInTheList(*it);
        it = _list.erase(it);
    }
    _list = tmp;
}