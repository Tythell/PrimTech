#pragma once
#include "Transform.h"
#include"Buffer.h"
#include"Vertex.h"
#include <string>
#include <d3d11.h>

class Model : public Transform
{
public:
	Model();
	void Init(const std::string path, ID3D11Device*& pDevice, ID3D11DeviceContext*& pDc, Buffer<hlsl::cbWorldTransforms3D>& pCbuffer);
	void Draw();

private:
	bool LoadObj(const std::string path, ID3D11Device*& pDevice);
	Buffer<Vertex3D> m_vbuffer;
	ID3D11DeviceContext* dc = nullptr;
	Buffer<hlsl::cbWorldTransforms3D>* mp_cbTransformBuffer;
};