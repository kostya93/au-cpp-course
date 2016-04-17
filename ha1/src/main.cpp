#include <iostream>
#include <cstring>
#include <string>
#include "decompress.h"
#include "compress.h"
#include "parse.h"
#include "result.h"


int main(int argc, char const *argv[])
{
	using namespace std;

	string in_file;
	string out_file;
	int op = 0;
	result res;
	if (parse(argc, argv, in_file, out_file, op));
	{
		switch (op)
		{
		case 1:
			res = compress(in_file, out_file);
			break;
		case 2:
			res = decompress(in_file, out_file);
			break;
		default:
			cout << "error parse" << endl;
			break;
		}
	}

	cout << res.size_of_source << endl;
	cout << res.size_of_rez << endl;
	cout << res.size_of_add_info << endl;

	return 0;
}