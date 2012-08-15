
#include <stdio.h>
#include <iostream>

inline void BM_inc_nodes_created() { }
inline void BM_inc_empty_nodes_created() { }
inline void BM_inc_nodes_copied() { }
inline void BM_inc_forests_copied() { }

#include "../forest.h"
#include "../util.h"

class simpleVal : public ihppObjectWithKey<const char*> {

	const char *key;

public:
	simpleVal(const char *k) { key=k; }
	const char *getKey() { return key; }
};

typedef node<const char*> nn;

#define N(f,c) (nn(#f,new simpleVal(#f), c))
#define add(n1,n2) ((n1)->addChildByVal(n2))

int main(int argc, char ** argv) {

	nn tree1 = N(a,2);

	nn *b1 = add(&tree1,N(b,1));

	add(b1,N(c,5));
	add(b1,N(d,4));

	nn tree2 = N(a,1);
	nn *temp = add(&tree2, N(b,1));
	add(&tree2, N(c, 6));

	add(temp, N(d,3));

	dump(tree1);
	dump(tree2);

	cout << endl << endl;

	forest<const char*> f = forest<const char*>::join(tree1, tree2);

	dump(f);

	getchar();
	return 0;
}