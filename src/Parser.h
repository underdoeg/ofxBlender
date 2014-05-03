#ifndef FILEPARSERS_H
#define FILEPARSERS_H

//all of the code is in the header file. this file should only be included once and that is by the file.cpp

#include "Scene.h"
#include "Object.h"
#include "File.h"

enum BLENDER_TYPES{
	MESH = 1
};

namespace ofx {

namespace blender {

//helper class to read structures
class DNAStructureReader {
public:

	DNAStructureReader(File::Block* b) {
		file = b->file;
		block = b;
		reset();
	};

	void reset() {
		setStructure(block->structure, block->offset);
	}

	void setStructure(DNAStructure* s, streamoff offset) {
		structure = s;
		currentOffset = offset;
	}

	//set another structure according to the provided field name
	bool setStructure(string structureName) {
		if(!structure->hasField(structureName)) {
			ofLogWarning(OFX_BLENDER) << "Property \"" << structureName << "\" not found in " << structure->type->name;
			return false;
		}
		DNAField& field = structure->getField(structureName);
		DNAStructure* s = file->catalog.getStructure(field.type->name);
		if(!s)
			return false;
		setStructure(s, currentOffset + std::streampos(field.offset));
		return true;
	}

	DNAField* setField(string fieldName) {
		if(!structure->hasField(fieldName)) {
			ofLogWarning(OFX_BLENDER) << "Property \"" << fieldName << "\" not found in " << structure->type->name;
			return NULL;
		}
		DNAField& field = structure->getField(fieldName);
		file->seek(currentOffset + std::streampos(field.offset));
		return &field;
	}

	template<typename Type>
	std::vector<Type> readArray(string fieldName) {
		std::vector<Type> ret;
		DNAField* field = setField(fieldName);
		if(!field) {
			return ret;
		}
		if(!field->isArray) {
			ofLogWarning(OFX_BLENDER) << "Property \"" << fieldName << "\" is not an array type";
			return ret;
		}

		//if(field->arrayDimensions == 1){
		if(field->arraySizes[0] != 0) {
			for(unsigned int i=0; i < field->arraySizes[0]; i++) {
				ret.push_back(file->read<Type>());
			}
			//Type* ts = file->readMany<Type>(field->arraySizes[0]);

		}
		//}
		return ret;
	}

	template<typename Type>
	Type read(string fieldName) {

		//check for specials functions
		if(std::is_same<Type, std::string>::value) {
			ofLogWarning(OFX_BLENDER) << "DNAStructureReader::read string types should be read with readString";
			return Type();
		}

		bool isPointer = std::is_pointer<Type>::value;

		DNAField* field = setField(fieldName);
		if(!field) {
			/*
			if(isPointer)
				return NULL;
			else
			*/
				return Type();
		}

		if(field->isArray) {
			ofLogWarning(OFX_BLENDER) << "DNAStructureReader::read field is an array and should be read with readArray";
			return Type();
		}

		//read the file contents
		return file->read<Type>();
	}

	string readString(string fieldName) {
		DNAField* field = setField(fieldName);
		if(!field) {
			return "undefined";
		}

		if(field->isArray) {
			string ret = file->readString();
			//strings are usually object names or paths, objects names have the object type prepending, check and remove
			if(ret.size() >= block->code.size() && block->code == ret.substr(0, block->code.size())) {
				ret = ret.substr(block->code.size());
			}
			return ret;
		}
		ofLogWarning(OFX_BLENDER) << "Could not read string, not a char* " << fieldName;
		return "undefined";
	}

	ofVec3f readVec3(string fieldName){
		ofVec3f ret;
		std::vector<float> floats = readArray<float>(fieldName);
		if(floats.size() >= 3){
			ret.set(floats[0], floats[1], floats[2]);
		}
		//ret *= block->file->scale;
		return ret;
	}

	unsigned long readPointer(string fieldName){
		DNAField* field = setField(fieldName);
		if(!field) {
			return 0;
		}
		return file->readPointer();
	}

	File* file;

private:
	DNAStructure* structure;
	File::Block* block;
	streamoff currentOffset;
};

//////////////////////////////////////////////////////////////////////////////////////// PARSER

class Parser {
public:

	//handler wrapers to automate the process of updating / creating new types
	class Handler_ {
	public:
		virtual void* call(DNAStructureReader&) = 0;
		virtual void* call(DNAStructureReader&, void*) = 0;
	};

	template<typename Type>
	class Handler: public Handler_ {
	public:
		typedef std::function<void(DNAStructureReader&, Type*)> funcType;
		Handler(funcType f) {
			function = f;
		}
		void* call(DNAStructureReader& reader) {
			return call(reader, new Type());
		}
		void* call(DNAStructureReader&  reader, void* obj) {
			Type* t = static_cast<Type*>(obj);
			function(reader, t);
			return obj;
		}

		funcType function;
	};


	//now handle all the parsing
	static void init() {
		if(isInit)
			return;

		Parser::handlers[BL_SCENE] = new Handler<Scene>(Parser::parseScene);
		Parser::handlers[BL_OBJECT] = new Handler<Object>(Parser::parseObject);

		isInit = true;
	}

	static void parseScene(DNAStructureReader& reader, Scene* scene) {
		reader.setStructure("id");
		scene->name = reader.readString("name");
	}

	static void parseObject(DNAStructureReader& reader, Object* obj) {
		reader.setStructure("id");
		obj->name = reader.readString("name");

		reader.reset();
		obj->setPosition(reader.readVec3("loc"));
		obj->setScale(reader.readVec3("size"));
		cout << "DATA " << reader.readPointer("data") << endl;
		cout << "TYPE " << reader.read<short>("type") << endl;
	}

	static void* parseFileBlock(File::Block* block) {
		return parseFileBlock(block, NULL);
	}

	static void* parseFileBlock(File::Block* block, void* obj) {
		if(handlers.find(block->structure->type->name) != handlers.end()) {
			DNAStructureReader reader = DNAStructureReader(block);
			if(obj == NULL)
				return handlers[block->structure->type->name]->call(reader);
			else
				return handlers[block->structure->type->name]->call(reader, obj);
		}
		return NULL;
	}

private:
	typedef std::map<std::string, Handler_*> HandlerList;
	static HandlerList handlers;
	static bool isInit;
};

}

}

#endif // FILEPARSERS_H
