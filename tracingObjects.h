
#ifndef __TRACING_OBJECTS__
#define __TRACING_OBJECTS__

#include <sstream>
#include <string.h>

#include "dataStructures.h"
#include "util.h"

template <typename keyT>
class TracingObject : public ObjectWithKey<keyT> {

protected:

	obj_counter_t simpleCounter;
	keyT key;

public:

	TracingObject(keyT k) : simpleCounter(0), key(k) {}
	virtual keyT getKey() { return key; }
	virtual obj_counter_t getSimpleCounter() { return simpleCounter; }
	virtual void incSimpleCounter() { simpleCounter++; }

	virtual operator string() const = 0;
};


//In this simple class (more struct-like) char pointers are used instead of std::string objects
//because in 32 bit archs they have sizeof = 28 bytes, versus the 4 bytes of the char*.
class insInfo {

public:

	const char *ins_text;
	ADDRINT targetAddr;
	ADDRINT targetFuncAddr;
	const char *externFuncName;
	bool isCall;

	bool isDirectBranchOrCall() { return targetAddr != 0; }

	insInfo() : targetAddr(0), targetFuncAddr(0), externFuncName(0) { }
	insInfo(const char *ins, ADDRINT tAddr, ADDRINT tfuncAddr, const char *extFuncName=0) : 
		ins_text(ins), targetAddr(tAddr), targetFuncAddr(tfuncAddr), externFuncName(extFuncName)
	{ 
		isCall = !strncmp(ins_text,"call",4);
	}

};

class FunctionObj : public TracingObject<ADDRINT> {

	string _functionName;
	string _fileName;

public:

	map<ADDRINT, insInfo> instructions;

	FunctionObj(ADDRINT ptr, string funcName, string fileName) : 
		TracingObject<ADDRINT>(ptr), _functionName(funcName), _fileName(fileName) { }

	ADDRINT functionAddress() const { return key; }
	string functionName() const { return _functionName; }
	string fileName() const { return _fileName; }

	operator string() const { return functionName()+"()"; }
};

class BasicBlock : public TracingObject<ADDRINT> {

	ADDRINT lastInsAddr;
	INT32 _firstLine;
	INT32 _firstCh;

public:

	FunctionObj *functionPtr;


	BasicBlock(ADDRINT ptr, FunctionObj *funcPtr, ADDRINT lastInsAddress, INT32 line, INT32 col) 
		: TracingObject<ADDRINT>(ptr), lastInsAddr(lastInsAddress), 
		_firstLine(line), _firstCh(col), functionPtr(funcPtr)
	{
		assert(functionPtr); 
	}

	ADDRINT blockAddress() const { return key; }
	ADDRINT blockEndAddress() const { return lastInsAddr; }
	ADDRINT functionAddr() const { return functionPtr->functionAddress(); }
	string functionName() const { return functionPtr->functionName(); }
	INT32 firstLine() const { return _firstLine; }
	INT32 firstCh() const { return _firstCh; }

	bool isFirstBlock() const { return blockAddress() == functionPtr->functionAddress(); }

	operator string() const { 
		
		stringstream ss;

		ss << "{";
		ss << functionName();
		ss << "+";
		ss << (blockAddress()-functionAddr());
		ss << " at ";
		ss << _firstLine;
		ss << ",";
		ss << _firstCh;
		ss << "}";

		return ss.str();
	}
};




inline ostream& operator << (ostream& s, BasicBlock& bb) { 

	return s << (string)bb;
}

inline ostream& operator << (ostream& s, FunctionObj& fc) { 
	
	return s << (string)fc;
}



typedef node<ADDRINT> ihppNode;
typedef forest<ADDRINT> ihppForest;
typedef map<ADDRINT, ihppNode*> ihppNodeMap;
typedef map<ADDRINT, FunctionObj*> FuncsMap;
typedef map<ADDRINT, BasicBlock*> BlocksMap;
typedef FuncsMap::iterator FuncsMapIt;
typedef BlocksMap::iterator BlocksMapIt;

inline ostream& operator << (ostream& s, ihppNode &n) { 
	
	TracingObject<ADDRINT> *obj = static_cast<TracingObject<ADDRINT> *>(n.getValue()); 

	assert(obj);

	return s << (string)*obj;
}

inline ostream& operator << (ostream& s, ihppNode *n) { 

	if (n) {
		
		TracingObject<ADDRINT> *obj = static_cast<TracingObject<ADDRINT> *>(n->getValue());
		assert(obj);

		return s << (string)*obj;
	}

	return s << "(null)";
}



#endif