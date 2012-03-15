
#ifndef __UTIL_H__
#define __UTIL_H__

#include <iostream>
#include <fstream>
#include <string>

#include <stdlib.h>

namespace kCCFLib { 



template <typename keyT, typename valueT>
void dump(node<keyT,valueT> &n, ostream& out=cerr, int ident=0) {

	for (int i=0; i < ident; i++)
		out << "   ";

	out << "| " << n << "," << n.getCounter() << endl;

	typename node<keyT,valueT>::nodesIterator it = n.getNodesIteratorBegin();
	typename node<keyT,valueT>::nodesIterator end = n.getNodesIteratorEnd();

	while (it != end) {
	
		dump(*it, out, ident+1);
		it++;
	}

}

template <typename keyT, typename valueT>
void dump(forest<keyT,valueT> &f, ostream& out=cerr) {

	typename forest<keyT,valueT>::treesIterator it = f.getTreesIteratorBegin();
	typename forest<keyT,valueT>::treesIterator end = f.getTreesIteratorEnd();

	while (it != end) {
	
		if (!it->getParentRef())
			dump(*it, out, 0);

		out << endl << endl;

		it++;
	}

}


template <typename keyT, typename valueT>
void nodeDump(node<keyT, valueT> n, ostream& out) {

	node<keyT, valueT> *ptr = &n;

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


}

#endif