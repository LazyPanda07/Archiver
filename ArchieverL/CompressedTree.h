#pragma once

#include <map>
#include <vector>
#include <array>

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

	}* root;
	std::map<char, std::vector<char>> encodeCodes;
	std::map<std::vector<char>, char> decodeCodes;
	std::vector<node*> allSymbols;

private:
	static std::array<char, 8> getBits(char c);

	void print(node* value);

	void buildCodes();

public:
	CompressedTree();

	std::pair<std::vector<char>, std::vector<size_t>> encode(const std::vector<char>& fileData, const std::vector<size_t>& sizes);

	std::vector<char> decode(const std::vector<char>& fileData, short importantBitsCount);

	void setDecodeCodes(const std::map<std::vector<char>, char>& decodeCodes);

	std::map<char, std::vector<char>>& getAllCodes();

	void print();

	~CompressedTree();
};