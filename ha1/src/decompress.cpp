#include <iostream>
#include <fstream>
#include <cstdint>
#include <string>
#include <cstddef>

#include "decompress.h"
#include "node.h"
#include "int2byte.h"
#include "result.h"


using namespace std;

char decoding_sym(const char * buffer, node * root, int & indx)
{
	if (root->l == NULL && root->r == NULL)
	{
		return root->sym;
	}

	bool buf = 0;
	int byte = indx / 8;
	int bit = indx % 8;


	buf = buffer[byte] & (1 << (7 - bit));


	if (buf)
	{
		return decoding_sym(buffer, root->r, ++indx);
	}
	else
	{
		return decoding_sym(buffer, root->l, ++indx);
	}
}


/*
 Функция обратная build_arr_from_tree. Берет дерево в виде массива
 и строит дерево
                          /\
10[а]10[в]0[с]   --->    а /\
                           в с
*/

void build_tree_from_arr(const char * buffer, node * root, int & indx)
{
	bool buf = 0;
	int byte = indx / 8; // в каком байте находится бит с номером indx
	int bit = indx % 8;  // смещение внутри байта

	buf = buffer[byte] & (1 << (7 - bit));
	if (buf)
	{
		node * tmp_l = new node;
		node * tmp_r = new node;

		root->l = tmp_l;
		root->r = tmp_r;

		indx++;
		build_tree_from_arr(buffer, tmp_l, indx);
		build_tree_from_arr(buffer, tmp_r, indx);
	}
	else
	{	
		// далее происходит магия, которая собирает один char
		// из двух частей, разбросанных по разным байтам
		char c1 = 0;
		int j = 7;
		for (int i = bit + 1; i < 8; ++i)
		{
			buf = buffer[byte] & (1 << (7 - i));
			if (buf)
			{
				c1 = c1 | (1 << j); 
			}
			--j;
		}
		char c2 = 0;

		for (int i = 0; i < bit + 1; ++i)
		{
			buf = buffer[byte + 1] & (1 << (7 - i));
			if (buf)
			{
				c2 = c2 | (1 << j);
			}
			--j;
		}

		root->sym = c1 | c2;
		indx += 9;
	}
}



uint32_t byte_to_int(char * buffer, int ind)
{
	for(size_t i = 0; i < 4; ++i)
		int2byte.i8[i] = buffer[ind + i];

	return int2byte.i32;
}

char * decoding_file(uint32_t size_of_rez, char * buffer, node * root, int indx)
{
	char * rez = new char [size_of_rez];
	for (size_t i = 0; i < size_of_rez; ++i)
	{
		rez[i] = decoding_sym(buffer, root, indx);
	}
	return rez;
}

result decompress(string in_file, string out_file)
{	
	ifstream input (in_file, ifstream::binary);

	result result_of_decomp = {0, 0, 0};

	if (!input.is_open())
	{
		cout << "no such file" << endl;
		return result_of_decomp;
	}

	input.seekg(0, input.end);
	uint32_t len = input.tellg();
	input.seekg(0, input.beg);

	
	
	if (len == 0)
	{
		ofstream output(out_file, ifstream::binary);
		return result_of_decomp;
	}

	char * buffer = new char [len];
	input.read(buffer, len);

	// восстанавливаем кол-во символов в исходном файле
	// и кол-во бит в дереве
	uint32_t size_of_rez = byte_to_int(buffer, 0);
	uint32_t size_of_tree = byte_to_int(buffer, 4);

	if (size_of_rez == 0)
		return result_of_decomp;
	
	node * root = new node;
	root->l = NULL;
	root->r = NULL;
	int indx = 64; // первый бит дерева в файле

	build_tree_from_arr(buffer, root, indx);
	
	char * rez = decoding_file(size_of_rez, buffer, root, indx);

	ofstream output(out_file, ifstream::binary);
	output.write(rez, size_of_rez);


	/*
	cout << len << endl;					// размер исходного файла
	cout << size_of_rez << endl;			// размер полученного файла
	cout << 8 + size_of_tree / 8 << endl;	// размер доп. информации, хранившейся в сжатом
											// файле (4 байта - кол-во символов в исходном файле)
											//       (4 байта - размер дерева в битах)
							    			//       (size_of_tree/8 байт - дерево)
	*/
	
	delete root;
	delete [] buffer;
	delete [] rez;


	result_of_decomp = {len, size_of_rez, 8 + size_of_tree / 8};

	return result_of_decomp;

}