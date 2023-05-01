#include "pch.h"
#include "LoaderFuncs.h"
#undef min
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace PrimtTech
{
	sm::Vector3 aiVecToSm(const aiVector3D aivec)
	{
		return sm::Vector3(aivec.x, aivec.y, aivec.z);
	}
	sm::Vector2 aiVecToSm(const aiVector2D aivec)
	{
		return sm::Vector2(aivec.x, aivec.y);
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
			shape.verts[i].texCoord = sm::Vector2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
		}
		UINT matIndex = mesh->mMaterialIndex;
		aiMaterial* material = scene->mMaterials[matIndex];
		aiString diffuseName;
		material->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), diffuseName);
		mtl.diffuseName = diffuseName.C_Str();
		// making vertexes clockwise because assimp is retarded
		for (int i = 0; i < numVerts / 3; i++)
		{
			Vertex3D triangle[3];
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
}