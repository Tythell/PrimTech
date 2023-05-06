
#include"pch.h"
#include "Serializer.h"

namespace PrimtTech
{

//#define WRITE(content) exporter.write((const char*)content, sizeof(content))
//#define WRITEA(content) exporter.write((const char*)&content, sizeof(content))

#define WRITEB(a, content, size) \
	exporter.write((const char*)a##content, size); \

#define WRITEA(a, content, size) \
	exporterAsci  << "(" << content << ") " << size << "\n"; \

#define WRITE(a, content, size) \
	exporter.write((const char*)a##content, size); \
	exporterAsci << #content << "(" << content << ") " << size << "\n"; \

#define WRITESHIT(shit) exporterAsci << "------------- " << shit << " -------------\n";

#define READ(a, content, size) importer.read((char*)a##content, size)
//#define READ(content) importer.read((char*)&content, sizeof(content))
//#define READ(content) importer.read((char*)content, sizeof(content))

	void NewScene(std::vector<pt::Entity>& entList)
	{
		Import("Default3DScene.ptsc", entList);
	}

	void Export(std::string path, std::vector<pt::Entity>& entList)
	{
		std::ofstream exporter(path, std::ios::out | std::ios::binary);
		std::ofstream exporterAsci(path + "ascii", std::ios::out);
		if (!exporter.is_open())
		{
			return;
		}

		uint magicNum = 0x6969;
		//WRITE(magicNum); // Magic number
		WRITE(&, magicNum, sizeof(float));

		const char pakPath[32] = "No paks?.pak";
		WRITE(, pakPath, 32);

		WRITESHIT("AssetNames");
		WRITESHIT("MeshesNames");

		uint numMeshes = ResourceHandler::GetNoMeshes();
		WRITE(&, numMeshes, sizeof(uint));

		char nameOfAsset[32]{""};

		for (uint i = 0; i < numMeshes; i++)
		{
			strcpy_s(nameOfAsset, ResourceHandler::GetMesh(i).GetName().c_str());
			WRITE(, nameOfAsset, 32);
		}

		uint numMaterials = ResourceHandler::GetNoMaterials();
		std::vector<Material>& mats = ResourceHandler::GetMaterialArrayReference();
		WRITE(&, numMaterials, sizeof(uint));

		WRITESHIT("Material");

		for (uint i = 0; i < numMaterials; i++)
		{
			strcpy_s(nameOfAsset, ResourceHandler::GetMaterial(i).GetFileName().c_str());
			WRITE(, nameOfAsset, 32);

			std::string matPat = "Assets/pmtrl/";
			matPat += nameOfAsset;
			matPat += ".pmtrl";

			mats[i].ExportMaterial(matPat);
		}

		int numEnts = pt::Entity::GetNoUsedEnts();
		WRITE(&, numEnts, sizeof(int));


		WRITESHIT("entTables")

		// exporting entity component tables
		for (int i = 0; i < numEnts; i++)
		{
			// number of components
			int numComponents = entList[i].CalculateNrOfComponents();
			char entName[32]{ "" };
			strcpy_s(entName, entList[i].GetName().c_str());
			WRITE(, entName, 32);
			WRITE(&, numComponents, sizeof(int));

			std::map<PrimtTech::HasComponent, uint> compTable = entList[i].GetCompTable();
			for (auto const& [key, val] : compTable) // might be slow but works for now and probably ever
			{
				WRITE(&, key, sizeof(HasComponent))
				WRITE(&, val, sizeof(uint))
			}
		}
		// ------------------------------------------- export components -------------------------------------------
		
		WRITESHIT("Transforms")

		// ------------------ transforms ------------------
		std::vector<pt::TransformComp>& transforms = ComponentHandler::GetComponentArray<pt::TransformComp>();
		int numComps = -1;
		GETCOMPNUMUSED(pt::TransformComp, numComps);
		PrimtTech::HasComponent compTypeId = PrimtTech::ec_transform;
		WRITE(&, compTypeId, sizeof(PrimtTech::HasComponent));
		WRITE(&, numComps, sizeof(int));
		for (int i = 0; i < transforms.size(); i++)
		{
			EntIdType entId = transforms[i].EntId();
			WRITE(&, entId, sizeof(EntIdType));

			float f3[3]{ 0.f };
			sm::Vector3 vec = transforms[i].GetPosition();
			memcpy(f3, &vec.x, sizeof(float) * 3);
			WRITE(, f3, sizeof(float) * 3);

			vec = transforms[i].GetRotation();
			memcpy(f3, &vec.x, sizeof(float) * 3);
			WRITE(,f3, sizeof(float) * 3);

			vec = transforms[i].GetScale();
			memcpy(f3, &vec.x, sizeof(float) * 3);
			WRITE(,f3, sizeof(float) * 3);
		}
		WRITESHIT("Msshrefs")
		// ------------------ meshrefs ------------------
		std::vector<pt::MeshRef>& meshrefs = ComponentHandler::GetComponentArray<pt::MeshRef>();
		GETCOMPNUMUSED(pt::MeshRef, numComps);
		compTypeId = PrimtTech::ec_meshRef;
		WRITE(&, compTypeId, sizeof(PrimtTech::HasComponent));
		WRITE(&, numComps, sizeof(int));
		char meshname[64]{ "" };
		for (int i = 0; i < meshrefs.size(); i++)
		{
			EntIdType entId = meshrefs[i].EntId();
			WRITE(&, entId, sizeof(EntIdType));
			strcpy_s(meshname, meshrefs[i].GetNameOfMesh().c_str());
			WRITE(,meshname, 64);
			uint numMats = meshrefs[i].GetNumMaterials();
			WRITE(&, numMats, sizeof(uint));
			for (int j = 0; j < numMats; j++)
			{
				uint index = meshrefs[i].GetMaterialIndex(j);
				WRITE(&, index, sizeof(uint));
			}
		}
		WRITESHIT("cams")
		// ------------------ cams ------------------
		GETCOMPVEC(pt::Camera, cams);
		GETCOMPNUMUSED(pt::Camera, numComps);
		//numComps = cams.size();
		compTypeId = PrimtTech::ec_cam;
		WRITE(&, compTypeId, sizeof(PrimtTech::HasComponent));
		WRITE(&, numComps, sizeof(int));
		for (int i = 0; i < numComps; i++)
		{
			EntIdType entId = cams[i].EntId();
			WRITE(&, entId, sizeof(EntIdType));

			float fov = 80.f;
			WRITE(&, fov, sizeof(float));

			float f3[3]{ 0.f };
			sm::Vector3 vec = cams[i].GetPositionOffset();
			memcpy(f3, &vec.x, sizeof(float) * 3);
			WRITE(,f3, sizeof(float) * 3);
			//exporter << f3[0] << " " << f3[1] << " " << f3[2] << " posoffset" << std::endl;

			vec = cams[i].GetRotationOffset();
			memcpy(f3, &vec.x, sizeof(float) * 3);
			WRITE(,f3, sizeof(float) * 3);

			// export view and projection
			const int MATRIXSIZE = sizeof(float) * 16;
			sm::Matrix mat = cams[i].GetViewMatrix();
			float f16[16]{};
			memcpy(f16, &mat._11, MATRIXSIZE);
			WRITE(,f16, MATRIXSIZE);

			mat = cams[i].GetProjMatrix();
			memcpy(f16, &mat._11, MATRIXSIZE);
			WRITE(,f16, MATRIXSIZE);
		}

		// ------------------ Lights ------------------
		WRITESHIT("Lights");
		GETCOMPVEC(pt::Light, lights);
		GETCOMPNUMUSED(pt::Light, numComps);
		compTypeId = PrimtTech::ec_light;
		WRITE(&,compTypeId, sizeof(PrimtTech::HasComponent));
		WRITE(&, numComps, sizeof(int));
		for (int i = 0; i < numComps; i++)
		{
			EntIdType u = lights[i].EntId();
			WRITE(&, u, sizeof(EntIdType));
			uchar uc = lights[i].GetType();
			WRITE(&, uc, sizeof(uchar));

			hlsl::Light lightData = lights[i].GetLightData();
			WRITEB(&, lightData, sizeof(hlsl::Light));
			WRITEA(&, "light struct", sizeof(hlsl::Light));

			sm::Vector4 v4 = lights[i].GetPositionOffset();
			float f3[3]{ 0 };
			memcpy(f3, &v4.x, sizeof(float) * 3);
			WRITE(&, f3, sizeof(float) * 3);

			v4 = lights[i].GetDirectionOffset();
			memcpy(f3, &v4.x, sizeof(float) * 3);
			WRITE(&, f3, sizeof(float) * 3);
		}

		// ------------------------------------ PhysBodies ------------------------------------
		WRITESHIT("PhysBodys");
		GETCOMPVEC(pt::PhysicsBody, pBodys);
		GETCOMPNUMUSED(pt::PhysicsBody, numComps);
		compTypeId = PrimtTech::ec_rigidBodies;
		WRITE(&,compTypeId, sizeof(PrimtTech::HasComponent));
		WRITE(&, numComps, sizeof(int));
		for (int i = 0; i < numComps; i++)
		{
			EntIdType u = pBodys[i].EntId();
			WRITE(&, u, sizeof(EntIdType));
			rp::BodyType bodyType = pBodys[i].GetType();
			WRITEB(&, bodyType, sizeof(rp::BodyType));
			WRITEA(&, "bodyType", sizeof(rp::BodyType));

			uint numColliders = pBodys[i].GetNoColliders();
			WRITE(&, numColliders, sizeof(uint));

			for (int j = 0; j < numColliders; j++)
			{
				pt::PhysicsBody::ePT_ShapeType colliderType = pBodys[i].GetColliderType(j);
				WRITEB(&, colliderType, sizeof(pt::PhysicsBody::ePT_ShapeType));
				WRITEA(&, "colliderType", sizeof(pt::PhysicsBody::ePT_ShapeType));
				bool isTrigger = pBodys[i].GetIsTrigger(j);
				WRITE(&, isTrigger, sizeof(bool));

				sm::Vector3 vec3;
				int writeSize = sizeof(float);

				switch (colliderType)
				{
				case pt::PhysicsBody::ePT_ShapeType::Box:
				{
					vec3 = pBodys[i].GetExtents(j);
					writeSize *= 3;
					break;
				}
				case pt::PhysicsBody::ePT_ShapeType::Sphere:
				{
					vec3.x = pBodys[i].GetSphereRadius(j);
					//writeSize *= 1;
					break;
				}
				case pt::PhysicsBody::ePT_ShapeType::Capsule:
				{
					writeSize *= 2;
					sm::Vector2 vec2 = pBodys[i].GetCapsuleLengths(j);
					memcpy(&vec3.x, &vec2.x, writeSize);
					//vec3.x = pBodys[i].GetCapsuleLengths(j).x;
					//vec3.y = pBodys[i].GetCapsuleLengths(j).y;
					break;
				}
				default:
					break;
				}
				WRITE(&, vec3.x, writeSize);
			}
		}

		// ------------------------------------ Scripts ------------------------------------

		WRITESHIT("scripts");

		GETCOMPVEC(pt::LuaScript, scripts);
		//GETCOMPNUMUSED(pt::LuaScript, numComps);
		numComps = PrimtTech::ComponentHandler::GetNoOfUsedComponents<pt::LuaScript>();
		compTypeId = PrimtTech::ec_lua;
		WRITE(&, compTypeId, sizeof(PrimtTech::HasComponent));
		WRITE(&, numComps, sizeof(int));
		for (int i = 0; i < numComps; i++)
		{
			EntIdType u = scripts[i].EntId();
			WRITE(&, u, sizeof(EntIdType));

			char filename[64]{""};
			strcpy_s(filename, scripts[i].GetFileName().c_str());

			int maxStringSize = 64;
			WRITE(&, maxStringSize, sizeof(int));
			WRITE(,filename, maxStringSize);
		}

		////PrimtTech::ec_lua

		exporter.close();
		exporterAsci.close();
	}

	bool Import(std::string path, std::vector<pt::Entity>& entList)
	{
		std::ifstream importer(path, std::ios::binary | std::ios::in);
		if (!importer.is_open())
			return false;

		uint magicNum = 0;
		READ(&, magicNum, sizeof(uint));
		if (magicNum != 0x6969)
		{
			importer.close();
			return false;
		}
		ComponentHandler::ZeroFreeArray();
		uint numUsedEnts = pt::Entity::GetNoUsedEnts();
		for (int i = entList.size()-1; i > -1 ; i--)
		{
			entList[i].Free();
		}

		char pakPath[32] = "";
		READ(, pakPath, 32);

		//WRITESHIT("AssetNames");
		//WRITESHIT("MeshesNames");

		uint numMeshes = 0;
		READ(&, numMeshes, sizeof(uint));
		//WRITE(&, numMeshes, sizeof(uint));

		char nameOfAsset[32]{ "" };

		for (uint i = 0; i < numMeshes; i++)
		{
			READ(, nameOfAsset, 32);

			std::string path = "Assets/models/";
			path += nameOfAsset;
			ResourceHandler::AddMesh(path);
			//strcpy_s(nameOfAsset, ResourceHandler::GetMesh(i).GetName().c_str());
		}

		uint numMaterials = 0;
		READ(&, numMaterials, sizeof(uint));
		std::vector<Material>& mats = ResourceHandler::GetMaterialArrayReference();

		ResourceHandler::ResizeMaterials(0);

		//WRITESHIT("Material");

		for (uint i = 0; i < numMaterials; i++)
		{
			//strcpy_s(nameOfAsset, ResourceHandler::GetMaterial(i).GetFileName().c_str());
			READ(, nameOfAsset, 32);

			std::string matPat /*= "Assets/pmtrl/"*/;
			matPat += nameOfAsset;
			matPat += ".pmtrl";

			ResourceHandler::AddMaterial(matPat);

			mats[i].ImportMaterial(matPat);
		}

		int numEnts = 0;
		READ(&, numEnts, sizeof(int));

		pt::Entity::SetNoUsedEnts(0);
		pt::Entity::SetNoEnts(0);
		//entList.resize(1, true);
		entList.clear();
		entList.reserve(numEnts);

		for (int i = 0; i < numEnts; i++)
		{
			pt::Entity::Create();
		}
		

		// importing entity component tables
		for (int i = 0; i < numEnts; i++)
		{
			char entName[32]{ "" };
			READ(, entName, 32);
			int numComponents = 0;
			READ(&, numComponents, sizeof(int));

			entList[i].SetName(entName);

			for (int j = 0; j < numComponents; j++)
			{
				PrimtTech::HasComponent key;
				uint val = 0;
				READ(&, key, sizeof(PrimtTech::HasComponent));
				READ(&, val, sizeof(uint));
				entList[i].InsertTable(key, val);
			}
		}
		
		// ------------------------------------------- export components -------------------------------------------
		// ----------------------- export transforms -----------------------
		std::vector<pt::TransformComp>& transforms = ComponentHandler::GetComponentArray<pt::TransformComp>();
		int numComps = 0;
		PrimtTech::HasComponent compTypeId = PrimtTech::HasComponent(0);
		READ(&, compTypeId, sizeof(PrimtTech::HasComponent));
		THROW_POPUP_ERROR((compTypeId == PrimtTech::ec_transform), "IS NOT TRANSFORM");
		READ(&, numComps, sizeof(int));


		transforms.clear();
		transforms.reserve(numComps);
		int caps = transforms.capacity();
		ComponentHandler::SetFreeComponents(ec_meshRef, 0);
		for (int i = 0; i < numComps; i++)
		{
			EntIdType entId = 0;
			READ(&, entId, sizeof(EntIdType));
			transforms.emplace_back(entId);

			float f3[3]{ 0.f, 0.f, 0.f }; 
			READ(,f3, sizeof(float)*3);
			transforms[i].SetPosition(f3[0], f3[1], f3[2]);

			READ(,f3, sizeof(float) * 3);
			transforms[i].SetRotation(f3[0], f3[1], f3[2]);

			READ(,f3, sizeof(float) * 3);
			transforms[i].SetScale(f3[0], f3[1], f3[2]);
		}

		//// ----------------------- read meshref -----------------------

		std::vector<pt::MeshRef>& meshrefs = ComponentHandler::GetComponentArray<pt::MeshRef>();

		READ(&, compTypeId, sizeof(PrimtTech::HasComponent));
		THROW_POPUP_ERROR((compTypeId == PrimtTech::ec_meshRef), "IS NOT MESHREF");
		numComps = 0;
		READ(&, numComps, sizeof(int));
		meshrefs.clear();
		meshrefs.reserve(numComps);
		char meshname[64]{ "" };
		ComponentHandler::SetFreeComponents(ec_meshRef, 0);
		for (int i = 0; i < numComps; i++)
		{
			EntIdType entId = 0;
			READ(&, entId, sizeof(EntIdType));
			meshrefs.emplace_back(entId);
			meshrefs[i].freeComponent(entId);

			READ(,meshname, 64);
			meshrefs[i].Init(std::string(meshname));
			uint numMats = 0;
			READ(&, numMats, sizeof(uint));
			for (int j = 0; j < numMats; j++)
			{
				uint index = 0;
				READ(&, index, sizeof(uint));
				meshrefs[i].SetMaterial(index, j);
			}
		}

		// ----------------------- read cam -----------------------

		std::vector<pt::Camera>& cams = ComponentHandler::GetComponentArray<pt::Camera>();
		numComps = 0;
		READ(&, compTypeId, sizeof(PrimtTech::HasComponent));
		THROW_POPUP_ERROR((compTypeId == PrimtTech::ec_cam), "IS NOT CAM");
		READ(&, numComps, sizeof(int));
		cams.clear();
		cams.reserve(numComps);
		ComponentHandler::SetFreeComponents(ec_cam, 0);
		for (int i = 0; i < numComps; i++)
		{
			EntIdType entId = 0;
			READ(&, entId, sizeof(EntIdType));

			float fov = 80.f;
			READ(&, fov, sizeof(float));
			cams.emplace_back(entId).SetPerspective(fov, 16.f/9.f, 0.1f, 100.f);

			float f3[3]{ 0.f };
			READ(,f3, sizeof(float)*3);
			cams[i].SetPositionOffset(f3[0], f3[1], f3[2]);

			READ(,f3, sizeof(float)*3);
			cams[i].SetRotationOffset(f3[0], f3[1], f3[2]);

			// export view and projection
			const int MATRIXSIZE = sizeof(float) * 16;
			float mat[16]{};
			READ(&, mat, MATRIXSIZE);
			cams[i].SetViewMatrix(mat);

			READ(&, mat, MATRIXSIZE);
			cams[i].SetProjMatrix(mat);
		}

		//// ------------------ lights ----------------

		GETCOMPVEC(pt::Light, lights);
		numComps = 0;
		READ(&, compTypeId, sizeof(PrimtTech::HasComponent));
		THROW_POPUP_ERROR(compTypeId == PrimtTech::ec_light, "IS NOT LIGHTS");
		READ(&, numComps, sizeof(int));
		lights.clear();
		lights.reserve(numComps);
		ComponentHandler::SetFreeComponents(ec_light, 0);
		//int hh = ComponentHandler::GetNoOfUsedComponents<pt::Light>();
		for (int i = 0; i < numComps; i++)
		{
			EntIdType u = 0;
			READ(&, u, sizeof(EntIdType));
			lights.emplace_back(u);
			//lights[i].freeComponent(u);
			//ComponentHandler::IncreaseFreeComponents(ec_light, -1);
			//
			uchar uc = 0;
			READ(&, uc, sizeof(uchar));
			lights[i].SetType(uc);

			hlsl::Light lightData = {};
			READ(&, lightData, sizeof(hlsl::Light));
			lights[i].SetColor(lightData.clr);

			float f3[3]{ 0 };
			READ(,f3, sizeof(float) * 3);
			lights[i].SetOffsetPosition(sm::Vector4(f3[0], f3[1], f3[2], 1.f));

			READ(,f3, sizeof(float) * 3);
			lights[i].SetDirectionOffset(sm::Vector4(f3[0], f3[1], f3[2], 1.f));
		}
		//
		// ------------------ PhysBodies ------------------
		GETCOMPVEC(pt::PhysicsBody, pBodys);
		GETCOMPNUMUSED(pt::PhysicsBody, numComps);

		READ(&, compTypeId, sizeof(PrimtTech::HasComponent));
		THROW_POPUP_ERROR(compTypeId == PrimtTech::ec_rigidBodies, "IS NOT PHYSBOD");
		ComponentHandler::SetFreeComponents(ec_rigidBodies, 0);

		READ(&, numComps, sizeof(int));

		for (int i = 0; i < pBodys.size(); i++)
			pBodys[i].Delete();
		pBodys.clear();
		pBodys.reserve(numComps);

		for (int i = 0; i < numComps; i++)
		{
			EntIdType u = 0;
			READ(&, u, sizeof(EntIdType)); // read ent id
			pBodys.emplace_back(u);

			if (!pBodys[i].Exists())
				pBodys[i].CreateRigidBody(transforms[u]);


			rp::BodyType bodyType = {};
			READ(&, bodyType, sizeof(rp::BodyType));
			pBodys[i].SetType(bodyType);

			uint numColliders = 0;
			READ(&, numColliders, sizeof(uint));

			for (int j = 0; j < numColliders; j++)
			{
				pt::PhysicsBody::ePT_ShapeType colliderType;
				READ(&, colliderType, sizeof(pt::PhysicsBody::ePT_ShapeType));

				bool isTrigger = false;
				READ(&, isTrigger, sizeof(bool));

				sm::Vector3 vec3;
				int readSize = sizeof(float);

				switch (colliderType)
				{
				case pt::PhysicsBody::ePT_ShapeType::Box:
				{
					readSize = 3 * sizeof(float);
					READ(&, vec3.x, readSize);
					pBodys[i].AddBoxColider(vec3);
					break;
				}
				case pt::PhysicsBody::ePT_ShapeType::Sphere:
				{
					READ(&, vec3.x, readSize);
					pBodys[i].AddSphereColider(vec3.x);
					break;
				}
				case pt::PhysicsBody::ePT_ShapeType::Capsule:
				{
					readSize *= 2;
					READ(&, vec3.x, readSize);
					pBodys[i].AddCapsuleColider(vec3.x, vec3.y);
					break;
				}
				default:
					break;
				}
				pBodys[i].SetIsTrigger(isTrigger, j);
			}
		}

		//// ------------------ scripts ----------------

		GETCOMPVEC(pt::LuaScript, scripts);
		numComps = 0;

		READ(&, compTypeId, sizeof(PrimtTech::HasComponent));
		THROW_POPUP_ERROR((compTypeId == PrimtTech::ec_lua), "IS NOT SCRIPT");

		READ(&, numComps, sizeof(int));

		ComponentHandler::SetFreeComponents(ec_lua, 0);
		scripts.clear();
		scripts.reserve(numComps);
		for (int i = 0; i < numComps; i++)
		{
			EntIdType u = 0;
			READ(&, u, sizeof(EntIdType)); // read ent id
			scripts.emplace_back(u);

			char filename[64]{ "" };
			//strcpy_s(filename, scripts[i].GetFileName().c_str());

			int maxStringSize = 64;
			READ(&, maxStringSize, sizeof(int));
			READ(,filename, maxStringSize);

			std::string path = "Scripts/";
			path += filename;

			scripts[i].LoadScript(path.c_str());
			
		}

		//importer.close();
		return true;
	}
}