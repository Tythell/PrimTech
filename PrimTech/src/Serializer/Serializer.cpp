
#include"pch.h"
#include "Serializer.h"

namespace PrimtTech
{

#define WRITE(content) exporter.write((const char*)content, sizeof(content))
#define WRITEA(content) exporter.write((const char*)&content, sizeof(content))

#define WRITE_S(content, size) exporter.write((const char*)&content, size)
#define READ_S(content, size) importer.read((char*)&content, size)
#define WRITE_asci(content) exporter << content << " " << #content << std::endl
#define WRITE_enter exporter << std::endl;
#define READA(content) importer.read((char*)&content, sizeof(content))
#define READ(content) importer.read((char*)content, sizeof(content))

	void Export_asci(std::string path, std::vector<pt::Entity>& entList)
	{
		std::ofstream exporter(path);
		if (!exporter.is_open())
		{
			return;
		}

		uint magicNum = 0x6969;
		//WRITE(magicNum); // Magic number
		WRITE_asci(magicNum);

		//const char pakPath[32] = "";
		//exporter << pakPath;

		int numEnts = entList.size();
		WRITE_asci(numEnts);

		// exporting entity component tables
		for (int entIndex = 0; entIndex < numEnts; entIndex++)
		{
			// number of components
			int numComponents = entList[entIndex].CalculateNrOfComponents();
			WRITE_asci(entIndex);
			WRITE_asci(numComponents);

			std::map<uint, uint> compTable = entList[entIndex].GetCompTable();
			for (auto const& [key, val] : compTable) // might be slow but works for now and probably ever
			{
				WRITE_asci(key);
				WRITE_asci(val);
			}
		}
		// ------------------------------------------- export components -------------------------------------------
		// export transforms
		std::vector<pt::TransformComp>& transforms = ComponentHandler::GetComponentArray<pt::TransformComp>();
		int numComps = transforms.size();
		WRITE_asci(numComps);
		for (int i = 0; i < transforms.size(); i++)
		{
			uint entId = transforms[i].EntId();
			WRITE_asci(entId);

			float f3[3]{0.f};
			sm::Vector3 vec = transforms[i].GetPosition();
			memcpy(f3, &vec.x, sizeof(float) * 3);
			//WRITE_asci(f3);
			exporter << f3[0] << " " << f3[1] << " "  << f3[2] << " pos" << std::endl;

			vec = transforms[i].GetRotation();
			memcpy(f3, &vec.x, sizeof(float) * 3);
			exporter << f3[0] << " " << f3[1] << " " << f3[2] << " rot" << std::endl;
			//WRITE_asci(f3);

			vec = transforms[i].GetScale();
			memcpy(f3, &vec.x, sizeof(float) * 3);
			exporter << f3[0] << " " << f3[1] << " " << f3[2] << " scale" << std::endl;
			//WRITE_asci(f3);
		}
		WRITE_enter;

		std::vector<pt::MeshRef>& meshrefs = ComponentHandler::GetComponentArray<pt::MeshRef>();
		numComps = meshrefs.size();
		WRITE_asci(numComps);
		char meshname[64]{ "" };
		for (int i = 0; i < meshrefs.size(); i++)
		{
			uint entId = meshrefs[i].EntId();
			WRITE_asci(entId);
			strcpy_s(meshname, meshrefs[i].GetNameOfMesh().c_str());
			WRITE_asci(meshname);
			uint numMats = meshrefs[i].GetNumMaterials();
			WRITE_asci(numMats);
			for (int j = 0; j < numMats; j++)
			{
				uint index = meshrefs[i].GetMaterialIndex(j);
				WRITE_asci(index);
			}
		}
		WRITE_enter;
		std::vector<pt::Camera>& cams = ComponentHandler::GetComponentArray<pt::Camera>();
		numComps = cams.size();
		WRITE_asci(numComps);
		for (int i = 1; i < numComps; i++)
		{
			uint entId = cams[i].EntId();
			WRITE_asci(entId);

			float fov = 80.f;
			WRITE_asci(fov);

			float f3[3]{ 0.f };
			sm::Vector3 vec = cams[i].GetPositionOffset();
			memcpy(f3, &vec.x, sizeof(float) * 3);
			exporter << f3[0] << " " << f3[1] << " " << f3[2] << " posoffset" << std::endl;

			vec = cams[i].GetRotationOffset();
			memcpy(f3, &vec.x, sizeof(float) * 3);
			exporter << f3[0] << " " << f3[1] << " " << f3[2] << " rotOffset" << std::endl;
		}
		exporter.close();
	}

