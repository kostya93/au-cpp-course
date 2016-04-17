#include <cstddef>

#include "node.h"

node::node()
: freq(0)
, l(NULL)
, r(NULL)
, sym(0)
{}

node::~node()
{
	delete l;
	delete r;
}

bool comp::operator()(const node * ln, const node * rn)
{
	return ln->freq > rn->freq;
}