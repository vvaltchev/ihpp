#pragma once

#include <sstream>
#include <cstring>

#include "dataStructures.h"
#include "util.h"

enum class TracingObjType {

    Other,
    Func,
    BasicBlock,
};

template <typename keyT>
class TracingObject : public ObjectWithKey<keyT> {

protected:

    TracingObjType type;
    obj_counter_t simpleCounter;
    keyT key;

public:

    TracingObject(TracingObjType t, keyT k)
        : type(t)
        , simpleCounter(0)
        , key(k)
    { }

    virtual ~TracingObject() = default;

    TracingObjType getType() { return type; }

    virtual keyT getKey() { return key; }
    virtual obj_counter_t getSimpleCounter() { return simpleCounter; }
    virtual void incSimpleCounter() { simpleCounter++; }
    virtual operator string() const = 0;
};

class insInfo {

public:

    string ins_text;
    ADDRINT targetAddr;
    ADDRINT targetFuncAddr;
    char *externFuncName;
    bool isCall;

    bool isDirectBranchOrCall() const { return targetAddr != 0; }

    insInfo()
        : targetAddr(0)
        , targetFuncAddr(0)
        , externFuncName(0)
        , isCall(false)
    { }

    insInfo(string ins, ADDRINT tAddr, ADDRINT tfuncAddr, char *extFuncName = nullptr)
        : ins_text(ins)
        , targetAddr(tAddr)
        , targetFuncAddr(tfuncAddr)
        , externFuncName(extFuncName)
        , isCall(ins_text.compare(0, 4, "call") == 0)
    { }
};

class FunctionObj : public TracingObject<ADDRINT> {

    string _functionName;
    string _fileName;

public:

    map<ADDRINT, insInfo> instructions;

    FunctionObj(ADDRINT ptr, string funcName, string fileName)
        : TracingObject<ADDRINT>(TracingObjType::Func, ptr)
        , _functionName(funcName)
        , _fileName(fileName)
    { }

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
        : TracingObject<ADDRINT>(TracingObjType::BasicBlock, ptr)
        , lastInsAddr(lastInsAddress)
        , _firstLine(line)
        , _firstCh(col)
        , functionPtr(funcPtr)
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
