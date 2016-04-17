#include "parse.h"
#include <string>

using namespace std;
bool parse(int argc, const char * argv[], string & in_file, string & out_file, int & op)
{
	op = 0;
	in_file = "";
	out_file = "";

	if (argc != 6)
	{
		return false;
	}

	for (int i = 1; i < argc; ++i)
	{
		string str = argv[i];
		if (str == "-c")
		{
			op = 1;
		}
		else if (str == "-u")
		{
			op = 2;
		}
		else if (str == "-f" || str == "--file")
		{
			if (i != argc - 1)
			{
				in_file = argv[i + 1];
			}
			else
			{
				return false;
			}
		}
		else if (str == "-o")
		{
			if (i != argc -1)
			{
				out_file = argv[i + 1];
			}
			else
			{
				return false;
			}
		}
		else
		{
			if (i == 1)
			{
				return false;
			}
			string buf = argv[i - 1];
			if (buf == "-f" || buf == "--file" || buf == "-o")
			{
				continue;
			}
			else
			{
				return false;
			}
		}
	}
	if ( (op == 1 || op == 2)
		&& (in_file.length() != 0 && out_file.length() != 0) )
		{
			return true;
		}
		else
		{
			return false;
		}
}