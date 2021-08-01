#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <cstdlib>

#include "forest.h"

std::vector<std::string> splitString(const std::string& s, char sep);

inline char *duplicate_string(const std::string& s)
{
    char *ptr = new char [s.size()+1];
    strcpy(ptr, s.c_str());
    return ptr;
}

template <typename keyT>
void dump(node<keyT> &n, std::ostream& out = std::cerr, int ident=0) {

    for (int i=0; i < ident; i++)
        out << "   ";

    out << "| " << n << "," << n.getCounter() << std::endl;

    for (auto& child : n)
        dump(child, out, ident+1);
}

template <typename keyT>
void dump(forest<keyT> &forest, std::ostream& out = std::cerr) {

    for (auto& tree : forest) {

        if (!tree.getParentRef())
            dump(tree, out, 0);

        out << std::endl << std::endl;
    }
}

template <typename keyT>
void nodeDump(node<keyT> n, std::ostream& out) {

    node<keyT> *ptr = &n;

    out << "\"" << n << "," << n.getCounter();
    out << " (";

    if (ptr->getParentRef()) {

        out << *ptr->getParentRef();
        ptr = ptr->getParentRef();

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
