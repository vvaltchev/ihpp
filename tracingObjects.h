


namespace kCCFLib {


inline string operator +(string str, INT32 n) {

	char buf[32];
	sprintf(buf, "%d", n);
	return str+string(buf);
}


template <typename keyT>
class TracingObject {

protected:

	unsigned int simpleCounter;
	keyT key;

public:

	TracingObject(keyT k) : simpleCounter(0), key(k) {}
	virtual keyT getKey() { return key; }
	virtual unsigned int getSimpleCounter() { return simpleCounter; }
	virtual void incSimpleCounter() { simpleCounter++; }

	virtual operator string() { return string(); }
};


class FunctionObj : public TracingObject<ADDRINT> {

	string _functionName;

public:

	FunctionObj(ADDRINT ptr, string funcName) : TracingObject<ADDRINT>(ptr), _functionName(funcName) { }

	ADDRINT functionAddress() { return key; }
	string functionName() { return _functionName; }

	operator string() { return functionName()+"()"; }
};

class BasicBlock : public TracingObject<ADDRINT> {

public:

	FunctionObj *functionPtr;
	INT32 firstLine;
	INT32 firstCh;


	BasicBlock(ADDRINT ptr, FunctionObj *funcPtr, INT32 line, INT32 col) 
		: TracingObject<ADDRINT>(ptr), functionPtr(funcPtr), firstLine(line), firstCh(col) 
	{
		assert(functionPtr); 
	}

	ADDRINT blockAddress() { return key; }
	ADDRINT functionAddr() { return functionPtr->functionAddress(); }
	string functionName() { return functionPtr->functionName(); }

	bool isFirstBlock() { return blockAddress() == functionPtr->functionAddress(); }

	operator string() { return "{"+functionName()+","+firstLine+","+firstCh+"}"; }
};




inline ostream& operator << (ostream& s, BasicBlock& bb) { 

	return s << (string)bb;
}

inline ostream& operator << (ostream& s, FunctionObj& fc) { 
	
	return s << (string)fc;
}



typedef node<ADDRINT, TracingObject<ADDRINT>*> kCCFNode;
typedef forest<ADDRINT, TracingObject<ADDRINT>*> kCCFForest;
typedef map<ADDRINT, kCCFNode*> kCCFNodeMap;
typedef map<ADDRINT, FunctionObj*> FuncsMap;
typedef map<ADDRINT, BasicBlock*> BlocksMap;
typedef FuncsMap::iterator FuncsMapIt;
typedef BlocksMap::iterator BlocksMapIt;

inline ostream& operator << (ostream& s, kCCFNode n) { 
	
	TracingObject<ADDRINT> *obj = n.getValue(); 

	assert(obj);

	return s << (string)*obj;
}

inline ostream& operator << (ostream& s, kCCFNode *n) { 

	if (n) {
		
		TracingObject<ADDRINT> *obj = n->getValue(); 
		assert(obj);

		return s << (string)*obj;
	}

	return s << "(null)";
}



}
