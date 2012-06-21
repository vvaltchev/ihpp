
#ifndef __UTIL_H__
#define __UTIL_H__

#include <iostream>
#include <fstream>
#include <string>

#include <string.h>
#include <stdlib.h>

#include "forest.h"

inline const char *duplicate_string(string &s) {

	char *ptr = new char [s.size()+1];

	strcpy(ptr, s.c_str());

	return ptr;
}

template <typename keyT>
void dump(node<keyT> &n, ostream& out=cerr, int ident=0) {

	for (int i=0; i < ident; i++)
		out << "   ";

	out << "| " << n << "," << n.getCounter() << endl;

	typename node<keyT>::nodesIterator it = n.getNodesIteratorBegin();
	typename node<keyT>::nodesIterator end = n.getNodesIteratorEnd();


	while (it != end) {

		dump(*it, out, ident+1);
		it++;
	}
}

template <typename keyT>
void dump(forest<keyT> &f, ostream& out=cerr) {

	typename forest<keyT>::treesIterator it = f.getTreesIteratorBegin();
	typename forest<keyT>::treesIterator end = f.getTreesIteratorEnd();


	while (it != end) {

		if (!it->getParentRef())
			dump(*it, out, 0);
	
		out << endl << endl;
		it++;
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


#endif