#include <iostream>
#include <fstream>
#include <map>
#include <queue> 
#include <cstdint>
#include <vector>
#include <string>
#include <cstddef>

#include "compress.h"
#include "node.h"
#include "int2byte.h"
#include "result.h"

using namespace std;

int const NUM_OF_SYM = 256;


/*
 функция записывает дерево в массив
 если у вершины есть потомки, то записываем 1 и рекурсивно
 вызываем для одного потомка, потом для другого. Если потомков нет,
 то записываем 0 и далее символ (8 бит)
 
     /\
    а /\       --->      10[а]10[в]0[с]
      в с
*/
void build_arr_from_tree(node * root, vector <short> & tree)
{
	if (root->l != NULL && root->r != NULL)
	{
		tree.push_back(1);
		build_arr_from_tree(root->l, tree);
		build_arr_from_tree(root->r, tree);
	}
	else
	{
		tree.push_back(0);
		bool buf = 0;
		for (int i = 0; i < 8; ++i)
		{
			buf = root->sym & (1 << (7 - i));
			if (buf)
			{
				tree.push_back(1);
			}
			else
			{
				tree.push_back(0);
			}
		}
	}
}


// функцйия построения таблицы (символ -> код) по заданному дереву
// поддерживаем code, если спускаемся в одну сторону - дописываем 1, если
// в другую - дописываем 0. Когда дошли до листа - записываем текущий код
// данному листу. При возвращении из обхода поддерева - удаляем один символ
// из текущего кода.
void build_table (node * root, vector <short> & code, vector <vector<short> > & table)
{
	if (root->l != NULL && root->r != NULL)
	{
		code.push_back(0);
		build_table(root->l, code, table);
		code.push_back(1);
		build_table(root->r, code, table);
	}
	else
	{
		unsigned char c = root->sym;
		table[c] = code;
	}
	if (code.size() > 0)
	{
		code.pop_back();
	}
}


// формируем очередь символов с приоритетами (по частоте появления в файле)
// и на основании ее строим дерево.
// Извлекаем 2 вершины, создаем 3-ю, подвешиваем к ней первые 2
// в качестве частоты для новой вершины записываем сумма двух ее потомков.
// Помещаем ее в очередь. 
priority_queue <node*, vector<node*> , comp> build_tree (vector <int> freq_of_symbols)
{
	priority_queue <node*, vector<node*> , comp> my_queue;  
	unsigned char ch = 0;
	for (size_t i = 0; i < freq_of_symbols.size(); ++i)
	{
		if (freq_of_symbols[ch] == 0)
		{
			ch++;
			continue;
		}
		node* temp = new node;
		temp->freq = freq_of_symbols[ch];
		temp->sym = ch;
		my_queue.push(temp);
		ch++;
	}
	while (my_queue.size()>1)
	{
		node* temp = new node;
		node* l = my_queue.top();
		my_queue.pop();
		node* r = my_queue.top();
		my_queue.pop();
		temp->l = l;
		temp->r = r;
		temp->freq = l->freq + r->freq;
		temp->sym = 0;
		my_queue.push(temp);
	}
	return my_queue;
}

// вычисляем частоты появления символов.
vector <int> freq_of_syms(char * buffer, uint32_t len)
{
	vector <int> freq_of_symbols(NUM_OF_SYM, 0);
	for (size_t i = 0; i < len; ++i)
	{
		unsigned char c = buffer[i];
		freq_of_symbols[c]++;
	}
	return freq_of_symbols;
}

vector <char> build_arr_from_table (char * buffer, uint32_t len, const vector <vector<short> > & table)
{
	vector <char> rez;
	for (size_t i = 0; i < len; ++i)
	{
		unsigned char c = buffer[i];
		for (size_t j = 0; j < table[c].size(); ++j)
		{
			rez.push_back(table[c][j]);
		}
	}
	return rez;
}

