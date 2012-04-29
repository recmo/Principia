#include "Parser/SourceProperty.h"
#include <fstream>

PropertyType SourceProperty::classType = PropertyType::Source;

SourceProperty::SourceProperty(const SourceProperty& copy)
: Property(copy)
, _filename(copy._filename)
, _fromLine(copy._fromLine)
, _toLine(copy._toLine)
, _fromColumn(copy._fromColumn)
, _toColumn(copy._toColumn)
{
}

SourceProperty::SourceProperty(const string& file, int fromLine, int fromColumn, int toLine, int toColumn)
: Property()
, _filename(file)
, _fromLine(fromLine)
, _toLine(toLine)
, _fromColumn(fromColumn)
, _toColumn(toColumn)
{
}

SourceProperty::~SourceProperty()
{
}

void SourceProperty::print(std::wostream& out) const
{
	out << _filename << L":" << _fromLine << L":" << _fromColumn;
}

void SourceProperty::printCaret(std::wostream& out) const
{
	print(out);
	out << endl;
	std::wifstream sourceFile(encodeLocal(_filename));
	
	// Skip the first lines
	string line;
	for(int l = 1; l < _fromLine; ++l)
		std::getline(sourceFile, line);
	
	for(int l = _fromLine; l <= _toLine; ++l)
	{
		std::getline(sourceFile, line);
		//for(int c = 0; c < line.size(); ++c)
		//	out << L'‘' << line[c] << L'’' <<  (line[c] == L'\t')<< endl;
		out << line << endl;
		int fromColumn = (l == _fromLine) ? _fromColumn : 1;
		int toColumn = (l == _toLine) ? _toColumn : line.size();
		for(int c = 0; c < fromColumn - 1; ++c)
			out << ((line[c] == L'\t') ? (L'\t') : (L' '));
		for(int c = fromColumn; c <= toColumn; ++c)
			out << L'‾';
		out << endl;
	}
	
	// this is an illegal ​( statement )
	//                    ‾‾‾‾‾‾‾‾‾‾‾‾‾
}

