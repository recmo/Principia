#include "Closure.h"
#include "DFG/Node.h"

Closure::Closure(std::shared_ptr<Node> closure)
: _closure(closure)
, _context()
{
}

Closure::Closure(std::shared_ptr<Node> closure, const vector<Value>& context)
: _closure(closure)
, _context(context)
{
}
