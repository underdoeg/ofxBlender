#ifndef FILE_H
#define FILE_H

#include "Utils.h"
#include <functional>
#include "Scene.h"

namespace ofx {
namespace blender {


//DNA TYPE STUFF
class DNAName {
public:
	DNAName(string n) {
		isPointer = false;
		isArray = false;
		arrayDimensions = 0;

		name=n;


		//now analyze the name
		unsigned int len = name.size();
		if(name[0] == '*')
			isPointer = true;
		if(len > 2 && name[1] == '*'){
			isArray = true;
			arrayDimensions = 1;
		}

		unsigned int numSquareBracks = ofStringTimesInString(name, "[");
		if(numSquareBracks != 0){
			isArray = true;
			if(numSquareBracks == 1){
				arrayDimensions = 1;
			}else if(numSquareBracks == 2){
				arrayDimensions = 2;
			}
		}

		//strip the name
		nameStriped = name;
		ofStringReplace(nameStriped, "*", "");
		nameStriped = ofSplitString(nameStriped, "[")[0];
	}

	std::string name;
	std::string nameStriped;
	bool isPointer;
	bool isArray;
	unsigned int arrayDimensions;
};

class DNAType {
public:
	DNAType(string n) {
		name=n;
	};
	string name;
	unsigned short size;
};

class DNAField {
public:
	DNAField(DNAType* t, DNAName* n, unsigned int off) {
		name=n;
		type=t;
		offset=off;
	};
	DNAName* name;
	DNAType* type;
	unsigned int offset;
};

class DNAStructure {
public:
	DNAStructure(DNAType* t) {
		type = t;
	};
	DNAType* type;
	std::vector<DNAField> fields;
};

class DNACatalog {
public:
	vector<DNAName> names;
	vector<DNAType> types;
	vector<DNAStructure> structures;

	bool hasStructure(std::string name) {
		for(vector<DNAStructure>::iterator it = structures.begin(); it<structures.end(); it++) {
			if((*it).type->name == name)
				return true;
		}
		return false;
	}
};

///

class File {
public:
	class Block {
	public:
		string code;
		unsigned int size;
		unsigned long oldAddress;
		unsigned int SDNAIndex;
		unsigned int count;
		streampos fileOffset;
		DNAStructure* structure;
	};

	File();
	~File();

	bool load(string path);
	void exportStructure(string path="structure.html");
	Scene* getScene(unsigned int index);

private:
	//templated read function
	template<typename Type>
	Type read(){
		Type ret;
		file.read((char*)&ret, sizeof(Type));
		return ret;
	}
	char* readChar(streamsize length);
	std::string readString(streamsize length);
	void readHeader(File::Block& block);
	void seek(streamoff to);
	void parseBlock(Block* block);

	//function that retreives the pointer type
	std::function<unsigned long()> readPointer;
	unsigned int pointerSize;
	string version;
	std::vector<Block> blocks;
	DNACatalog catalog;
	std::ifstream file;
	std::map<std::string, std::function<void(Block* block)> > parsers;
};

}
}

#endif // FILE_H
