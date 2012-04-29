#pragma once
#include "fixups.h"
#include "DFG/PropertyType.h"
#include "DFG/Property.h"

class SourceProperty: public Property
{
public:
	SourceProperty(const SourceProperty& copy);
	SourceProperty(const string& file, int fromLine, int fromColumn, int toLine , int toColumn);
	virtual ~SourceProperty();
	
	static PropertyType classType;
	virtual PropertyType type() const { return classType; }
	virtual SourceProperty* clone() const { return new SourceProperty(*this); }
	
	virtual void print(std::wostream& out) const;
	void printCaret(std::wostream& out) const;
	
	int fromLine() const { return _fromLine; }
	int toLine() const { return _toLine; }
	int fromColumn() const { return _fromColumn; }
	int toColumn() const { return _toColumn; }
	
protected:
	string _filename;
	int _fromLine;
	int _toLine;
	int _fromColumn;
	int _toColumn;
}; 
