#pragma once

struct node
{
	node();
	int 	freq;
	node*	l;
	node*	r;
	char	sym;
	~node();
};

struct comp
{
	bool operator()(const node * ln, const node * rn);
};