void create_output(char * out, int out_size, uint32_t len, const vector<short> & tree, const vector<char> & rez)
{
	// Записываем кол-во символов исходного файла
	// в первые 4 байта сжатого файла.
	int2byte.i32 = len;
	out[0] = int2byte.i8[0];
	out[1] = int2byte.i8[1];
	out[2] = int2byte.i8[2];
	out[3] = int2byte.i8[3];

	// Записываем размер дерева(в битах)
	// в байты 4-7 сжатого файла
	int2byte.i32 = tree.size();
	out[4] = int2byte.i8[0];
	out[5] = int2byte.i8[1];
	out[6] = int2byte.i8[2];
	out[7] = int2byte.i8[3];

	int count = 0;
	char c = 0;
	int j = 8;

	// Пакуем дерево и результат сжатия в байты
	// и записываев в out
	for (size_t i = 0; i < tree.size(); ++i)
	{
		c = c | tree[i] << (7 - count);
		count++;
		if (count == 8)
		{
			out[j] = c;
			j++;
			count = 0;
			c = 0;
		}
	}

	for (size_t i = 0; i < rez.size(); ++i)
	{
		c = c | rez[i] << (7 - count);
		count++;
		if (count == 8)
		{
			out[j] = c;
			j++;
			count = 0;
			c = 0;
		}
	}

	if (j < out_size)
	{
		out[j] = c;
	}
}

int calculate_out_size(int size_of_rez, int size_of_tree)
{
	int out_size = 0;
	if ( (size_of_rez + size_of_tree) % 8 == 0)
	{
		out_size = (size_of_rez + size_of_tree) / 8;
	}
	else
	{
		out_size = (size_of_rez + size_of_tree) / 8 + 1;
	}

	// 4 байта на кол-во символов в исходном файле
	// 4 байта на размер дерева (в битах)
	out_size += 8;

	return out_size;
}

result compress(string in_file, string out_file)
{
	ifstream input (in_file, ifstream::binary);
	
	result result_of_comp;
	if (!input.is_open())
	{
		cout << "no such file" << endl;
		result_of_comp = {0, 0, 0};
		return result_of_comp;
	}

	//вычисляем длину файла
	input.seekg(0, input.end);
	uint32_t len = input.tellg();
	input.seekg(0, input.beg);


	if (len == 0)
	{
		ofstream output(out_file, ifstream::binary);
		result_of_comp = {0, 0, 0};
		return result_of_comp;
	}

	char * buffer = new char [len];
	input.read(buffer, len);

	vector<int> freq_of_symbols = freq_of_syms(buffer, len);

	priority_queue <node*, vector<node*> , comp> my_queue = build_tree(freq_of_symbols);

	// создаем таблицу символ -> код
	// и записываем дерево в массив
	//map <char, vector<short> > table;

	vector <vector<short> > table(NUM_OF_SYM);
	vector<short> code; //(1);
	build_table(my_queue.top(), code, table);
	
	vector<short> tree;
	build_arr_from_tree(my_queue.top(), tree);

	vector <char> rez = build_arr_from_table(buffer, len, table);

	ofstream output(out_file, ifstream::binary);

	// Заархивированный файл будет выглядеть следующим образом
	// [кол-во символов в исходном файле (4 байта)][кол-во бит в дереве (4 байта)][дерево][закодированный файл]
	uint32_t out_size = calculate_out_size(rez.size(), tree.size());

	char * out = new char[out_size];

	create_output(out, out_size, len, tree, rez);

	// записываем out в файл.
	output.write(out, out_size);
	/*
	cout << len << endl;					// размер исходного файла
	cout << out_size << endl;				// размер полученного файла
	cout << 8 + tree.size() / 8 << endl;	// размер доп. информации, хранившейся в сжатом
											// файле (4 байта - кол-во символов в исходном файле)
											//       (4 байта - размер дерева в битах)
							    			//       (size_of_tree/8 байт - дерево)
	*/
	delete[] out;
	delete my_queue.top();
	delete[] buffer;

	result_of_comp = {len, out_size, 8 + (uint32_t)tree.size() / 8};

	return result_of_comp;
}