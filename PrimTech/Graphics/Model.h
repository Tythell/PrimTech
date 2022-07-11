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
	void IncreaseUses();
	int GetNrOfUses() const;
private:
	Buffer<Vertex3D> m_vbuffer;
	std::string m_name = "unloaded";
	int m_nrOfUses = 0;
};

enum class ModelType {eUNSPECIFIED, eDEBUG};
class Model : public Transform
{
public:
	Model();
	void Init(const std::string path, ModelType e = ModelType(0), bool makeLeftHanded = true);
	void Draw();
	void UpdateTextureScroll(const float& deltatime);
	void LoadTexture(std::string path, TextureType type = eDiffuse);
	void SetMaterialBuffer(Buffer<hlsl::cbpMaterialBuffer>& cbMaterialBuffer);
	Material& GetMaterial();
	Mesh* GetMeshP();
	std::string GetName() const;
	void SetDCandBuffer(ID3D11DeviceContext*& pdc, Buffer<hlsl::cbpWorldTransforms3D>& pCbuffer);
	ModelType GetModelType() const;
private:
	ID3D11DeviceContext* dc = nullptr;
	Buffer<hlsl::cbpWorldTransforms3D>* mp_cbTransformBuffer = nullptr;
	Mesh* mp_mesh = nullptr;
	Material m_material;
	std::string m_name;
	ModelType m_type;
};

class AllModels
{
public:
	static void SetBuffers(ID3D11DeviceContext*& dc, Buffer<hlsl::cbpWorldTransforms3D>& buffer);
	static void AddModelAdress(Model* d);
	static void SetNamesToVector(std::vector<std::string>& v);
	static int GetNrOfModels();
	static Model* GetModel(int index);
private:
	static std::vector<Model*> m_models;
};