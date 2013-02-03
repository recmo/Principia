#pragma once
#include <fixups.h>

class MetaMathTokenStore {
public:
	uint map(const string& symbol);
	string map(uint identifier);
	
protected:
	static map<uint, string> _tokens;
};


