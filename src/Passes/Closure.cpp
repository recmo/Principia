#include "Closure.h"
#include "DFG/Node.h"

Closure::Closure(const Node* closure)
: _closure(closure)
, _context()
{
}

Closure::Closure(const Node* closure, const vector< Value >& context)
: _closure(closure)
, _context(context)
{
}
