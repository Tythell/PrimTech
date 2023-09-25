#include "pch.h"
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../3rdParty/stb_image.h"
#undef min
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "LoaderFuncs.h"

namespace PrimtTech
{
	float3 aiVecToSm(const aiVector3D aivec)
	{
		return float3(aivec.x, aivec.y, aivec.z);
	}
	float2 aiVecToSm(const aiVector2D aivec)
	{
		return float2(aivec.x, aivec.y);
	}
	Shape ProcessMesh(aiMesh* mesh, const aiScene* scene, Mtl& mtl)
	{
		Shape shape;
		UINT numVerts = mesh->mNumVertices;
		shape.verts.resize(numVerts);
		for (int i = 0; i < numVerts; i++)
		{
			shape.verts[i].position = aiVecToSm(mesh->mVertices[i]);
			shape.verts[i].normal = aiVecToSm(mesh->mNormals[i]);
			shape.verts[i].tangent = aiVecToSm(mesh->mTangents[i]);
			shape.verts[i].bitangent = aiVecToSm(mesh->mBitangents[i]);
			shape.verts[i].texCoord = float2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
		}
		UINT matIndex = mesh->mMaterialIndex;
		aiMaterial* material = scene->mMaterials[matIndex];
		aiString diffuseName;
		material->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), diffuseName);
		mtl.diffuseName = diffuseName.C_Str();
		// making vertexes clockwise because assimp is retarded
		for (int i = 0; i < numVerts / 3; i++)
		{
			PrimtTech::Vertex3D triangle[3];
			for (int j = 0; j < 3; j++)
				triangle[j] = shape.verts[i * 3 + j];
			std::swap(triangle[0], triangle[2]);

			for (int j = 0; j < 3; j++)
				shape.verts[i * 3 + j] = triangle[j];
		}
		return shape;
	}
	void ProcessNode(aiNode* node, const aiScene* scene, std::vector<Shape>& fullShape, std::vector<Mtl>& allMtls)
	{
		for (int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			Mtl mtl;
			fullShape.emplace_back(ProcessMesh(mesh, scene, mtl));
			allMtls.emplace_back(mtl);
		}
		for (int i = 0; i < node->mNumChildren; i++)
			ProcessNode(node->mChildren[i], scene, fullShape, allMtls);
	}
	bool FileLoader::AssimpLoad(std::string path, std::vector<Shape>& shape, std::vector<Mtl>& allMtls)
	{
		Assimp::Importer importer;
		const aiScene* pScene = importer.ReadFile(path.c_str(), aiProcess_MakeLeftHanded | aiProcess_CalcTangentSpace);
		std::string errString = path + " file does not exist";

		THROW_POPUP_ERROR(pScene != NULL, errString);

		ProcessNode(pScene->mRootNode, pScene, shape, allMtls);
		return true;
	}

	bool FileLoader::LoadObjToBuffer(std::string path, std::vector<Shape>& shape, std::vector<Mtl>& localMtls, std::vector<int>& matIndex, bool makeLeftHanded)
	{
		////shape.resize(1);
		//Shape localShape;
		//UINT nofMats = 0;
		//UINT currentMat = 0;

		////makeLeftHanded = false;

		//std::string dummy;
		//std::vector<float3> v;
		//std::vector<float3> vn;
		//std::vector<float2> vt;

		//std::vector<UINT> posIndex;
		//std::vector<UINT> normalIndex;
		//std::vector<UINT> texcoordsIndex;

		////std::vector<Mtl> mtls;

		//std::ifstream reader(path);

		//if (!reader.is_open())
		//	return false;
		//while (std::getline(reader, dummy))
		//{
		//	std::string input;
		//	//std::cout << s;
		//	reader >> input;
		//	if (input == "v")
		//	{
		//		DirectX::XMFLOAT3 vertex;
		//		reader >> vertex.x >> vertex.y >> vertex.z;
		//		if (makeLeftHanded) vertex.z *= -1;
		//		v.emplace_back(vertex);
		//	}
		//	else if (input == "vt")
		//	{
		//		DirectX::XMFLOAT2 uv;
		//		reader >> uv.x >> uv.y;
		//		vt.emplace_back(uv);
		//	}
		//	else if (input == "vn")
		//	{
		//		DirectX::XMFLOAT3 normal;
		//		reader >> normal.x >> normal.y >> normal.z;
		//		if (makeLeftHanded) normal.z *= -1;
		//		vn.emplace_back(normal);
		//	}
		//	else if (input == "f")
		//	{
		//		int vIndexTemp = -1;
		//		int vtIndexTemp = -1;
		//		int vnIndexTemp = -1;
		//		char slash;
		//		PrimtTech::Vertex3D triangle[3];
		//		for (int i = 0; i < 3; i++)
		//		{
		//			reader >> vIndexTemp >> slash >> vtIndexTemp >> slash >> vnIndexTemp;
		//			vIndexTemp--;
		//			vnIndexTemp--;
		//			vtIndexTemp--;
		//			triangle[i].position = v[vIndexTemp];
		//			triangle[i].normal = vn[vnIndexTemp];
		//			triangle[i].texCoord = vt[vtIndexTemp];
		//		}

		//		//if (makeLeftHanded)
		//		//	for (int i = 2; i > -1; i--)
		//		//		localShape.verts.emplace_back(triangle[i]);
		//		//else
		//		//	for (int i = 0; i < 3; i++)
		//		//		localShape.verts.emplace_back(triangle[i]);
		//		if (makeLeftHanded)
		//		{
		//			PrimtTech::Vertex3D temp = triangle[0];
		//			triangle[0] = triangle[2];
		//			triangle[2] = temp;
		//		}
		//		for (int i = 0; i < 3; i++)
		//			localShape.verts.emplace_back(triangle[i]);

		//	}
		//	else if (input == "g")
		//	{
		//		std::string sgname;
		//		reader >> sgname;
		//		if (!localShape.verts.empty())
		//		{
		//			shape.emplace_back(localShape);
		//			localShape.verts.clear();
		//		}
		//	}
		//	else if (input == "usemtl")
		//	{
		//		std::string sgname;
		//		reader >> sgname;

		//		if (!localShape.verts.empty())
		//		{
		//			shape.emplace_back(localShape);
		//			localShape.verts.clear();
		//		}

		//		for (int i = 0; i < localMtls.size(); i++)
		//		{
		//			if (localMtls[i].name == sgname)
		//				matIndex.emplace_back(i);
		//		}

		//	}
		//	else if (input == "mtllib")
		//	{
		//		std::string matname;
		//		reader >> matname;
		//		std::ifstream matreader("Assets/models/" + matname);
		//		if (matreader.is_open())
		//		{
		//			std::string sdummy;
		//			while (std::getline(matreader, sdummy))
		//			{
		//				if (sdummy[0] == 'n' && sdummy[1] == 'e')
		//				{
		//					Mtl mtl;
		//					nofMats++;
		//					input = sdummy.substr(7);
		//					mtl.name = input;
		//					//mtls.emplace_back(mtl);
		//					localMtls.resize(localMtls.size() + 1);
		//					localMtls[localMtls.size() - 1] = mtl;
		//				}
		//				matreader >> input;
		//				if (input == "newmtl")
		//				{
		//					Mtl mtl;
		//					nofMats++;
		//					matreader >> input;
		//					mtl.name = input;
		//					localMtls.resize(localMtls.size() + 1);
		//					localMtls[localMtls.size() - 1] = mtl;
		//					//mtls.emplace_back(mtl);
		//				}
		//				else if (input == "map_Kd")
		//				{
		//					matreader >> input;
		//					localMtls[nofMats - 1].diffuseName = input;
		//				}

		//			}
		//			matreader.close();
		//		}
		//	}

		//}
		//shape.emplace_back(localShape);
		//for (int si = 0; si < shape.size(); si++)
		//{
		//	for (int i = 0; i < shape[si].verts.size(); i += 3)
		//	{
		//		float2 UVA = shape[si].verts[i + 0].texCoord;
		//		float2 UVB = shape[si].verts[i + 1].texCoord;
		//		float2 UVC = shape[si].verts[i + 2].texCoord;

		//		float3 POSA = shape[si].verts[i + 0].position;
		//		float3 POSB = shape[si].verts[i + 1].position;
		//		float3 POSC = shape[si].verts[i + 2].position;

		//		float2 dAB = UVB - UVA;
		//		float2 dAC = UVC - UVA;

		//		float3 edge1 = POSB - POSA;
		//		float3 edge2 = POSC - POSA;

		//		float f = 1.0f / (dAB.x * dAC.y - dAC.x * dAB.y);

		//		float3 tangent;
		//		tangent.x = f * (dAC.y * edge1.x - dAB.y * edge2.x);
		//		tangent.y = f * (dAC.y * edge1.y - dAB.y * edge2.y);
		//		tangent.z = f * (dAC.y * edge1.z - dAB.y * edge2.z);

		//		shape[si].verts[i].tangent = tangent;
		//		shape[si].verts[i + 1].tangent = tangent;
		//		shape[si].verts[i + 2].tangent = tangent;
		//	}

		//	//if(!localMtls.empty())
		//	//	shape[si].material = localMtls[si];
		//}

		////localShape.clear();
		//reader.close();
		return true;
	}
	bool FileLoader::StbiCreateCharFromFile(const char* path, unsigned char*& imagedData, int& imagewidth, int& imageheight, int channels, bool flipUV)
	{
		stbi_set_flip_vertically_on_load(flipUV);

		imagedData = stbi_load(path, &imagewidth, &imageheight, nullptr, channels);
		POPUP_MESSAGE((imagedData), std::string(path) + " does not exist");

		return (imagedData != nullptr);
	}
	void FileLoader::StbiExportCharToImage(const char* path, unsigned char* imageData, int width, int height, int channels)
	{
		//stbi_set_flip_vertically_on_load(false);
		//std::string ext = StringHelper::GetExtension(std::string(path));
		//POPUP_MESSAGE((ext == "png"), "is not png, no image exported");
		//if (ext == "png")
		//	stbi_write_png(path, width, height, channels, imageData, channels * width);
		//else
		//	Popup::Error(path + std::string(" is not png, no image exported"));
	}
}