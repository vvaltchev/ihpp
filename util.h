#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <cstdlib>

#include "forest.h"

vector<string> splitString(const string& s, char sep);

inline char *duplicate_string(const string& s)
{
    char *ptr = new char [s.size()+1];
    strcpy(ptr, s.c_str());
    return ptr;
}

template <typename keyT>
void dump(node<keyT> &n, ostream& out=cerr, int ident=0) {

    for (int i=0; i < ident; i++)
        out << "   ";

    out << "| " << n << "," << n.getCounter() << endl;

    for (auto it = n.getNodesIteratorBegin(); it != n.getNodesIteratorEnd(); it++) {
        dump(*it, out, ident+1);
    }
}

template <typename keyT>
void dump(forest<keyT> &f, ostream& out=cerr) {

    for (auto it = f.getTreesIteratorBegin(); it != f.getTreesIteratorEnd(); it++) {

        if (!it->getParentRef())
            dump(*it, out, 0);

        out << endl << endl;
    }
}

template <typename keyT>
void nodeDump(node<keyT> n, ostream& out) {

    node<keyT> *ptr = &n;

    out << "\"" << n << "," << n.getCounter();
    out << " (";

    if (ptr->getParentRef()) {

        out << *ptr->getParentRef();
        ptr=ptr->getParentRef();

        while (ptr->getParentRef()) {

            out << ",";
            out << *ptr->getParentRef();
            ptr=ptr->getParentRef();
        }

    } else {

        out << "(null)";
    }

    out << ") ";
    out << "\"";
}
