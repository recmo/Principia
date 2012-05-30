#include "DFG/PropertyMap.h"

PropertyMap::~PropertyMap()
{
	for(auto it = _propertyMap.begin(); it != _propertyMap.end(); ++it)
		delete (*it).second;
}

void PropertyMap::set(const Property& prop)
{
	// Deduce property type
	const PropertyType type = prop.type();
	
	// Delete current value if exists
	auto it = _propertyMap.find(type);
	if(it != _propertyMap.end())
		delete (*it).second;
	
	// Insert the new value
	_propertyMap[type] = prop.clone();
}

bool PropertyMap::has(const PropertyType type) const
{
	return _propertyMap.find(type) != _propertyMap.end();
}

const Property* PropertyMap::get(const PropertyType type) const
{
	auto it = _propertyMap.find(type);
	if(it == _propertyMap.end()) {
		wcerr << "Could not retrieve property " << type.toString() << endl;
		throw L"Could not retrieve property.";
	}
	Property* prop = (*it).second;
	return prop;
}

void PropertyMap::printProperties(std::wostream& out) const
{
	for(auto it = _propertyMap.begin(); it != _propertyMap.end(); ++it)
		out << L"\t" << *((*it).second) << endl;
}

 