#include <iostream>
#include <string>


using namespace std;

#include "../dataStructures.h"



class node : public kObjectWithKey<int> {

	public:

	int _key;
	string val;

	node() { _key=-1; }
	node(int k, string s) { _key=k; val=s; }
	int getKey() { return _key; }
};

int main() {

	kNodeChildrenContainerVec< int, node > c;

	c.insert(1,node(1, "a"));
	c.insert(2,node(2, "b"));
	c.insert(3,node(3, "c"));

	node *n = &c.insert(4, node(4, "pippo"));

	n->val = "ciccio";

	n = &c.replace(2, node(2, "b2"));

	n->val = "b22";

	cout << "1: " << (c.find(1))->val << endl;

	for (kNodeChildrenContainerVec< int, node >::iterator it = c.begin(); it != c.end(); it++) {

		node n;
		n = *it;
		cout << n.getKey() << " -> " << n.val << endl;
	}

	return 0;
}
