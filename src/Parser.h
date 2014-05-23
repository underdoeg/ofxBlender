#ifndef FILEPARSERS_H
#define FILEPARSERS_H

//all of the code is in the header file. this file should only be included once and that is by the file.cpp

#include "Scene.h"
#include "Object.h"
#include "File.h"
#include "Mesh.h"
#include "Material.h"

enum BLENDER_TYPES {
    BL_EMPTY_ID = 0,
    BL_MESH_ID = 1,
    BL_TEXT_ID = 4,
    BL_LIGHT_ID = 10,
    BL_CAM_ID = 11
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
	std::vector<Type> readVector(string fieldName) {
		std::vector<Type> ret;
		DNAField* field = setField(fieldName);
		if(!field) {
			return ret;
		}
		if(!field->isArray) {
			ofLogWarning(OFX_BLENDER) << "Property \"" << fieldName << "\" is not an array type";
			return ret;
		}

		if(field->arrayDimensions > 1) {
			ofLogWarning(OFX_BLENDER) << "Property \"" << fieldName << "\" has more than one array dimension, use readMultArray";
		}

		if(field->arraySizes[0] != 0) {
			for(unsigned int i=0; i < field->arraySizes[0]; i++) {
				ret.push_back(file->read<Type>());
			}
		}

		return ret;
	}

	std::vector<unsigned long> readAddressArray(string fieldName) {
		std::vector<unsigned long> ret;
		DNAField* field = setField(fieldName);
		if(!field) {
			return ret;
		}

		//cout << field->arraySizes[0] << endl;

		if(!field->isArray) {
			ofLogWarning(OFX_BLENDER) << "Property \"" << fieldName << "\" is not an array type";
			return ret;
		}

		if(field->arraySizes[0] > 0) {
			for(unsigned int i=0; i < field->arraySizes[0]; i++) {
				unsigned long ptr = file->readPointer();
				if(ptr != 0)
					ret.push_back(ptr);
			}
		} else {
			while(true) {
				unsigned long addr = file->readPointer();
				if(addr == 0)
					break;
				else
					ret.push_back(addr);
			}
		}
		return ret;
	}

	std::vector<DNAStructureReader> readStructureArray(string fieldName) {
		std::vector<unsigned long> addresses = readAddressArray(fieldName);
		std::vector<DNAStructureReader> ret;
		for(unsigned long& address: addresses) {
			File::Block* block = file->getBlockByAddress(address);
			if(block != NULL) {
				ret.push_back(DNAStructureReader(block));
			}
		}
		return ret;
	};

	template<typename Type>
	std::vector<std::vector<Type> > readMultArray(string fieldName) {
		std::vector<std::vector<Type> > ret;
		DNAField* field = setField(fieldName);
		if(!field) {
			return ret;
		}
		if(!field->isArray) {
			ofLogWarning(OFX_BLENDER) << "Property \"" << fieldName << "\" is not an array type";
			return ret;
		}

		if(field->arrayDimensions <= 1)
			ofLogWarning(OFX_BLENDER) << "Property \"" << fieldName << "\" has only one array dimension, use readArray";

		for(unsigned int i=0; i < field->arraySizes[0]; i++) {
			std::vector<Type> temp;
			for(unsigned int j=0; j<field->arraySizes[1]; j++) {
				temp.push_back(file->read<Type>());
			}
			ret.push_back(temp);
		}


		return ret;
	}

