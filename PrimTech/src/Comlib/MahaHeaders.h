#pragma once
typedef unsigned int uint;

enum Headers
{
	MESSAGE = 0,
	eNEWMESH,
	eOBJECTDRAG,
	eCAMMESSAGE,
	eVERTEXDRAG,
	eNEWTOPOLOGY,
	eMaterialConnected,
	eNAMECHANGE,
	eDELETEMODEL
};

struct SectionHeader
{
	Headers header;
	size_t msgLen;
	size_t msgID;
};

struct MayaVertex
{
	float position[3]{0.f};
	float uv[2]{0.f};
	float normal[3]{0.f};
};

struct NewMeshMessageStruct
{
	char meshName[64]{""};
	uint numVertices = 0;
	uint numIndexes = 0;
};

struct NewMessage
{
	char meshName[64]{ "" };
	uint vertexId = 0;
	MayaVertex newVertex;
};

struct DeleteMesh
{
	char meshName[64]{ "" };
};

struct NameChange
{
	char oldName[64]{ "" };
	char newName[64]{ "" };
};

struct MoveObjectStruct
{
	char meshName[64]{""};
	float matrix[4 * 4]{0.f};
	uint childCount = 0;
};

struct VertexDrag
{
	char meshName[64]{""};
	uint vertexId = 0;
	MayaVertex newVertex;
};

struct CameraData
{
	char cameraName[64]{""};
	float viewMatrix[4][4]{0.f};
	float projMatrix[4][4]{0.f};
	//float fov = 0.f;
	//float nearZ = 0.f;
	//float farZ = 0.f;
};

struct MaterialMessage
{
	char mtrlName[64]{ "" };
	float color[3];
	char diffuseName[128]{ "" };
	char normalName[128]{ "" };
};

