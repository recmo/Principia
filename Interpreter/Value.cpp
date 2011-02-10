#include "Value.h"
#include "Closure.h"
#include "IR/ClosureNode.h"
#include "IR/SymbolVertex.h"
#include "math.h"

std::wostream& operator<<(std::wostream& out, const Value& value)
{
	switch(value.kind)
	{
		case Value::None:
			out << "none";
			break;
		case Value::Function:
			out << value.function()->closure()->function()->identifier();
			break;
		case Value::Integer:
			out << value.integer();
			break;
		case Value::Real:
			out.precision(5);
			out << std::fixed << value.real();
			break;
	}
	return out;
}
