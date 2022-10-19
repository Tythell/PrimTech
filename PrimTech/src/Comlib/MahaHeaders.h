#pragma once
typedef unsigned int uint;


enum Headers
{
	MESSAGE = 0,
	eNEWMESH,
	eOBJECTDRAG,
	eCAMERAMOVE, eCAMERACREATE,
	eVERTEXDRAG,
	eNEWTOPOLOGY,
	eLOADTEXTURE,
	eNAMECHANGE,
};

struct MayaVertex
{
	float position[3];
	float uv[2];
	float normal[3];
};

struct NewMeshMessageStruct
{
	char meshName[64];
	uint numVertices;
	MayaVertex* vertexArr;
};

struct NewMessage
{
	char meshName[64];
	uint vertexId;
	MayaVertex newVertex;
};

struct NewTexture
{
	char meshName[64];
	uint textureType;
	char texturePath[128];
};

struct NameChange
{
	char oldName[64];
	char newName[64];
};

struct MoveObjectStruct
{
	char meshName[64];
	float matrix[4 * 4];
	uint childCount;
};

struct CameraMove
{
	char cameraName[64];
	float matrix[4 * 4];
};

struct CameraCreate
{
	char cameraName[64];
	float matrix[4 * 4];
	float fov;
	float nearZ;
	float farZ;
};

