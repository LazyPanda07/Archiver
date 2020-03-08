#pragma once

#include <map>
#include <vector>

class CompressedTree
{
private:
	struct node
	{
		char symbol;
		bool isParent;
		bool bit;
		node* left;
		node* right;
		node* parent;

		node(char symbol);

		node(node* left, node* right);

	}*root;
	std::map<char, std::vector<char>> encodeCodes;
	std::map<std::vector<char>, char> decodeCodes;
	std::vector<node*> allSymbols;

private:
	static std::vector<char> getBits(char c);

	void print(node* value);

	void buildCodes();

public:
	CompressedTree();

	std::vector<char> encode(const std::vector<char>& fileData);

	std::vector<char> decode(const std::vector<char>& fileData, short importantBitsCount);

	void setDecodeCodes(const std::map<std::vector<char>, char>& decodeCodes);

	std::map<char, std::vector<char>>& getAllCodes();

	void print();

	~CompressedTree();
};