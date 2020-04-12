#include "CompressedTree.h"

#include <iostream>
#include <algorithm>
#include <stack>

#include "BinaryFileStructure.h"

using namespace std;

CompressedTree::node::node(char symbol) : symbol(symbol), isParent(false), left(nullptr), right(nullptr), parent(nullptr), bit(false)
{

}

CompressedTree::node::node(node* left, node* right) : symbol(NULL), isParent(true), left(left), right(right), parent(nullptr), bit(false)
{

}

array<char, 8> CompressedTree::getBits(char c)
{
	int mask = 128;
	array<char, 8> res;

	for (size_t i = 0; i < CHAR_BIT; i++, mask >>= 1)
	{
		res[i] = c & mask ? '1' : '0';
	}

	return res;
}

void CompressedTree::print(node* value)
{
	if (value)
	{
		print(value->left);
		if (value->symbol == ' ')
		{
			cout << "\' \' ";
		}
		else if (!value->isParent)
		{
			printf("%c ", value->symbol);
		}
		print(value->right);
	}
}

void CompressedTree::buildCodes()
{
	for (const auto& i : allSymbols)
	{
		vector<char> path;
		node* tem = i;

		while (tem != root)
		{
			path.push_back(tem->bit);
			tem = tem->parent;
		}

		reverse(begin(path), end(path));

		encodeCodes[i->symbol] = path;
		decodeCodes[path] = i->symbol;
	}
}

CompressedTree::CompressedTree() : root(nullptr)
{

}

pair<vector<char>, vector<size_t>> CompressedTree::encode(const vector<char>& fileData, const vector<size_t>& sizes)
{
	map<char, size_t> entries;

	for (auto&& i : fileData)
	{
		entries[i]++;
	}

	vector<pair<size_t, node*>> data(entries.size());	//count - symbol
	allSymbols.reserve(entries.size());

	auto it = begin(entries);

	for (size_t i = 0; i < data.size(); i++, it++)
	{
		data[i] = { it->second,new node(it->first) };
	}

	while (data.size() != 1)
	{
		stable_sort(begin(data), end(data));

		auto firstChild = *begin(data);
		auto secondChild = *++begin(data);

		data.push_back({ firstChild.first + secondChild.first,new node(firstChild.second,secondChild.second) });

		auto& check = data.back();

		data.erase(begin(data));
		data.erase(begin(data));

		if (firstChild.second && !firstChild.second->isParent)
		{
			allSymbols.push_back(firstChild.second);
		}

		if (secondChild.second && !secondChild.second->isParent)
		{
			allSymbols.push_back(secondChild.second);
		}

		firstChild.second->bit = 0;
		secondChild.second->bit = 1;

		firstChild.second->parent = secondChild.second->parent = data.back().second;
	}

	root = begin(data)->second;

	this->buildCodes();

	pair<vector<char>, vector<size_t>> res = make_pair<vector<char>, vector<size_t>>({}, { 0 });

	for (size_t i = 0, j = 0; i < fileData.size(); i++)
	{
		if (i == sizes[j])
		{
			res.second.push_back(res.first.size());
			j++;
		}

		res.first.insert(end(res.first), begin(encodeCodes[fileData[i]]), end(encodeCodes[fileData[i]]));
	}

	return res;
}

void print(char c)
{
	int mask = 128;

	for (size_t i = 0; i < CHAR_BIT; i++, mask >>= 1)
	{
		cout << ((c & mask) ? '1' : '0');
	}
	printf("\n");
}

vector<char> CompressedTree::decode(const vector<char>& fileData, short importantBitsCount)
{
	vector<char> res;
	vector<char> data;

	for (auto& i : fileData)
	{
		array<char, 8> tem = this->getBits(i);
		data.insert(end(data), begin(tem), end(tem));
	}

	for (size_t i = 0; importantBitsCount && i < CHAR_BIT - importantBitsCount; i++)
	{
		data.pop_back();
	}

	vector<char> tem;

	size_t temSize = 0;

	for (auto& i : data)
	{
		tem.push_back(i);

		auto value = decodeCodes.find(tem);

		if (value != end(decodeCodes))
		{
			res.push_back(value->second);
			tem.clear();
		}
	}

	return res;
}

void CompressedTree::setDecodeCodes(const map<vector<char>, char>& decodeCodes)
{
	this->decodeCodes = decodeCodes;
}

map<char, vector<char>>& CompressedTree::getAllCodes()
{
	return encodeCodes;
}

void CompressedTree::print()
{
	this->print(root);
}

CompressedTree::~CompressedTree()
{
	stack<node*> s;

	if (root)
	{
		s.push(root);
	}

	while (s.size())
	{
		node* tem = s.top();
		s.pop();

		if (tem->left)
		{
			s.push(tem->left);
		}

		if (tem->right)
		{
			s.push(tem->right);
		}

		delete tem;
	}
}