
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

	if (!( (ctx->shadowstack.size()-1) % k )) {

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
