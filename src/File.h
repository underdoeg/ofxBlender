#ifndef FILE_H
#define FILE_H


#include "Utils.h"
#include "Scene.h"
#include <functional>
#include "Object.h"

namespace ofx
{
namespace blender
{

//all the names of the blender object types
#define BL_SCENE "Scene"
#define BL_OBJECT "Object"
#define BL_MESH "Mesh"
#define BL_CAMERA "Camera"

/////////////////////////////////////////////////////////////////////////////////////////////////////// DNA
class DNAName
{
public:
	DNAName(string n) {
		name=n;

		//strip the name
		nameClean = name;
		ofStringReplace(nameClean, "*", "");
		nameClean = ofSplitString(nameClean, "[")[0];
	}

	std::string name;
	std::string nameClean;
};

class DNAType
{
public:
	DNAType(string n, unsigned int i) {
		id = i;
		name = n;
	};
	string name;
	unsigned short size;
	unsigned int id;
};

class DNAField
{
public:
	DNAField(DNAType* t, DNAName* n, unsigned int off) {
		name=n;
		type=t;
		offset=off;

		isPointer = false;
		isArray = false;
		arrayDimensions = 0;

		//now analyze the name
		unsigned int len = name->name.size();
		if(name->name[0] == '*')
			isPointer = true;
		if(len > 2 && name->name[1] == '*') {
			isArray = true;
			arrayDimensions = 1;
		}

		unsigned int numSquareBracks = ofStringTimesInString(name->name, "[");
		if(numSquareBracks != 0) {
			isArray = true;
			if(numSquareBracks == 1) {
				arrayDimensions = 1;
			} else if(numSquareBracks == 2) {
				arrayDimensions = 2;
			}

			//now extract array sizes from brackets
			if(arrayDimensions > 0) {
				std::vector<std::string> parts = ofSplitString(name->name, "[");
				arraySizes.push_back(ofToInt(ofSplitString(parts[1], "]")[0]));
				if(arrayDimensions > 1)
					arraySizes.push_back(ofToInt(ofSplitString(parts[2], "]")[0]));
			}
		}

		//check if arraySizes is filled up properly
		if(isArray && arrayDimensions != arraySizes.size()) {
			for(unsigned int i=arraySizes.size(); i<arrayDimensions; i++) {
				arraySizes.push_back(-1);
			}
		}
	};

	DNAName* name;
	DNAType* type;
	unsigned int offset;

	bool isPointer;
	bool isArray;
	unsigned int arrayDimensions;
	std::vector<unsigned int> arraySizes;
};

class DNAStructure
{
public:
	DNAStructure(DNAType* t) {
		type = t;
	}

	bool hasField(string fieldName) {
		for(DNAField& field: fields) {
			if(field.name->nameClean == fieldName)
				return true;
		}
		return false;
	}

	DNAField& getField(string fieldName) {
		for(DNAField& field: fields) {
			if(field.name->nameClean == fieldName)
				return field;
		}
		return fields[0];
	}

	DNAType* type;
	std::vector<DNAField> fields;
};

class DNACatalog
{
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

	DNAStructure* getStructure(std::string name) {
		for(vector<DNAStructure>::iterator it = structures.begin(); it<structures.end(); it++) {
			if((*it).type->name == name)
				return &(*it);
		}
		ofLogWarning(OFX_BLENDER) << "Structure " << name << " not found in DNA Catalogue";
		return NULL;
	}
};

////////////////////////////////////////////////////////////////////////////////////////// FILE
class DNAStructureReader;

class File
{
public:

	File();
	~File();

	bool load(string path);
	void exportStructure(string path="structure.html");
	unsigned int getNumberOfScenes();
	Scene* getScene(unsigned int index);
	unsigned int getNumberOfObjects();
	Object* getObject(unsigned int index);

private:
	class Block
	{
	public:
		Block(File* f) {
			file = f;
		}
		File* file;
		string code;
		unsigned int size;
		unsigned long address;
		unsigned int SDNAIndex;
		unsigned int count;
		streampos offset;
		DNAStructure* structure;
	};

	//templated read function
	template<typename Type>
	Type read() {
		Type ret;
		file.read((char*)&ret, sizeof(Type));
		return ret;
	}

	template<typename Type>
	Type readMany(unsigned int howMany) {
		Type* ret = new Type[howMany];
		for(unsigned int i=0; i<howMany; i++) {
			ret[i] = read<Type>();
		}
		return ret;
	}

	char* readChar(streamsize length);
	std::string readString(streamsize length=0);
	void readHeader(File::Block& block);
	void seek(streamoff to);
	void* parseFileBlock(Block* block);

	unsigned int getNumberOfTypes(string typeName);
	std::vector<Block*> getBlocksByType(string typeName);
	Block* getBlocksByType(string typeName, unsigned int pos);
	Block* getBlockByAddress(unsigned long address);
	Object* getObjectByAddress(unsigned long address);

	//function that retreives the pointer type
	std::function<unsigned long()> readPointer;
	unsigned int pointerSize;
	string version;
	std::vector<Block> blocks;
	std::map<unsigned long, void*> parsedBlocks;
	DNACatalog catalog;
	std::ifstream file;
	float scale;

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////// ALL ABOUT PARSING //////////////////////////////////////////////////////

	friend class DNAStructureReader;
	friend class DNALinkedListReader;
	friend class Parser;

};

}
}

#endif // FILE_H
