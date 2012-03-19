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

	int a = 3;
	int &p = a;

	p = 99;

	cout << "a = " << a << endl;
	cout << "p = " << p << endl;

	kNodeChildrenContainerVec< int, node > c;

	cout << "insert 1\n";
	c.insert(1,node(1, "a"));
	cout << "insert 2\n";
	c.insert(2,node(2, "b"));
	cout << "insert 3\n";
	c.insert(3,node(3, "c"));

	cout << "insert 4\n";
	node *n = &c.insert(4, node(4, "pippo"));

	n->val = "ciccio";

	cout << "replace b2\n";
	n = &c.replace(2, node(2, "b2"));

	cout << "insert b22\n";
	n->val = "b22";

	cout << "1: " << (c.find(1))->val << endl;

	for (kNodeChildrenContainerVec< int, node >::iterator it = c.begin(); it != c.end(); it++) {

		node n;
		n = *it;
		cout << n.getKey() << " -> " << n.val << endl;
	}

	return 0;
}
