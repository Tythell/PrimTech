#pragma once
#include "../../Utility/StringHelper.h"
#include "../Buffer.h"
#include "../Vertex.h"
#include "../Material.h"
namespace pt
{
	class MayaMesh
	{
	public:
		MayaMesh(std::vector<Vertex3D> vArray, ID3D11Device*& device, ID3D11DeviceContext*& dc);

		void RefreshMesh(std::vector<Vertex3D> vArray);

		void Draw();
	private:
		Material m_mat;
		std::string m_name = "";
		ID3D11DeviceContext* dc = nullptr;
		ID3D11Device* device = nullptr;
		Buffer<hlsl::cbpWorldTransforms3D>* mp_cbTransformBuffer = nullptr;
		Buffer<Vertex3D> m_vBuffer;

		UINT m_nOfMats = 0;
	};
}