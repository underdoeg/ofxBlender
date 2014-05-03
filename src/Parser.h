#ifndef FILEPARSERS_H
#define FILEPARSERS_H

//all of the code is in the header file. this file should only be included once and that is by the file.cpp

#include "Scene.h"
#include "Object.h"
#include "File.h"
#include "Mesh.h"

enum BLENDER_TYPES {
    BL_MESH_ID = 1
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
		curBlock = 0;
		nextBlock();
	}

	//multiple structures can be integrated into one block, move ahead in the file
	void nextBlock() {
		setStructure(block->structure, block->offset + streampos(structure->type->size * curBlock));
		curBlock++;
	}

	//jump to a specific block
	void blockAt(unsigned int pos) {
		curBlock = pos;
		nextBlock();
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

	//get a vec3f
	ofVec3f readVec3(string fieldName) {
		ofVec3f ret;
		std::vector<float> floats = readArray<float>(fieldName);
		if(floats.size() >= 3) {
			ret.set(floats[0], floats[1], floats[2]);
		}
		//ret *= block->file->scale;
		return ret;
	}

	//get a pointer address
	unsigned long readPointer(string fieldName) {
		DNAField* field = setField(fieldName);
		if(!field) {
			return 0;
		}
		return file->readPointer();
	}
	/*
	std::vector<unsigned long> readPointerArray(string fieldName, unsigned int length) {
		std::vector<unsigned long> ret;
		DNAField* field = setField(fieldName);
		if(!field) {
			return ret;
		}

		if(!field->isArray && !field->isPointer) {
			ofLogWarning(OFX_BLENDER) << "Property \"" << fieldName << "\" is not an array type";
			return ret;
		}

		for(unsigned int i=0; i < length; i++) {
			ret.push_back(file->readPointer());
		}

		return ret;
	}
	*/

	//get an object at pointer address

	File* file;

private:
	DNAStructure* structure;
	File::Block* block;
	streamoff currentOffset;
	unsigned int curBlock;
};

//////////////////////////////////////////////////////////////////////////////////////// PARSER

class Parser {
public:

	//handler wrapers to automate the process of updating / creating new types
	class Handler_ {
	public:
		virtual void* call(DNAStructureReader&) = 0;
		virtual void* call(DNAStructureReader&, void*) = 0;
		virtual void* create() {
			return NULL;
		}
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

		void* create() {
			return new Type();
		}

		funcType function;
	};

	//ObjectHandlers are special because they have to check what kind of object it is first and then call the right parser
	class ObjectHandler: public Handler_ {
	public:
		typedef std::function<void(DNAStructureReader&, Object*)> ObjFunction;
		ObjectHandler(ObjFunction func) {
			objFunction = func;
		}

		void* call(DNAStructureReader& reader) {
			Handler_* handler = getHandlerFor(getTypeId(reader));
			if(!handler)
				return NULL;
			return call(reader, handler->create());
		}

		void* call(DNAStructureReader&  reader, void* obj) {
			//check if we have a handler for the type
			Handler_* handler = getHandlerFor(getTypeId(reader));
			if(!handler)
				return NULL;
			Object* t = static_cast<Object*>(obj);
			//parse all object parameters first
			objFunction(reader, t);
			//and then load the right data block and call the object
			File::Block* dataBlock = reader.file->getBlockByAddress(reader.readPointer("data"));
			if(dataBlock) {
				DNAStructureReader dataReader(dataBlock);
				handler->call(dataReader, t);
			} else {
				ofLogWarning(OFX_BLENDER) << "ObjectHandler could not read datablock at pointer " << reader.readPointer("data");
			}
			return obj;
		}

		template<typename Type>
		void addHandler(int typeId, std::function<void(DNAStructureReader&, Type*)> func) {
			handlers[typeId] = new Handler<Type>(func);
		}

	private:

		int getTypeId(DNAStructureReader& reader) {
			return reader.read<short>("type");
		}

		Handler_* getHandlerFor(int type) {
			if(handlers.find(type) != handlers.end()) {
				return handlers[type];
			}
			return NULL;
		}
		std::map<int, Handler_*> handlers;
		ObjFunction objFunction;
	};


	//now handle all the parsing
	static void init() {
		if(isInit)
			return;

		Parser::handlers[BL_SCENE] = new Handler<Scene>(Parser::parseScene);

		//Types based on objects like Mesh, Camera, Light are special and need to be registered with the ObjectHandler
		ObjectHandler* objHandler = new ObjectHandler(Parser::parseObject);
		objHandler->addHandler<Mesh>(BL_MESH_ID, Parser::parseMesh);
		Parser::handlers[BL_OBJECT] = objHandler;

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
		//cout << "DATA " << reader.readPointer("data") << endl;
		//cout << "TYPE " << reader.read<short>("type") << endl;
	}

	static void parseMesh(DNAStructureReader& reader, Mesh* mesh) {
		reader.setStructure("id");
		mesh->meshName = reader.readString("name");

		reader.reset();

		//get address of the polygon blocks
		DNAStructureReader polyReader(reader.file->getBlockByAddress(reader.readPointer("mpoly")));
		//get address of the loops blocks
		DNAStructureReader loopReader(reader.file->getBlockByAddress(reader.readPointer("mloop")));
		//get address of the vertices blocks
		DNAStructureReader vertReader(reader.file->getBlockByAddress(reader.readPointer("mvert")));


		//read all vertices and add to the mesh
		mesh->mesh.clear();
		int totalVertices = reader.read<int>("totvert");
		for(int i=0; i<totalVertices; i++) {
			mesh->mesh.addVertex(vertReader.readVec3("co"));
			//normals are stored as shorts
			std::vector<short> norms = vertReader.readArray<short>("no");
			mesh->mesh.addNormal(ofVec3f(norms[0], norms[1], norms[2]));
			vertReader.nextBlock();
		}

		//get the total number of polygons
		int totalPolys = reader.read<int>("totpoly");
		for(int i=0; i<totalPolys; i++) {
			unsigned int vertCount = polyReader.read<int>("totloop");
			if (vertCount<3) {
				ofLogWarning(OFX_BLENDER) << "can't convert polygon with only 2 or less vertices";
				continue;
			}
			if (vertCount>4) {
				ofLogWarning(OFX_BLENDER) << "can't convert polygon with more than 4 vertices";
				continue;
			}
			int loopStart = polyReader.read<int>("loopstart");
			if(vertCount == 4){
				loopReader.blockAt(loopStart);
				unsigned int index1 = loopReader.read<int>("v");
				loopReader.nextBlock();
				unsigned int index2 = loopReader.read<int>("v");
				loopReader.nextBlock();
				unsigned int index3 = loopReader.read<int>("v");
				loopReader.nextBlock();
				unsigned int index4 = loopReader.read<int>("v");
				loopReader.nextBlock();
				mesh->mesh.addTriangle(index3, index2, index1);
				mesh->mesh.addTriangle(index3, index4, index1);
			}else{
				loopReader.blockAt(loopStart);
				unsigned int index1 = loopReader.read<int>("v");
				loopReader.nextBlock();
				unsigned int index2 = loopReader.read<int>("v");
				loopReader.nextBlock();
				unsigned int index3 = loopReader.read<int>("v");
				mesh->mesh.addTriangle(index1, index2, index3);
			}

			//done, let's advance to the next polygon
			polyReader.nextBlock();
		}
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
