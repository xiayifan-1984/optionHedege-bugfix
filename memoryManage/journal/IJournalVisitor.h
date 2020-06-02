#ifndef MEMORYMANAGE_IJOURNALVISITOR_H
#define MEMORYMANAGE_IJOURNALVISITOR_H

#include "../utils/memoryManage_declare.h"

MM_NAMESPACE_START

class Frame;
/**
 * visitor mode for reader,
 * any class with this interface can be added into JournalReader
 */
class IJournalVisitor
{
public:
	virtual void visit(const string& name, Frame& frame) = 0;
};

MM_NAMESPACE_END
#endif
