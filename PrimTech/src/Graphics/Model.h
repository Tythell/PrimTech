#pragma once
#include "Transform.h"
#include"Buffer.h"
#include"Vertex.h"
#include "Material.h"

using Shape = std::vector<Vertex3D>;

bool LoadObjToBuffer(std::string path, Shape& shape, bool makeLeftHanded = true);

// Contains Vertex data
class Mesh
{
public:
	Mesh(std::string path, ID3D11Device*& device, bool makeLeftHanded = true);
	Buffer<Vertex3D>& GetVBuffer();
	std::string GetName() const;
	void IncreaseUses();
	void DecreaseUses();
	void ResetUses();
	int GetNrOfUses() const;
	d::BoundingSphere GetBSphere() const;
private:
	Buffer<Vertex3D> m_vbuffer;
	std::string m_name = "unloaded";
	int m_nrOfUses = 0;
	d::BoundingSphere m_bsphere;
};

enum class Sceneheaders
{
	enull, 
	eMODEL, 
};

enum class ModelType {eUNSPECIFIED, eDEBUG};

struct ModelStruct
{
	char modelname[24];
	char mtrlname[24];
	sm::Vector3 scale;
	sm::Vector3 rotation;
	sm::Vector3 position;
	ModelType type;
};

class Model : public Transform
{
public:
	void Init(const std::string path, ModelType e = ModelType(0), bool makeLeftHanded = true);
	void Draw();
	void UpdateTextureScroll(const float& deltatime);
	void LoadTexture(std::string path, TextureType type = eDiffuse);
	void SetLight(const sm::Vector4& v, const UINT& index);
	void SetMaterialBuffer(Buffer<hlsl::cbpMaterialBuffer>& cbMaterialBuffer);
	void DecreaseMeshUsage();

	Material& GetMaterial();
	Mesh* GetMeshP();
	std::string GetName() const;
	void SetDCandBuffer(ID3D11DeviceContext*& pdc, Buffer<hlsl::cbpWorldTransforms3D>& pCbuffer);
	ModelType GetModelType() const;
	d::BoundingSphere GetBSphere() const;
	const sm::Vector4 GetCharacterLight(int i) const;
private:
	std::string m_name = "";
	Mesh* mp_mesh = nullptr;
	Material m_material;
	sm::Vector4 m_characterLight[2]{ {0.f, 0.9f, 0.1f, 0.51f} };
	ModelType m_type = ModelType::eUNSPECIFIED;
	ID3D11DeviceContext* dc = nullptr;
	Buffer<hlsl::cbpWorldTransforms3D>* mp_cbTransformBuffer = nullptr;
};