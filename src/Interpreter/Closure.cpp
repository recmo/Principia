#include "Closure.h"
#include "DFG/Node.h"

Closure::Closure(const Node* closure)
: _closure(closure)
, _context()
{
}
