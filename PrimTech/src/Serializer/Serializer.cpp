
#include"pch.h"
#include "Serializer.h"

namespace PrimtTech
{

#define WRITE(content) exporter.write((const char*)content, sizeof(content))
#define WRITEA(content) exporter.write((const char*)&content, sizeof(content))
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
		std::vector<pt::Camera>& cams = ComponentHandler::GetComponentArray<pt::Camera>();
		numComps = cams.size();
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
			//exporter << f3[0] << " " << f3[1] << " " << f3[2] << " rotOffset" << std::endl;
		}
		exporter.close();
	}

	bool Import(std::string path, std::vector<pt::Entity>& entList)
	{
		std::ifstream importer(path, std::ios::binary);
		if (!importer.is_open())
			return false;

		for (int i = 1; i < entList.size(); i++)
		{
			entList[i].Free();
		}

		int magicNum = 0;
		READA(magicNum);
		if (magicNum != 0x6969)
		{
			importer.close();
			return false;
		}

		char pakPath[32] = "";
		READ(pakPath);

		int numEnts = 0;
		READA(numEnts);

		entList.resize(numEnts, true);
		pt::Entity::SetNoUsedEnts(numEnts);
		

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
		transforms.reserve(numComps);
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
			//transforms[i].SetScale(f3[0], f3[1], f3[2]);
		}

		std::vector<pt::MeshRef>& meshrefs = ComponentHandler::GetComponentArray<pt::MeshRef>();
		numComps = 0;
		READA(numComps);
		meshrefs.reserve(numComps);
		char meshname[64]{ "" };
		for (int i = 0; i < numComps; i++)
		{
			uint entId = 0;
			READA(entId);
			meshrefs[i].FreeComponent(entId);
			meshrefs.emplace_back(entId);

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
		cams.reserve(numComps);
		for (int i = 1; i < numComps; i++)
		{
			uint entId = 0;
			READA(entId);
			cams.emplace_back(entId);

			float fov = 80.f;
			READA(fov);

			float f3[3]{ 0.f };
			READ(f3);
			cams[i].SetPositionOffset(f3[0], f3[1], f3[2]);

			READ(f3);
			cams[i].SetRotationOffset(f3[0], f3[1], f3[2]);
		}


		importer.close();
		return true;
	}
}