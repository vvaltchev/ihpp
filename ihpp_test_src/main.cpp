
#include <stdio.h>
#include <iostream>

inline void BM_inc_nodes_created() { }
inline void BM_inc_empty_nodes_created() { }
inline void BM_inc_nodes_copied() { }
inline void BM_inc_forests_copied() { }

#include "test_data_structs.h"



template <typename T>
void traceObject_generic(TracingObject<T> *to, testCtx<T> *ctx, node<T>* &top, node<T>* &bottom, bool acc, unsigned int k) 
{

	node<T> *n;
	map<T, node<T>* >::iterator it;
	T key = to->getKey();
	
	if (!top && !bottom)	
		ctx->rootKey=key;

	if (acc) {
		if (top && to == top->getValue()) {
	
			top->incCounter();
			return;
		}
	}

	if (!( (ctx->counter) % k )) {

		bottom=top;
		it = ctx->R.find(key);

		if (it == ctx->R.end())
			ctx->R[key] = ( top = ctx->kSlabForest.addTreeByVal(node<T>(key, to, 0))  );
		else
			top = it->second;
	
	} else {
		
		n = top->getChildRef(key);

		if (!n)
			n = top->addChildByVal(node<T>(key, to, 0));
		
		top=n;
	}
	
	top->incCounter();

	if (bottom) {
	
		n = bottom->getChildRef(key);

		if (!n)
			n = bottom->addChildByVal(node<T>(key, to, 0));
		
		bottom=n;
		bottom->incCounter();
	}

	ctx->counter++;
}


typedef node<const char*> nn;

#define N(f,c) (nn(#f,new simpleVal(#f), c))
#define add(n1,n2) ((n1)->addChildByVal(n2))
#define t(x) traceObject_generic((x), &ctx, ctx.top, ctx.bottom, false, 1000000)
#define tk(x) traceObject_generic((x), &ctx, ctx.top, ctx.bottom, false, k)

void join_op() {

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

}

int main(int argc, char ** argv) {

	testCtx<const char*> ctx;

	testFuncObj *a = new testFuncObj("a");
	testFuncObj *b = new testFuncObj("b");
	testFuncObj *c = new testFuncObj("c");

	t(a); t(b); t(c); t(a);

	cout << "dump of ksf\n";

	dump(ctx.kSlabForest);

	getchar();
	return 0;
}