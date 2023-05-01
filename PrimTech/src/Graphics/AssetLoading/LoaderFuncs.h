#pragma once
#include <string>
#include<vector>
#include "Vertex.h"

namespace PrimtTech
{
	struct Mtl
	{
		std::string name;
		std::string diffuseName; // map_Kd
	};

	struct Shape
	{
		std::vector<Vertex3D> verts;
		//UINT mtlIndex = 0;
		//Mtl material;
	};

	class FileLoader
	{
	public:
		static bool AssimpLoad(std::string path, std::vector<Shape>& shape, std::vector<Mtl>& allMtls);
	};

	
}