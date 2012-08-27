
#include <stdio.h>
#include <iostream>

inline void BM_inc_nodes_created() { }
inline void BM_inc_empty_nodes_created() { }
inline void BM_inc_nodes_copied() { }
inline void BM_inc_forests_copied() { }

#include "test_data_structs.h"
#include "test_traceobj.h"


typedef node<const char*> nn;
typedef const char *keyT;

///////////////////////////////////////////////////

testCtx<const char*> ctx;
unsigned int k_value=2;

void call_handler(testFuncObj *f, int n=1) {

	ShadowStackItemType<keyT> s;

	for (int i=0; i < n; i++) {
		
		s = ctx.shadowstack.top();
		traceObject_generic(f, &ctx, s.treeTop, s.treeBottom, false, k_value);
	}

	ctx.shadowstack.push(ShadowStackItemType<keyT>(s.treeTop, s.treeBottom));
}

void ret_handler() {

	ctx.shadowstack.pop();
}


#define N(f,c) (nn(#f,new simpleVal(#f), c))
#define add(n1,n2) ((n1)->addChildByVal(n2))
#define call(x) call_handler((x))
#define call2(x,n) call_handler((x),(n))
#define ret() ret_handler()

template <typename keyT>
void joinchildren2(node<keyT> &t1, node<keyT> &t2) {

	node<keyT> *t;
	typename node<keyT>::nodesIterator it;

	t1.setCounter(t1.getCounter() + t2.getCounter());

	for (it = t2.getNodesIteratorBegin(); it != t2.getNodesIteratorEnd(); it++) {
	
		t = t1.getChildRef(it->getKey());

		if (!t)
			t1.addChild(*it);
		else
			joinchildren2(*t, *it); 
	}
}

template<typename T>
forest<T> join2(node<T> &t1, node<T> &t2) {

	forest<T> res;
	node<T> *n1;

	if (t1.getKey() != t2.getKey()) {
	
		res.addTree(t1);
		res.addTree(t2);
		return res;
	}

	n1 = res.addTree(t1);

	joinchildren2(*n1, t2);

	return res;
}

void join_test() {

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

	forest<const char*> f;
	//forest<const char*> f = forest<const char*>::join(tree1, tree2);

	//printf("classic join...\n");
	//dump(f);

	f = join2(tree1,tree2);
	printf("new join..\n");

	dump(f);
	
}

void ksf_test() {

	testFuncObj *r = new testFuncObj("r");
	testFuncObj *g = new testFuncObj("g");
	testFuncObj *f = new testFuncObj("f");
	testFuncObj *h = new testFuncObj("h");
	testFuncObj *b = new testFuncObj("b");
	testFuncObj *x = new testFuncObj("x");
	testFuncObj *a = new testFuncObj("a");

	k_value = 2;

	call2(r,1);
	call2(g,2);
	call2(f,3);
	ret();
	call2(h,6);
	call2(f,5);
	call2(b,7);
	ret();
	ret();
	ret();
	ret();
	call2(a,4);
	call2(h,3);
	call2(x,8);

	cout << "dump of ksf\n";

	dump(ctx.kSlabForest);
}

int main(int argc, char ** argv) {

	ctx.shadowstack.push(ShadowStackItemType<keyT>(0,0));

	join_test();

	getchar();
	return 0;
}