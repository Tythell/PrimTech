#pragma once
#include "Transform.h"
#include"Buffer.h"
#include"Vertex.h"
#include <string>
#include "Texture.h"

bool LoadObjToBuffer(std::string path, ID3D11Device*& pDevice, Buffer<Vertex3D>& vbuffer);

// Contains Vertex data
class Mesh
{
public:
	Mesh(std::string path, ID3D11Device*& device);
	Buffer<Vertex3D>& GetVBuffer();
	std::string GetName() const;
private:
	Buffer<Vertex3D> m_vbuffer;
	std::string m_name = "unloaded";
};

class ResourceHandler
{
public:
	static Mesh* AddMesh(std::string path, ID3D11Device*& pDevice);
	static Mesh& GetMesh(unsigned int index);
	static Mesh* GetMeshAdress(unsigned int index);
	static int CheckMeshNameExists(std::string meshName);
	static void Unload();
private:
	static std::vector<Mesh*> m_meshes;

};

// Contains mesh, textures and transformation data
class Model : public Transform
{
public:
	Model();
	void Init(const std::string path, ID3D11Device*& pDevice, ID3D11DeviceContext*& pDc, Buffer<hlsl::cbWorldTransforms3D>& pCbuffer);
	void Draw();
	void SetMesh(Mesh& mesh);
private:
	Mesh* mp_mesh;
	ID3D11DeviceContext* dc = nullptr;
	Buffer<hlsl::cbWorldTransforms3D>* mp_cbTransformBuffer = nullptr;
	TextureMap* mp_diffuse = nullptr;
};