#pragma once
#include <DFG/PropertyMap.h>
#include <Unicode/string.h>

class SourceProperty: public PropertyMap::Property
{
public:
	SourceProperty(): _filename(), _fromLine(0), _toLine(0), _fromColumn(0), _toColumn(0) { }
	SourceProperty(const SourceProperty& copy);
	SourceProperty(const string& file, int fromLine, int fromColumn, int toLine , int toColumn);
	virtual ~SourceProperty();
	virtual void print(std::wostream& out) const override;
	
	void printLocation(std::wostream& out) const;
	void printContent(std::wostream& out) const;
	void printCaret(std::wostream& out) const;
	
	bool isEmpty() const { return _filename.empty(); }
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
