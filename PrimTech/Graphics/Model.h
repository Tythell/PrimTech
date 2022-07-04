#pragma once
#include "Transform.h"
#include"Buffer.h"
#include"Vertex.h"
#include "Material.h"

bool LoadObjToBuffer(std::string path, ID3D11Device*& pDevice, Buffer<Vertex3D>& vbuffer, bool makeLeftHanded = true);

// Contains Vertex data
class Mesh
{
public:
	Mesh(std::string path, ID3D11Device*& device, bool makeLeftHanded = true);
	Buffer<Vertex3D>& GetVBuffer();
	std::string GetName() const;
private:
	Buffer<Vertex3D> m_vbuffer;
	std::string m_name = "unloaded";
};

// Contains mesh, textures and transformation data
class Model : public Transform
{
public:
	Model();
	//void Init(const std::string path, bool makeLeftHanded = true);
	void Init(const std::string path, ID3D11DeviceContext*& pDc, Buffer<hlsl::cbpWorldTransforms3D>& buffer, bool makeLeftHanded = true);
	void Draw();
	void UpdateTextureScroll(const float& deltatime);
	void LoadTexture(std::string path, TextureType type = eDiffuse);
	void setDiffuseScrollSpeed(float x, float y);
	void SetMaterialBuffer(Buffer<hlsl::cbpMaterialBuffer>& cbMaterialBuffer);
	Material& GetMaterial();

	void SetDCandBuffer(ID3D11DeviceContext*& pdc, Buffer<hlsl::cbpWorldTransforms3D>& pCbuffer);
private:
	ID3D11DeviceContext* dc;
	Buffer<hlsl::cbpWorldTransforms3D>* mp_cbTransformBuffer;
	Mesh* mp_mesh = nullptr;
	Material m_material;
};

class AllModels
{
public:
	static void Addbuffers(ID3D11DeviceContext*& dc, Buffer<hlsl::cbpWorldTransforms3D>& buffer);
	static void AddModelAdress(Model* d);
private:
	static std::vector<Model*> m_models;
};