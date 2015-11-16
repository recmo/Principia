#include "string.h"
#include <Unicode/convert.h>

std::ostream& operator<<(std::ostream& out, const std::wstring& value)
{
	return out << encodeLocal(value);
}
