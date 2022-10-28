#pragma once
#include "Transform.h"
#include"Buffer.h"
#include"Vertex.h"
#include "Material.h"

//bool LoadObjToBuffer(std::string path, Shape& shape, bool makeLeftHanded = true);

struct Mtl
{
	std::string name;
	std::string diffuseName; // map_Kd
};

struct Shape
{
	std::vector<Vertex3D> verts;
	std::vector<uint> index;
	//UINT mtlIndex = 0;
	//Mtl material;
};

// Contains Vertex data
class Mesh
{
public:
	Mesh(std::string path, ID3D11Device*& device, char makeLeftHanded = 1);
	// ignores error checking
	Mesh(std::vector<Vertex3D> vArray, std::vector<uint> iArray, ID3D11Device*& device, ID3D11DeviceContext*& dc);
	Buffer<Vertex3D>& GetVBuffer();
	Buffer<uint>& GetIBuffer();
	std::string GetName() const;
	void IncreaseUses();
	void DecreaseUses();
	void ResetUses();
	void UpdateVertex(const uint& id, const Vertex3D& v);

	int GetNrOfUses() const;
	d::BoundingSphere GetBSphere() const;
	const UINT GetNofMeshes() const;
	std::vector<Mtl> GetMtl() const;
	std::vector<int> GetMtlIndex() const;
	std::vector<int> GetMeshOffsfets() const;
private:
	Buffer<Vertex3D> m_vbuffer;
	Buffer<uint> m_ibuffer;
	std::vector<Mtl> m_mtls;
	std::vector<int> m_mtlIndexes;
	std::vector<int> m_offsets;
	std::string m_name = "unloaded";
	int m_nrOfUses = 0;
	d::BoundingSphere m_bsphere;
	UINT m_nofMeshes = 0;
	Shape m_shape;
};

enum class Sceneheaders
{
	enull, 
	eMODEL, 
};

enum class ModelType {eUNSPECIFIED, eDEBUG, eMAYA};

struct ModelStruct
{
	char modelname[24];
	char mtrlname[24];
	sm::Vector3 scale;
	sm::Vector3 rotation;
	sm::Vector3 position;
	UINT noOfExtraMats = 0;
};

class Model : public Transform
{
public:
	~Model();
	void Init(const std::string path, ModelType e = ModelType(0), unsigned char makeLeftHanded = true);
	void CreateFromArray(std::vector<Vertex3D> vArray, std::vector<uint> iArray, ID3D11Device*& device, ID3D11DeviceContext*& dc);
	void ChangeVertex(const uint& id, const Vertex3D& v);
	void Draw();
	void UpdateTextureScroll(const float& deltatime);
	void LoadTexture(std::string path, UINT i = 0, TextureType type = eDiffuse);
	void SetLight(const sm::Vector4& v, const UINT& index);
	void SetMaterialBuffer(Buffer<hlsl::cbpMaterialBuffer>& cbMaterialBuffer);
	void DecreaseMeshUsage();
	void SetName(const std::string& n);
	void AssignMaterial(std::string mtrlName, bool checkError = true);
	void AssignMaterial(int index);
	//void CreateMaterial();
	//void AssignMaterial(Material& material);

	Material& GetMaterial(const UINT& i = 0);
	Mesh* GetMeshP();
	std::string GetName() const;
	void SetDCandBuffer(ID3D11DeviceContext*& pdc, Buffer<hlsl::cbpWorldTransforms3D>& pCbuffer);
	ModelType GetModelType() const;
	d::BoundingSphere GetBSphere() const;
	const sm::Vector4 GetCharacterLight(int i) const;
private:
	std::string m_name = "";
	Mesh* mp_mesh = nullptr;
	Material* mp_material = nullptr;
	sm::Vector4 m_characterLight[2]{ {0.f, 0.9f, 0.1f, 0.0f} };
	ModelType m_type = ModelType::eUNSPECIFIED;
	ID3D11DeviceContext* dc = nullptr;
	Buffer<hlsl::cbpWorldTransforms3D>* mp_cbTransformBuffer = nullptr;
	UINT m_nOfMats = 0;
};