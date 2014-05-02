#ifndef FILEPARSERS_H
#define FILEPARSERS_H

//all of the code is in the header file. this file should only be included once and that is by the file.cpp

#include "Scene.h"
#include "Object.h"
#include "File.h"

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
	void setStructure(string fieldName) {
		if(!structure->hasField(fieldName)) {
			ofLogWarning(OFX_BLENDER) << "Property " << fieldName << " not found in " << structure->type->name;
			return;
		}
		DNAField& field = structure->getField(fieldName);
		DNAStructure* s = file->catalog.getStructure(field.type->name);
		if(!s)
			return;
		setStructure(s, currentOffset + std::streampos(field.offset));
	}

	template<typename Type>
	Type readArray(string fieldName) {
		bool isPointer = std::is_pointer<Type>::value;
		if(!structure->hasField(fieldName)) {
			ofLogWarning(OFX_BLENDER) << "Property " << fieldName << " not found in " << structure->type->name;
			if(isPointer)
				return NULL;
			else
				return Type();
		}
		DNAField& field = structure->getField(fieldName);

		//read the file contents
		file->seek(currentOffset + std::streampos(field.offset));
		return file->read<Type>();
	}

	template<typename Type>
	Type read(string fieldName) {
		bool isPointer = std::is_pointer<Type>::value;
		if(!structure->hasField(fieldName)) {
			ofLogWarning(OFX_BLENDER) << "Property " << fieldName << " not found in " << structure->type->name;
			if(isPointer)
				return NULL;
			else
				return Type();
		}
		DNAField& field = structure->getField(fieldName);

		//seek to the fields position
		file->seek(currentOffset + std::streampos(field.offset));

		if(isPointer) {
			ofLogWarning(OFX_BLENDER) << "Property " << fieldName << " is a pointer - not implemented yet - returning NULL";
			return NULL;
		}

		//check if the type is a string, strings are actually char arrays
		if(std::is_same<Type, std::string>::value) {
			if(field.isArray) {
				//if(field.arraySizes[0] != -1){
				string ret = file->readString();

				//strings are usually object names or paths, objects names have the object type prepending, check and remove
				if(ret.size() >= block->code.size() && block->code == ret.substr(0, block->code.size())) {
					ret = ret.substr(block->code.size());
				}
				return ret;
				//}
			}
			ofLogWarning(OFX_BLENDER) << "Could not read string " << fieldName;
			return "undefined";
		}

		if(field.isArray) {
			cout << "IT IS AN ARRAY!" << endl;
			return Type();
		}

		//read the file contents
		return file->read<Type>();
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
		typedef std::function<void*(DNAStructureReader&, Type*)> funcType;
		Handler(funcType f) {
			function = f;
		}
		void* call(DNAStructureReader& reader) {
			return call(reader, new Type());
		}
		void* call(DNAStructureReader&  reader, void* obj) {
			Type* t = static_cast<Type*>(obj);
			return function(reader, t);
		}

		funcType function;
	};


	//now handle all the parsing
	static void init() {
		if(isInit)
			return;

		Parser::handlers[BL_SCENE] = new Handler<Scene>(Parser::parseScene);
		//Parser::handlers[BL_OBJECT] = new Handler<Object>(Parser::parseObject);

		isInit = true;
	}

	static void* parseScene(DNAStructureReader& reader, Scene* scene) {
		reader.setStructure("id");
		cout << "READING " << reader.read<string>("name") << endl;
		//scene->name = reader.read<char>("name");
		return scene;
	}

	static void* parseObject(DNAStructureReader& reader, Object* obj) {
		return NULL;
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
