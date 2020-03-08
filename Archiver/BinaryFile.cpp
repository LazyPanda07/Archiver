#include "BinaryFile.h"

#include <map>
#include <vector>
#include <fstream>
#include <iomanip>
#include <filesystem>

#include "CompressedTree.h"

using namespace std;

namespace BinaryFile
{
	BinaryFileStructure::BinaryFileStructure(short importantBitsCount, unsigned __int64 sizeInBytes) : importantBitsCount(importantBitsCount), sizeInBytes(sizeInBytes)
	{
		memset(fileName, '\0', sizeof(fileName));
		memset(filePath, '\0', sizeof(filePath));
	}

	BinaryFileStructure::BinaryFileStructure(short importantBitsCount, const std::wstring& fileName, unsigned __int64 sizeInBytes, const std::wstring* const filePath) : BinaryFileStructure(importantBitsCount, sizeInBytes)
	{
		if (fileName.size() > 15)
		{
			return;
		}

		if (filePath && filePath->size() > 10)
		{
			return;
		}

		wmemcpy(this->fileName, fileName.data(), fileName.size());

		if (filePath)
		{
			wmemcpy(this->filePath, filePath->data(), filePath->size());
		}

		this->fileName[15] = L'\0';
		this->filePath[10] = L'\0';
	}

	void encodeBinaryFile(const wstring& binaryFileNameIn, const wstring& binaryFileNameOut)
	{
		map<char, size_t> entries;
		CompressedTree tree;
		vector<char> fileData;
		vector<char> data;
		ifstream in;
		ofstream dictionary;
		ofstream out;
		wstring dictionaryName(begin(binaryFileNameOut), begin(binaryFileNameOut) + binaryFileNameOut.rfind('.'));
		dictionaryName += L".di";

		in.open(binaryFileNameIn, ios::binary);
		while (!in.eof())
		{
			char c;
			in >> noskipws >> c;

			data.push_back(c);
		}
		data.pop_back();
		fileData.insert(end(fileData), begin(data), end(data));

		for (auto&& i : data)
		{
			entries[i]++;
		}
		in.close();
		data.~vector();

		vector<char> code = tree.encode(fileData);
		map<char, vector<char>> codes = tree.getAllCodes();

		dictionary.open(dictionaryName);
		for (auto i : codes)
		{
			dictionary << static_cast<int>(i.first) << " : ";
			for (auto j : i.second)
			{
				dictionary << static_cast<int>(j);
			}
			dictionary << endl;
		}
		dictionary.close();

		BinaryFileStructure file(code.size() % CHAR_BIT, binaryFileNameIn, code.size() / CHAR_BIT);

		out.open(binaryFileNameOut, ios::binary);
		out.write(reinterpret_cast<const char*>(&file), sizeof(BinaryFileStructure));
		vector<char> temp;
		for (size_t i = 0; i < code.size(); i++)
		{
			temp.push_back(code[i]);

			if (temp.size() == CHAR_BIT)
			{
				char oneByte = 0;
				for (int j = temp.size() - 1, k = 0; j >= 0; j--, k++)
				{
					oneByte += temp[j] << k;
				}

				out << oneByte;
				temp.clear();
			}
			else if (i + 1 == code.size())
			{
				char oneByte = 0;
				int mask = 128;

				for (int j = temp.size() - 1; j >= 0; j--, mask >>= 1)
				{
					oneByte += ((temp[j] ? 255 : 0) & mask);
				}

				out << oneByte;
				temp.clear();
			}

		}
		out.close();
	}

	void decodeBinaryFile(const wstring& binaryFileName)
	{
		map<vector<char>, char> decodeCodes;
		CompressedTree tree;
		vector<char> code;
		ifstream in;
		ifstream dictionary;
		wstring dictionaryName(begin(binaryFileName), begin(binaryFileName) + binaryFileName.rfind('.'));
		BinaryFileStructure file;
		dictionaryName += L".di";

		in.open(binaryFileName, ios::binary);
		in.read(reinterpret_cast<char*>(&file), sizeof(BinaryFileStructure));
		while (!in.eof())
		{
			char c = 0;
			in >> noskipws >> c;
			code.push_back(c);
		}
		code.pop_back();
		in.close();

		dictionary.open(dictionaryName);
		while (true)
		{
			string tem;
			getline(dictionary, tem);

			if (tem == "")
			{
				break;
			}

			vector<char> temCode;

			string charCode;

			for (size_t i = 0; i < tem.size(); i++)
			{
				if (tem[i] == ' ')
				{
					break;
				}
				charCode += tem[i];
			}

			for (size_t i = tem.rfind(' ') + 1; i < tem.size(); i++)
			{
				temCode.push_back(tem[i]);
			}

			decodeCodes[temCode] = static_cast<char>(atoi(charCode.data()));
		}
		dictionary.close();

		tree.setDecodeCodes(decodeCodes);

		vector<char> res = tree.decode(code, file.importantBitsCount);

		ofstream resultFile(file.fileName, ios::binary);
		resultFile.write(res.data(), res.size());
		resultFile.close();
	}
}