	template<typename Type>
	Type read(string fieldName) {

		//check for specials functions
		if(std::is_same<Type, std::string>::value) {
			ofLogWarning(OFX_BLENDER) << "DNAStructureReader::read string types should be read with readString";
			//return Type();
		}

		bool isPointer = std::is_pointer<Type>::value;

		DNAField* field = setField(fieldName);
		if(!field) {
			return Type();
		}

		if(field->isArray) {
			ofLogWarning(OFX_BLENDER) << "DNAStructureReader::read field is an array and should be read with readArray";
			//return Type();
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
	ofVec2f readVec2(string fieldName) {
		ofVec2f ret;
		std::vector<Type> vals = readVector<Type>(fieldName);
		if(vals.size() >= 2) {
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
		std::vector<Type> vals = readVector<Type>(fieldName);
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

	//get an array
	template<typename Type>
	Type* readArray(string fieldName, unsigned int amount) {
		DNAField* field = setField(fieldName);
		if(!field) {
			return new Type[0];
		}
		return file->readMany<Type>(amount);
	}

//get a pointer address
	unsigned long readAddress(string fieldName) {
		DNAField* field = setField(fieldName);
		if(!field) {
			return 0;
		}
		return file->readPointer();
	}

	char* readChar(string fieldName, unsigned int length) {
		setField(fieldName);
		return file->readChar(length);
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

	std::vector<DNAStructureReader> readLinkAsList(string fieldName) {
		//TODO: I don't really get how these Links work, doublecheck

		std::vector<DNAStructureReader> ret;
		unsigned long address = readAddress(fieldName);
		if(address == 0)
			return ret;


		//go back to the first structure
		DNAStructureReader link = readStructure(fieldName);

		if(file->doesAddressExist(link.readAddress("next"))) {
			ret.push_back(link.readStructure("next"));
		}

		if(file->doesAddressExist(link.readAddress("prev"))) {
			DNAStructureReader first = link.readStructure("prev");
			while(first.hasPrev()) {
				first = first.getPrev();
			}

			ret.push_back(first);

			while(first.hasNext()) {
				first = first.getNext();
				ret.push_back(first);
			}
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
		File::Block* block = file->getBlockByAddress(address);
		if(block == NULL) {
			ofLogWarning(OFX_BLENDER) << "DNAStructureReader::readStructure could not read structure \"" << fieldName << "\" in \"" << getType() << "\" returning self";
			return *this;
		}
		return DNAStructureReader(block);
	}

	bool hasNext() {
		if(!structure->hasField("id"))
			return false;
		setStructure("id");
		unsigned long addr = readAddress("next");
		if(addr!=0 && !file->doesAddressExist(addr)) {
			addr = 0;
		}
		reset();
		return  addr != 0;
	}

	DNAStructureReader getNext() {
		setStructure("id");
		DNAStructureReader ret = readStructure("next");
		//if(ret.getType() == "ID")
		//	ret.reset();
		reset();
		return ret;
	}

	bool hasPrev() {
		if(!structure->hasField("id"))
			return false;
		setStructure("id");
		unsigned long addr = readAddress("prev");
		if(addr!=0 && !file->doesAddressExist(addr)) {
			addr = 0;
		}
		reset();
		return  addr != 0;
	}

	DNAStructureReader getPrev() {
		setStructure("id");
		DNAStructureReader ret = readStructure("prev");
		//if(ret.getType() == "ID")
		//	ret.reset();
		reset();
		return ret;
	}

	void* parse() {
		return file->parseFileBlock(block);
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
			//cout << getTypeId(reader) << endl;

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
		Parser::handlers[BL_MATERIAL] = new Handler<Material>(Parser::parseMaterial);
		Parser::handlers[BL_TEXTURE] = new Handler<Texture>(Parser::parseTexture);

		//Types based on objects like Mesh, Camera, Light are special and need to be registered with the ObjectHandler
		ObjectHandler* objHandler = new ObjectHandler(Parser::parseObject);
		objHandler->addHandler<Mesh>(BL_MESH_ID, Parser::parseMesh);
		objHandler->addHandler<Camera>(BL_CAM_ID, Parser::parseCamera);
		objHandler->addHandler<Light>(BL_LIGHT_ID, Parser::parseLight);
		Parser::handlers[BL_OBJECT] = objHandler;

		isInit = true;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	static void parseScene(DNAStructureReader& reader, Scene* scene) {
		reader.setStructure("id");
		scene->name = reader.readString("name");
		reader.reset();

		//read render settings
		reader.setStructure("r");
		short fps = reader.read<short>("frs_sec");
		scene->timeline.setLoop(true);
		scene->timeline.setDuration(1.f/(float)fps * (float)reader.read<int>("efra") * 1000);
		reader.reset();

		//Timeline infos
		//cout << "LENGTH " << reader.readStructure("fps_info").getType() << endl;

		//loop through all objects and add them to the scene
		DNAStructureReader next = reader.readStructure("base");
		do {
			//Parser::parseFileBlock(getBlockByType(BL_OBJECT, index))
			Object* object = reader.file->getObjectByAddress(next.readAddress("object"));
			if(object != NULL)
				scene->addObject(object);
			//cout << next.readStructure("object").setStructure("id").readString("name") << endl;
			if(next.readAddress("next") == 0) {
				break;
			}
			next = next.readStructure("next");
		} while(true);

		//set active camera
		if(reader.readAddress("camera") != 0) {
			scene->setActiveCamera(static_cast<Camera*>(reader.file->getObjectByAddress(reader.readAddress("camera"))));
		}

		//WORLD INFOS
		DNAStructureReader worldReader = reader.readStructure("world");
		scene->bgColor = ofFloatColor(worldReader.read<float>("horr"), worldReader.read<float>("horg"), worldReader.read<float>("horb"));
		//cout << "COLOR MODEL " << worldReader.read<short>("colormodel") << endl;
		//cout << reader.readStructure("base").readStructure("object").setStructure("id").readString("name") << endl;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	static void parseObject(DNAStructureReader& reader, Object* object) {
		reader.setStructure("id");
		object->name = reader.readString("name");
		reader.reset();

		//get transformation
		vector<vector<float> > matArray = reader.readMultArray<float>("obmat");
		ofMatrix4x4 mat(matArray[0][0], matArray[0][1], matArray[0][2], matArray[0][3],
		                matArray[1][0], matArray[1][1], matArray[1][2], matArray[1][3],
		                matArray[2][0], matArray[2][1], matArray[2][2], matArray[2][3],
		                matArray[3][0], matArray[3][1], matArray[3][2], matArray[3][3]);

		//object->setPosition(object->getPosition()*reader.file->scale);
		object->setTransformMatrix(mat);

		//check for parent
		unsigned long parentAddress = reader.readAddress("parent");
		if(parentAddress != 0){
			DNAStructureReader parentReader = reader.readStructure("parent");
			Object* parent = static_cast<Object*>(parentReader.parse());
			parent->addChild(object);
			object->setTransformMatrix(mat * ofMatrix4x4::getInverseOf(parent->getGlobalTransformMatrix()));
			//object->setTransformMatrix(parent->getGlobalTransformMatrix().getInverse() * mat);
		}
		
		//parse the anim data
		unsigned long animDataAddress = reader.readAddress("adt");
		if(animDataAddress != 0) {
			DNAStructureReader animReader(reader.file->getBlockByAddress(animDataAddress));
			if(animReader.readAddress("action") != 0) {
				DNAStructureReader actionReader = animReader.readStructure("action");
				//actionReader.setStructure("id");
				//actionReader.reset();

				//load all curves
				vector<DNAStructureReader> curves = actionReader.readLinkedList("curves");
				for(DNAStructureReader& curve: curves) {
					string rnaPath = curve.readString("rna_path");
					cout << rnaPath << endl;
					int arrayIndex = curve.read<int>("array_index");
					string address = rnaPath;
					int channel = arrayIndex;
					//if the channel is rotation, scale or translate, add an X
					if(address == "location" || address == "rotation" || address == "scale" || address=="rotation_euler") {

						//create the animation
						BezierAnimation<float>* animation = new BezierAnimation<float>(address, channel);

						//add keyframes
						int numPoints = curve.read<int>("totvert");
						DNAStructureReader bezier = curve.readStructure("bezt");
						for(int i=0; i<numPoints; i++) {
							std::vector<ofVec3f> points = bezier.readVec3fArray("vec");

							short ipo = bezier.read<short>("ipo");

							//beziers come in this array [handle1X, handle1Y, 0], [Frame, Value, 0], [handle2X, handle2Y, 0]

							//TODO: read proper frame rate
							float fps = 24; //default blender frame rate

							unsigned long long time = 1./fps * points[1][0];
							time *= 1000;

							if(ipo == 1)
								animation->addKeyframe(time, points[1][1]);
							else if(ipo == 2)
								animation->addKeyframe(time, points[1][1], points[1], points[0], points[2]);
							else if(ipo == 0)
								ofLogWarning(OFX_BLENDER) << "Constant Keyframes are not yet supported";
							bezier.nextBlock();
						}

						//DNAStructureReader fpt = curve.readStructure("fpt");

						//set the listener
						object->timeline.add(animation);

					} else {
						ofLogWarning(OFX_BLENDER) << "Unknown rna path " << rnaPath;
					}
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

		enum DrawFlag {
		    DRAW_FLAT = 67,
		    DRAW_SMOOTH = 75
		};

		//two sided
		//TODO: check how flags actually work, this will only be right when flag is exactly 4
		if(reader.read<short>("flag") == 4){
			mesh->isTwoSided = true;
		}else{
			mesh->isTwoSided = false;
		}


		//get address of the polygon blocks
		DNAStructureReader polyReader = reader.readStructure("mpoly");
		//get address of the loops blocks
		DNAStructureReader loopReader = reader.readStructure("mloop");
		//get address of the vertices blocks
		DNAStructureReader vertReader = reader.readStructure("mvert");
		//get address of the edge blocks
		DNAStructureReader edgeReader = reader.readStructure("medge");

		//read all vertices and add to the mesh
		mesh->clear();
		unsigned int totalVertices = reader.read<int>("totvert");
		for(unsigned int i=0; i<totalVertices; i++) {
			//cout << vertReader.readVec3f("co") << endl;
			mesh->addVertex(vertReader.readVec3f("co"), vertReader.readVec3<short>("no").getNormalized());
			vertReader.nextBlock();
		}

		//read all Materials
		std::vector<Material*> materials;
		std::vector<DNAStructureReader> materialStructs = reader.readLinkAsList("mat");
		for(DNAStructureReader& matReader: materialStructs) {
			materials.push_back(static_cast<Material*>(matReader.parse()));
		}

		//try to read uv coordinates
		bool hasUV = false;
		DNAStructureReader uvReader = reader;
		if(reader.readAddress("mloopuv") != 0) {
			uvReader = reader.readStructure("mloopuv");
			hasUV = true;
		}

		//get the total number of polygons
		int totalPolys = reader.read<int>("totpoly");
		ofVec3f e0, e1;
		//build triangles
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

			//check the shading
			Shading shading = FLAT;
			if(((int)polyReader.read<char>("flag")) == 3) {
				shading = SMOOTH;
			}
			mesh->pushShading(shading);

			//pick the material
			unsigned int materialNumber = polyReader.read<short>("mat_nr");
			if(materialNumber < materials.size())
				mesh->pushMaterial(materials[materialNumber]);

			//write triangles
			int loopStart = polyReader.read<int>("loopstart");

			if(vertCount == 4) {
				loopReader.blockAt(loopStart);

				//cout << loopReader.read<int>("e") << endl;

				unsigned int index1 = loopReader.read<int>("v");
				loopReader.nextBlock();
				unsigned int index2 = loopReader.read<int>("v");
				loopReader.nextBlock();
				unsigned int index3 = loopReader.read<int>("v");
				loopReader.nextBlock();
				unsigned int index4 = loopReader.read<int>("v");
				loopReader.nextBlock();

				e0 = mesh->getVertex(index1) - mesh->getVertex(index2);
				e1 = mesh->getVertex(index3) - mesh->getVertex(index4);

				if(e0.lengthSquared() < e1.lengthSquared()) {
					mesh->addTriangle(index1, index2, index3);
					mesh->addTriangle(index3, index4, index1);
				} else {
					mesh->addTriangle(index1, index2, index4);
					mesh->addTriangle(index4, index2, index3);
				}

				if(hasUV) {
					uvReader.blockAt(loopStart);
					mesh->setUV(index1, uvReader.readVec2f("uv"));
					uvReader.nextBlock();
					mesh->setUV(index2, uvReader.readVec2f("uv"));
					uvReader.nextBlock();
					mesh->setUV(index3, uvReader.readVec2f("uv"));
					uvReader.nextBlock();
					mesh->setUV(index4, uvReader.readVec2f("uv"));
				}
			} else {
				loopReader.blockAt(loopStart);
				unsigned int index1 = loopReader.read<int>("v");
				loopReader.nextBlock();
				unsigned int index2 = loopReader.read<int>("v");
				loopReader.nextBlock();
				unsigned int index3 = loopReader.read<int>("v");
				mesh->addTriangle(index1, index2, index3);

				if(hasUV) {
					uvReader.blockAt(loopStart);
					mesh->setUV(index1, uvReader.readVec2f("uv"));
					uvReader.nextBlock();
					mesh->setUV(index2, uvReader.readVec2f("uv"));
					uvReader.nextBlock();
					mesh->setUV(index3, uvReader.readVec2f("uv"));
				}
			}

			//done, let's advance to the next polygon
			polyReader.nextBlock();
		}
	}

	static void parseMaterial(DNAStructureReader& reader, Material* material) {
		reader.setStructure("id");
		material->name = reader.readString("name");
		reader.reset();

		material->material.setShininess(reader.read<float>("spec"));


		//TODO: why are those colors flipped?
		material->material.setSpecularColor(ofFloatColor(reader.read<float>("r"), reader.read<float>("g"), reader.read<float>("b")));
		material->material.setAmbientColor(ofFloatColor(reader.read<float>("r"), reader.read<float>("g"), reader.read<float>("b")));
		material->material.setDiffuseColor(ofFloatColor(reader.read<float>("specr"), reader.read<float>("specg"), reader.read<float>("specb")));

		std::vector<DNAStructureReader> textures = reader.readStructureArray("mtex");
		for(DNAStructureReader& texReader: textures) {
			material->textures.push_back(static_cast<Texture*>(texReader.readStructure("tex").parse()));
		}
	}

	static void parseTexture(DNAStructureReader& reader, Texture* texture) {
		reader.setStructure("id");
		texture->name = reader.readString("name");
		reader.reset();

		//there are many more types
		enum texTypes {
		    BLEND = 1,
		    WOOD = 2,
		    VOXEL = 15,
		    IMAGE = 8
		};

		if(reader.read<short>("type") != IMAGE) {
			ofLogWarning(OFX_BLENDER) << "CAN ONLY READ TEXTURES OF TYPE IMAGE \"" << texture->name << "\"";
			return;
		}

		DNAStructureReader imgReader = reader.readStructure("ima");

		//check if file is packed or has to be loaded
		if(imgReader.readAddress("packedfile")) {
			DNAStructureReader packedFile = imgReader.readStructure("packedfile");
			unsigned int size = packedFile.read<int>("size");
			DNAStructureReader dataBlock = packedFile.readStructure("data");

			char* pixels = dataBlock.readChar("next", size);
			ofBuffer buffer(pixels, size);
			texture->img.loadImage(buffer);
		} else {
			string path = imgReader.readString("name");
			ofStringReplace(path, "//", "");
			texture->img.loadImage(path);
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	static void parseCamera(DNAStructureReader& reader, Camera* cam) {
		ofCamera* camera = &cam->camera;
		float fov = ofRadToDeg(2 * atan(16 / reader.read<float>("lens")));
		camera->setFov(fov);

		camera->setNearClip(reader.read<float>("clipsta"));
		camera->setFarClip(reader.read<float>("clipend"));

		//camera->setupPerspective(true, fov, 0, 1000000);
	}

	static void parseLight(DNAStructureReader& reader, Light* light) {
		//http://en.wikibooks.org/wiki/GLSL_Programming/Blender/Diffuse_Reflection
		enum LightType {
		    BL_POINT = 0,
		    BL_SUN = 1,
		    BL_SPOT = 2,
		    BL_HEMI = 3,
		    BL_AREA = 4
		};

		//TODO: the lights get kind of parsed, but there is lots to do, best would be to use blenders shaders

		unsigned int type = reader.read<short>("type");
		if(type == BL_POINT) {
			light->light.setPointLight();
			float energy = reader.read<float>("energy");
			if(energy == 0)
				energy = .00001;
			float distance = 1.f / reader.read<float>("dist");
			light->light.setAttenuation(1.f / energy, reader.read<float>("att1") * distance, reader.read<float>("att2") * distance);
		} else if(type == BL_SUN) {
			light->light.setDirectional();	
			light->light.tilt(180);
		} else if(type == BL_SPOT) {
			light->light.setSpotlight(ofRadToDeg(reader.read<float>("spotsize"))*.5, (1-reader.read<float>("spotblend"))*128);
		} else if(type == BL_HEMI) {
		} else {
			ofLogWarning(OFX_BLENDER) << "Light \"" << light->name << "\" has an unsupported type";
		}

		light->light.setDiffuseColor(ofFloatColor(reader.read<float>("r"), reader.read<float>("g"), reader.read<float>("b")));
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
		ofLogWarning(OFX_BLENDER) << "No Parser for Object Type \"" << block->structure->type->name << "\" found";
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
