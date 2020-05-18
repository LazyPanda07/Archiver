#include "BinaryFile.h"

#include <map>
#include <unordered_map>
#include <fstream>
#include <iomanip>
#include <filesystem>

#include "CompressedTree.h"
#include "BinaryFileStructure.h"

#ifdef _DEBUG
#include <iostream>
#endif // _DEBUG

using namespace std;

namespace BinaryFile
{
	void encodeBinaryFile(const vector<pair<wstring, vector<wstring>>>& binaryFilesNamesIn, const wstring& binaryFileNameOut)
	{
		CompressedTree tree;
		vector<char> fileData;
		vector<size_t> sizes;
		vector<char> data;
		ifstream in;
		ofstream out;
		short curIndex = 0;
		unordered_map<short, const wstring*> filePath;
		unordered_map<unsigned __int32, wstring> fileName;

		sizes.reserve(binaryFilesNamesIn.size());

		for (auto&& j : binaryFilesNamesIn)
		{
			for (auto&& i : j.second)
			{
				in.open(i, ios::binary);

				while (!in.eof())
				{
					char c;
					in >> noskipws >> c;

					data.push_back(c);
				}

				data.pop_back();
				fileData.insert(end(fileData), begin(data), end(data));
				sizes.push_back(fileData.size());
				data.clear();

				in.close();

				fileName.insert(make_pair(curIndex, i.substr(i.rfind('\\') + 1)));
				filePath.insert(make_pair(curIndex++, &j.first));
			}
		}

		data.~vector();

		pair<vector<char>, vector<size_t>> code = tree.encode(fileData, sizes);	//all files code - size of each file
		map<char, vector<char>> codes = tree.getAllCodes();

		out.open(binaryFileNameOut);
		for (auto i : codes)
		{
			out << static_cast<__int32>(i.first) << " : ";
			for (auto j : i.second)
			{
				out << static_cast<__int32>(j);
			}
			out << endl;
		}

		out << "DICTIONARY END" << endl;

		out.close();

		out.open(binaryFileNameOut, ios::binary | ios::app);

		size_t cur = 0;

		for (size_t i = 0; i < code.second.size(); i++)
		{
			size_t currentFileSize;

			if (i + 1 == code.second.size())
			{
				currentFileSize = code.first.size() - code.second[i];
			}
			else
			{
				currentFileSize = code.second[i + 1] - code.second[i];
			}

			BinaryFileStructure file(currentFileSize % CHAR_BIT, fileName[i], currentFileSize % CHAR_BIT ? currentFileSize / CHAR_BIT + 1 : currentFileSize / CHAR_BIT, filePath[i]);

			out.write(reinterpret_cast<const char*>(&file), sizeof(BinaryFileStructure));

			vector<char> temp;
			for (size_t next = cur + currentFileSize; cur < next; cur++)
			{
				temp.push_back(code.first[cur]);

				if (temp.size() == CHAR_BIT)
				{
					char oneByte = 0;
					for (__int32 j = temp.size() - 1, k = 0; j >= 0; j--, k++)
					{
						oneByte += temp[j] << k;
					}

					out << oneByte;
					temp.clear();
				}
				else if (cur + 1 == next)
				{
					char oneByte = 0;
					__int32 mask = 128;

					for (size_t j = 0; j < temp.size(); j++, mask >>= 1)
					{
						oneByte += ((temp[j] ? 255 : 0) & mask);
					}

					out << oneByte;
					temp.clear();
				}
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
		ofstream out;
		unsigned __int64 offset;

		in.open(binaryFileName);
		while (true)
		{
			string tem;
			getline(in, tem);

			if (tem == "DICTIONARY END")
			{
				offset = in.tellg();
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
		in.close();

		tree.setDecodeCodes(decodeCodes);

		in.open(binaryFileName, ios::binary);
		in.seekg(offset, ios::beg);

		while (!in.eof())
		{
			BinaryFileStructure file;
			in.read(reinterpret_cast<char*>(&file), sizeof(BinaryFileStructure));
			filesystem::path filePath = file.filePath;

			vector<char> fileData(file.sizeInBytes);
			in.read(fileData.data(), fileData.size());

			vector<char> resultFileData = tree.decode(fileData, file.importantBitsCount);

			filesystem::create_directories(filePath);

			filePath.append(file.fileName);

			out.open(filePath, ios::binary);
			out.write(resultFileData.data(), resultFileData.size());
			out.close();
		}

		in.close();
	}
}