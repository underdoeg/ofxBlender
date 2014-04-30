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
		name=n;
	}
	std::string name;
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

	//function that retreives the pointer type
	std::function<unsigned long()> readPointer;
	unsigned int pointerSize;
	string version;
	std::vector<Block> blocks;
	DNACatalog catalog;
	std::ifstream file;
};

}
}

#endif // FILE_H
