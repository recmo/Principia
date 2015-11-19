#pragma once
#include "fixups.h"


///TODO: Class to write IR to a file
/*

File:
	[# ClosureNodes]
	[# CallNodes]
	[# Symbols]
	foreach ClosureNode
		[# arguments]
		foreach argument
			[# symbol number]
		[# returns]
		foreach return
			[# symbol number]
	foreach CallNode
		[# arguments]
		foreach argument
			[# symbol number]
		[# returns]
		foreach return
			[# symbol number]
	foreach Symbol
		[S label] (optional?)

TODO:
	- Calculate minimal scopes of variables
	- Use De Buijn indices on those variables
	- Arithmetic coding
*/


class StoreIR
{
};

