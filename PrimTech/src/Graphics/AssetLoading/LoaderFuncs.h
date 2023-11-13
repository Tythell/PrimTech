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
		std::vector<PrimtTech::Vertex3D> verts;
		//UINT mtlIndex = 0;
		//Mtl material;
	};


	class FileLoader
	{
	public:
		static bool AssimpLoad(std::string path, std::vector<Shape>& shape, std::vector<Mtl>& allMtls, std::vector<std::string>& submeshNames);
		static bool LoadObjToBuffer(std::string path, std::vector<Shape>& shape, std::vector<Mtl>& localMtls, std::vector<int>& matIndex, bool makeLeftHanded);
		
		static bool StbiCreateCharFromFile(const char* path, unsigned char*& imagedData, int& imagewidth, int& imageheight, int channels, bool flipUV = true);
		static void StbiExportCharToImage(const char* path, unsigned char* imageData, int width, int height, int channels);
	};
}