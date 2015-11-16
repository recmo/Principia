#include "PropertyMap.h"

std::wostream& operator<<(std::wostream& out, const PropertyMap& map)
{
	return out << map._map;
}

std::wostream& operator<<(std::wostream& out,
	const PropertyMap::Property& property)
{
	property.print(out);
	return out;
}