	void Export(std::string path, std::vector<pt::Entity>& entList)
	{
		std::ofstream exporter(path);
		if (!exporter.is_open())
		{
			return;
		}

		uint magicNum = 0x6969;
		//WRITE(magicNum); // Magic number
		WRITEA(magicNum);

		const char pakPath[32] = "";
		WRITE(pakPath);

		int numEnts = entList.size();
		WRITEA(numEnts);

		// exporting entity component tables
		for (int i = 0; i < numEnts; i++)
		{
			// number of components
			int numComponents = entList[i].CalculateNrOfComponents();
			WRITEA(numComponents);

			std::map<uint, uint> compTable = entList[i].GetCompTable();
			for (auto const& [key, val] : compTable) // might be slow but works for now and probably ever
			{
				exporter.write((const char*)&key, sizeof(uint));
				exporter.write((const char*)&val, sizeof(uint));
			}
		}
		// ------------------------------------------- export components -------------------------------------------
		
		// ------------------ transforms ------------------
		std::vector<pt::TransformComp>& transforms = ComponentHandler::GetComponentArray<pt::TransformComp>();
		int numComps = transforms.size();
		WRITEA(numComps);
		for (int i = 1; i < transforms.size(); i++)
		{
			uint entId = transforms[i].EntId();
			WRITEA(entId);

			float f3[3]{ 0.f };
			sm::Vector3 vec = transforms[i].GetPosition();
			memcpy(f3, &vec.x, sizeof(float) * 3);
			WRITE(f3);

			vec = transforms[i].GetRotation();
			memcpy(f3, &vec.x, sizeof(float) * 3);
			WRITE(f3);

			vec = transforms[i].GetScale();
			memcpy(f3, &vec.x, sizeof(float) * 3);
			WRITE(f3);
		}
		// ------------------ meshrefs ------------------
		std::vector<pt::MeshRef>& meshrefs = ComponentHandler::GetComponentArray<pt::MeshRef>();
		numComps = meshrefs.size();
		WRITEA(numComps);
		char meshname[64]{ "" };
		for (int i = 0; i < meshrefs.size(); i++)
		{
			uint entId = meshrefs[i].EntId();
			WRITEA(entId);
			strcpy_s(meshname, meshrefs[i].GetNameOfMesh().c_str());
			WRITEA(meshname);
			uint numMats = meshrefs[i].GetNumMaterials();
			WRITEA(numMats);
			for (int j = 0; j < numMats; j++)
			{
				uint index = meshrefs[i].GetMaterialIndex(j);
				WRITEA(index);
			}
		}
		// ------------------ cams ------------------
		GETCOMPVEC(pt::Camera, cams);
		GETCOMPNUMUSED(pt::Camera, numComps);
		//numComps = cams.size();
		WRITEA(numComps);
		for (int i = 0; i < numComps; i++)
		{
			uint entId = cams[i].EntId();
			WRITEA(entId);

			float fov = 80.f;
			WRITEA(fov);

			float f3[3]{ 0.f };
			sm::Vector3 vec = cams[i].GetPositionOffset();
			memcpy(f3, &vec.x, sizeof(float) * 3);
			WRITE(f3);
			//exporter << f3[0] << " " << f3[1] << " " << f3[2] << " posoffset" << std::endl;

			vec = cams[i].GetRotationOffset();
			memcpy(f3, &vec.x, sizeof(float) * 3);
			WRITE(f3);

			// export view and projection
			const int MATRIXSIZE = sizeof(float) * 16;
			sm::Matrix mat = cams[i].GetViewMatrix();
			float f16[16]{};
			memcpy(f16, &mat._11, MATRIXSIZE);
			WRITE_S(f16, MATRIXSIZE);

			mat = cams[i].GetProjMatrix();
			memcpy(f16, &mat._11, MATRIXSIZE);
			WRITE_S(f16, MATRIXSIZE);
		}

		// ------------------ Lights ------------------
		GETCOMPVEC(pt::Light, lights);
		GETCOMPNUMUSED(pt::Light, numComps);
		WRITEA(numComps);
		for (int i = 0; i < numComps; i++)
		{
			EntIdType u = lights[i].EntId();
			WRITEA(u);
			uchar uc = lights[i].GetType();
			WRITEA(uc);

			hlsl::Light lightData = lights[i].GetLightData();
			WRITEA(lightData);

			sm::Vector4 v4 = lights[i].GetPositionOffset();
			float f3[3]{ 0 };
			memcpy(f3, &v4.x, sizeof(float) * 3);
			WRITE(f3);

			v4 = lights[i].GetDirectionOffset();
			memcpy(f3, &v4.x, sizeof(float) * 3);
			WRITE(f3);
		}

		// ------------------------------------ PhysBodies ------------------------------------
		GETCOMPVEC(pt::PhysicsBody, pBodys);
		GETCOMPNUMUSED(pt::PhysicsBody, numComps);
		WRITEA(numComps);
		for (int i = 0; i < numComps; i++)
		{
			EntIdType u = pBodys[i].EntId();
			WRITEA(u);
			rp::BodyType bodyType = pBodys[i].GetType();
			WRITEA(bodyType);

			uint numColliders = pBodys[i].GetNoColliders();
			WRITEA(numColliders);

			for (int j = 0; j < numColliders; j++)
			{
				pt::PhysicsBody::ePT_ShapeType colliderType = pBodys[i].GetColliderType(j);
				WRITEA(colliderType);
				bool isTrigger = pBodys[i].GetIsTrigger(j);
				WRITEA(isTrigger);

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
				WRITE_S(vec3.x, writeSize);
			}
		}

		// ------------------------------------ Scripts ------------------------------------
		GETCOMPVEC(pt::LuaScript, scripts);
		GETCOMPNUMUSED(pt::LuaScript, numComps);
		WRITEA(numComps);
		for (int i = 0; i < numComps; i++)
		{
			EntIdType u = scripts[i].EntId();
			WRITEA(u);

			char filename[64]{""};
			strcpy_s(filename, scripts[i].GetFileName().c_str());

			int maxStringSize = 64;
			WRITE_S(maxStringSize, sizeof(int));
			WRITE_S(filename, maxStringSize);
		}

		//PrimtTech::ec_lua

		exporter.close();
	}

