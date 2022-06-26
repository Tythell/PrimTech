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
	void Init(const std::string path, ID3D11Device*& pDevice, ID3D11DeviceContext*& pDc, Buffer<hlsl::cbpWorldTransforms3D>& pCbuffer, bool makeLeftHanded = true);
	void Draw();
	void UpdateTextureScroll(const float& deltatime);
	void LoadDiffuse(const std::string path);
	void LoadDistortion(const std::string path);
	void setTextureScrollSpeed(float x, float y);
	void SetMaterialBuffer(Buffer<hlsl::cbpMaterialBuffer>& cbMaterialBuffer);
private:
	Mesh* mp_mesh = nullptr;
	Material m_material;
	ID3D11DeviceContext* dc = nullptr;
	ID3D11Device* device = nullptr;
	Buffer<hlsl::cbpWorldTransforms3D>* mp_cbTransformBuffer = nullptr;
};