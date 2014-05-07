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

	string getType() {
		return structure->type->name;
	}

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

	DNAStructureReader& setStructure(DNAStructure* s, streamoff offset) {
		structure = s;
		currentOffset = offset;
		return *this;
	}

	//set another structure according to the provided field name
	DNAStructureReader& setStructure(string structureName) {
		if(!structure->hasField(structureName)) {
			ofLogWarning(OFX_BLENDER) << "Property \"" << structureName << "\" not found in " << structure->type->name;
			return *this;
		}
		DNAField& field = structure->getField(structureName);
		DNAStructure* s = file->catalog.getStructure(field.type->name);
		if(!s)
			return *this;
		setStructure(s, currentOffset + std::streampos(field.offset));
		return *this;
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

		//sometimes strings are stored as pointers to a string structure, make a shortcut for convenience
		if(field->isPointer && field->type->name == "char") {
			//pointers to char are actually pointers to Link objects
			DNAStructureReader link = readStructure(fieldName);
			return link.readString("next");
		}

		if(field->isArray || field->isPointer) {
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

	//get a vec2
	template<typename Type>
	ofVec3f readVec2(string fieldName) {
		ofVec3f ret;
		std::vector<Type> vals = readArray<Type>(fieldName);
		if(vals.size() >= 3) {
			ret.set(vals[0], vals[1]);
		}
		return ret;
	}
	ofVec2f readVec2f(string fieldName) {
		return readVec2<float>(fieldName);
	}

	//get a vec3
	template<typename Type>
	ofVec3f readVec3(string fieldName) {
		ofVec3f ret;
		std::vector<Type> vals = readArray<Type>(fieldName);
		if(vals.size() >= 3) {
			ret.set(vals[0], vals[1], vals[2]);
		}
		return ret;
	}
	ofVec3f readVec3f(string fieldName) {
		return readVec3<float>(fieldName);
	}

	template<typename Type>
	std::vector<ofVec3f> readVec3Array(string fieldName, unsigned int len=0) {
		std::vector<ofVec3f> ret;
		DNAField* field = setField(fieldName);
		if(!field) {
			return ret;
		}

		if(len == 0) {
			if(field->arrayDimensions > 1)
				len = field->arraySizes[0];
		}

		for(unsigned int i=0; i<len; i++) {
			Type x = file->read<Type>();
			Type y = file->read<Type>();
			Type z = file->read<Type>();
			ret.push_back(ofVec3f(x, y, z));
		}

		return ret;
	}
	std::vector<ofVec3f> readVec3fArray(string fieldName, unsigned int len=0) {
		return readVec3Array<float>(fieldName, len);
	}

	//get a pointer address
	unsigned long readAddress(string fieldName) {
		DNAField* field = setField(fieldName);
		if(!field) {
			return 0;
		}
		return file->readPointer();
	}

	//a linked list is basically an array of different structures
	std::vector<DNAStructureReader> readLinkedList(string fieldName) {
		std::vector<DNAStructureReader> ret;
		unsigned long address = readAddress(fieldName);
		if(address == 0)
			return ret;

		/*DNAStructureReader linkReader(block->file->getBlockByAddress(address));
		unsigned long first = linkReader.readAddress("first");
		if(first == 0){
			return ret;
		}*/

		//Get the first structure
		ret.push_back(DNAStructureReader(block->file->getBlockByAddress(address)));
		while(ret.back().readAddress("next")) {
			//and the next one
			ret.push_back(DNAStructureReader(block->file->getBlockByAddress(ret.back().readAddress("next"))));
		}

		return ret;
	}

	//get a reader for sturcture at address
	DNAStructureReader readStructure(string fieldName) {
		unsigned long address = readAddress(fieldName);
		if(address == 0) {
			ofLogWarning(OFX_BLENDER) << "DNAStructureReader::readStructure could not read structure \"" << fieldName << "\" in \"" << getType() << "\" returning self";
			return *this;
		}
		return DNAStructureReader(file->getBlockByAddress(address));
	}

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
			File::Block* dataBlock = reader.file->getBlockByAddress(reader.readAddress("data"));
			if(dataBlock) {
				DNAStructureReader dataReader(dataBlock);
				handler->call(dataReader, t);
			} else {
				ofLogWarning(OFX_BLENDER) << "ObjectHandler could not read datablock at pointer " << reader.readAddress("data");
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

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	static void parseScene(DNAStructureReader& reader, Scene* scene) {
		reader.setStructure("id");
		scene->name = reader.readString("name");
		reader.reset();

		//loop through all objects and add them to the scene
		DNAStructureReader next = reader.readStructure("base");
		do {
			//Parser::parseFileBlock(getBlockByType(BL_OBJECT, index))
			Object* object = reader.file->getObjectByAddress(next.readAddress("object"));
			scene->addObject(object);
			//cout << next.readStructure("object").setStructure("id").readString("name") << endl;
			if(next.readAddress("next") == 0) {
				break;
			}
			next = next.readStructure("next");
		} while(true);
		//cout << reader.readStructure("base").readStructure("object").setStructure("id").readString("name") << endl;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	static void parseObject(DNAStructureReader& reader, Object* object) {
		reader.setStructure("id");
		object->name = reader.readString("name");

		//get transformation
		reader.reset();
		object->setPosition(reader.readVec3f("loc"));
		object->setScale(reader.readVec3f("size"));

		//parse the anim data
		unsigned long animDataAddress = reader.readAddress("adt");
		if(animDataAddress != 0) {
			DNAStructureReader animReader(reader.file->getBlockByAddress(animDataAddress));
			DNAStructureReader actionReader = animReader.readStructure("action");
			//DNAStructureReader actionReader(animReader.file->getBlockByAddress(animReader.readAddress("action")));
			actionReader.setStructure("id");
			actionReader.reset();

			//load all curves
			vector<DNAStructureReader> curves = actionReader.readLinkedList("curves");
			for(DNAStructureReader& curve: curves) {
				string rnaPath = curve.readString("rna_path");
				int arrayIndex = curve.read<int>("array_index");
				string channel = rnaPath;
				string address = "";
				//if the channel is rotation, scale or translate, add an X
				if(channel == "location" || channel == "rotation" || channel == "scale") {
					if(arrayIndex == 0) {
						address = "x";
					} else if(arrayIndex == 1) {
						address = "y";
					} else if(arrayIndex == 2) {
						address = "z";
					}

					//create the animation
					TweenAnimation<float>* animation = new TweenAnimation<float>(channel, address);

					//add keyframes
					int numPoints = curve.read<int>("totvert");
					DNAStructureReader bezier = curve.readStructure("bezt");
					for(int i=0; i<numPoints; i++) {
						std::vector<ofVec3f> points = bezier.readVec3fArray("vec");
						//beziers come in this array [handle1X, handle1Y, 0], [Frame, Value, 0], [handle2X, handle2Y, 0]

						for(ofVec3f v:points) {
							//cout << v << endl;
						}

						//TODO: read proper frame rate
						float fps = 24; //default blender frame rate

						unsigned long long time = 1./fps * points[1][0];
						time *= 1000;

						animation->addKeyframe(time, points[1][1]);

						bezier.nextBlock();
					}

					//set the listener
					object->timeline.add(animation);

				} else {
					ofLogWarning(OFX_BLENDER) << "Unknown rna path " << rnaPath;
				}

			}

			//cout << curves[0].readStructure("driver").readString("expression") << endl;
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	static void parseMesh(DNAStructureReader& reader, Mesh* mesh) {
		reader.setStructure("id");
		mesh->meshName = reader.readString("name");

		reader.reset();

		//get address of the polygon blocks
		DNAStructureReader polyReader(reader.file->getBlockByAddress(reader.readAddress("mpoly")));
		//get address of the loops blocks
		DNAStructureReader loopReader(reader.file->getBlockByAddress(reader.readAddress("mloop")));
		//get address of the vertices blocks
		DNAStructureReader vertReader(reader.file->getBlockByAddress(reader.readAddress("mvert")));


		//read all vertices and add to the mesh
		mesh->mesh.clear();
		int totalVertices = reader.read<int>("totvert");
		for(int i=0; i<totalVertices; i++) {
			mesh->mesh.addVertex(vertReader.readVec3f("co"));
			mesh->mesh.addNormal(vertReader.readVec3<short>("no"));
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
			if(vertCount == 4) {
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
			} else {
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