	bool Import(std::string path, std::vector<pt::Entity>& entList)
	{
		std::ifstream importer(path, std::ios::binary);
		if (!importer.is_open())
			return false;

		uint numUsedEnts = pt::Entity::GetNoUsedEnts();

		uint magicNum = 0;
		READA(magicNum);
		if (magicNum != 0x6969)
		{
			importer.close();
			return false;
		}

		for (int i = 1; i < numUsedEnts; i++)
		{
			entList[i].Free();
		}

		char pakPath[32] = "";
		READ(pakPath);

		int numEnts = 0;
		READA(numEnts);

		pt::Entity::SetNoUsedEnts(numEnts-1);
		pt::Entity::SetNoEnts(0);
		//entList.resize(1, true);
		entList.clear();
		entList.resize(numEnts, true);

		for (int i = 0; i < numEnts; i++)
		{
			entList[i].SetEntId(i);
		}
		

		// importing entity component tables
		for (int i = 0; i < numEnts; i++)
		{
			int numComponents = 0;
			READA(numComponents);

			for (int j = 0; j < numComponents; j++) // might be slow but works for now and probably ever
			{
				uint key = 0, val = 0;
				READA(key);
				READA(val);
				entList[i].InsertTable(key, val);
			}
		}
		// ------------------------------------------- export components -------------------------------------------
		// export transforms
		std::vector<pt::TransformComp>& transforms = ComponentHandler::GetComponentArray<pt::TransformComp>();
		int numComps = 0;
		READA(numComps);


		transforms.resize(1, 0);
		transforms.reserve(numComps);
		int caps = transforms.capacity();
		//ComponentHandler::SetFreeComponents(ec_meshRef, 1);
		for (int i = 1; i < numComps; i++)
		{
			uint entId = 0;
			READA(entId);
			transforms.emplace_back(entId);

			float f3[3]{ 0.f }; READ(f3);
			transforms[i].SetPosition(f3[0], f3[1], f3[2]);

			READ(f3);
			transforms[i].SetRotation(f3[0], f3[1], f3[2]);

			READ(f3);
			transforms[i].SetScale(f3[0], f3[1], f3[2]);
		}

		std::vector<pt::MeshRef>& meshrefs = ComponentHandler::GetComponentArray<pt::MeshRef>();

		numComps = 0;
		READA(numComps);
		meshrefs.clear();
		meshrefs.reserve(numComps);
		char meshname[64]{ "" };
		//ComponentHandler::SetFreeComponents(ec_meshRef, 0);
		for (int i = 0; i < numComps; i++)
		{
			uint entId = 0;
			READA(entId);
			meshrefs.emplace_back(entId);
			meshrefs[i].freeComponent(entId);
			//ComponentHandler::IncreaseFreeComponents(ec_meshRef, -1);

			READ(meshname);
			meshrefs[i].Init(std::string(meshname));
			uint numMats = 0;
			READA(numMats);
			for (int j = 0; j < numMats; j++)
			{
				uint index = 0;
				READA(index);
				meshrefs[i].SetMaterial(index, j);
			}
		}

		std::vector<pt::Camera>& cams = ComponentHandler::GetComponentArray<pt::Camera>();
		numComps = 0;
		READA(numComps);
		cams.clear();
		cams.reserve(numComps);
		for (int i = 0; i < numComps; i++)
		{
			uint entId = 0;
			READA(entId);

			float fov = 80.f;
			READA(fov);
			cams.emplace_back(entId).SetPerspective(fov, 16.f/9.f, 0.1f, 100.f);

			float f3[3]{ 0.f };
			READ(f3);
			cams[i].SetPositionOffset(f3[0], f3[1], f3[2]);

			READ(f3);
			cams[i].SetRotationOffset(f3[0], f3[1], f3[2]);

			// export view and projection
			//sm::Matrix mat = cams[i].GetViewMatrix();
			//float* d = reinterpret_cast<float*>(&mat);
			const int MATRIXSIZE = sizeof(float) * 16;
			float mat[16]{};
			READ_S(mat, MATRIXSIZE);
			cams[i].SetViewMatrix(mat);

			READ_S(mat, MATRIXSIZE);
			cams[i].SetProjMatrix(mat);
		}

		// ------------------ lights ----------------

		GETCOMPVEC(pt::Light, lights);
		numComps = 0;
		READA(numComps);
		lights.clear();
		lights.reserve(numComps);
		//ComponentHandler::SetFreeComponents(ec_light, 0);
		int hh = ComponentHandler::GetNoOfUsedComponents<pt::Light>();
		for (int i = 0; i < numComps; i++)
		{
			EntIdType u = 0;
			READA(u); // read ent id
			lights.emplace_back(u);
			//lights[i].freeComponent(u);
			//ComponentHandler::IncreaseFreeComponents(ec_light, -1);
			ComponentHandler::ZeroFreeArray();
			uchar uc = 0;
			READA(uc);
			lights[i].SetType(uc);

			hlsl::Light lightData = {};
			READA(lightData);
			lights[i].SetColor(lightData.clr);

			float f3[3]{ 0 };
			READ(f3);
			lights[i].SetOffsetPosition(sm::Vector4(f3[0], f3[1], f3[2], 1.f));

			READ(f3);
			lights[i].SetDirectionOffset(sm::Vector4(f3[0], f3[1], f3[2], 1.f));
		}
		
		// ------------------ PhysBodies ------------------
		GETCOMPVEC(pt::PhysicsBody, pBodys);
		GETCOMPNUMUSED(pt::PhysicsBody, numComps);
		READA(numComps);

		for (int i = 0; i < pBodys.size(); i++)
			pBodys[i].Delete();
		pBodys.clear();
		pBodys.reserve(numComps);

		for (int i = 0; i < numComps; i++)
		{
			EntIdType u = 0;
			READA(u); // read ent id
			pBodys.emplace_back(u);

			if (!pBodys[i].Exists())
				pBodys[i].CreateRigidBody(transforms[u]);


			rp::BodyType bodyType = {};
			READA(bodyType);
			pBodys[i].SetType(bodyType);

			uint numColliders = 0;
			READA(numColliders);

			for (int j = 0; j < numColliders; j++)
			{
				pt::PhysicsBody::ePT_ShapeType colliderType;
				READA(colliderType);

				bool isTrigger = false;
				READA(isTrigger);

				sm::Vector3 vec3;
				int readSize = sizeof(float);

				switch (colliderType)
				{
				case pt::PhysicsBody::ePT_ShapeType::Box:
				{
					readSize *= 3;
					READ_S(vec3.x, readSize);
					pBodys[i].AddBoxColider(vec3);
					break;
				}
				case pt::PhysicsBody::ePT_ShapeType::Sphere:
				{
					READ_S(vec3.x, readSize);
					pBodys[i].AddSphereColider(vec3.x);
					break;
				}
				case pt::PhysicsBody::ePT_ShapeType::Capsule:
				{
					readSize *= 2;
					READ_S(vec3.x, readSize);
					pBodys[i].AddCapsuleColider(vec3.x, vec3.y);
					break;
				}
				default:
					break;
				}
				pBodys[i].SetIsTrigger(isTrigger, j);
				//WRITE_S(vec3.x, writeSize);
			}
		}

		// ------------------ scripts ----------------

		GETCOMPVEC(pt::LuaScript, scripts);
		numComps = 0;
		READA(numComps);
		scripts.clear();
		scripts.reserve(numComps);
		for (int i = 0; i < numComps; i++)
		{
			EntIdType u = 0;
			READA(u); // read ent id
			scripts.emplace_back(u);

			char filename[64]{ "" };
			//strcpy_s(filename, scripts[i].GetFileName().c_str());

			int maxStringSize = 64;
			READ_S(maxStringSize, sizeof(int));
			READ_S(filename, maxStringSize);

			scripts[i].LoadScript(filename);
			
		}

		importer.close();
		return true;
	}
}