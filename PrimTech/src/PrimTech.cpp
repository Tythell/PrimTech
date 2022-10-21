#define _CRT_SECURE_NO_WARNINGS

#include "PrimTech.h"
#include "Graphics/DX11Wrapper.h"
#include <string>
namespace pt
{
	PrimTech::PrimTech() :
		m_playerSpeed(5.f)
	{
		consumerBuffer = new Comlib(L"Filemap", 150, Consumer);
		//m_cam3d.resize(1);
		//HideCursor();
	}

	PrimTech::~PrimTech()
	{
		delete consumerBuffer;
		if (mp_gApi) 
			delete mp_gApi;
	}

	void PrimTech::Init(LPCWSTR windowName, HINSTANCE hInstance, std::wstring windowClass, unsigned int width, unsigned int height)
	{
		m_window.init(windowName, hInstance, windowClass, width, height);

		

		m_window.SetInputP(m_kb);

		//m_cellCam.SetPosition(0.f, 0.f, -1.f);
		m_cams.Init({ (int)width, (int)height }, CamFlags_SAME_NAME_EQUALS_SAMECAM);
		m_cams.CreatePerspectiveCamera("testcam", 70, (float)width / (float)height, 0.1f, 100.f);
		//pCam2->SetPosition(2.f, 0.5f, -3.f);
		//m_cam3d[0].SetPosition(2.f, 0, -3.f);

		mp_gApi = new DX11Addon(m_window, m_cams);
		mp_gApi->SetInputP(m_kb);
	}

	void newMeshMessage(char* message, NewMeshMessageStruct& m, std::vector<Vertex3D>& verts, std::vector<uint>& iBuffer)
	{
		memcpy((char*)&m, message, sizeof(NewMeshMessageStruct));
		uint sizeOfVertexMessage = sizeof(MayaVertex) * m.numVertices;
		uint sizeOfIndexMessage = sizeof(uint) * m.numIndexes;
		std::vector<Vertex3D> vertexes;
		std::vector<uint> indexes;
		for (int i = 0; i < m.numVertices; i++)
		{
			Vertex3D vertex;
			memcpy((char*)&vertex,
				(message + sizeof(NewMeshMessageStruct)) + (i * sizeof(MayaVertex)),
				sizeof(MayaVertex));
			vertexes.emplace_back(vertex);
		}
		if (true) // if we want to recieve index bufffer
		{
			for (int i = 0; i < m.numVertices; i++)
			{
				uint index = 0;
				uint offset = sizeof(NewMeshMessageStruct);
				offset += sizeOfVertexMessage;
				offset += (i * sizeof(uint));
				memcpy((char*)&index, (message + offset), sizeof(uint));
				indexes.emplace_back(index);
			}
		}
		else
		{
			for (int i = 0; i < m.numVertices; i++)
				indexes.emplace_back(i);
		}
	}

	void PrimTech::Update(const float& dt)
	{
		if (m_kb.IsKeyDown(m_shutDownKey))
			m_window.ShutDown();


		while (consumerBuffer->Recieve(message, mainHeader))
		{

			if (mainHeader->header == MESSAGE)
			{
				OutputDebugStringW(L"We got a MESSAGE message.");
			}
			switch (this->mainHeader->header)
			{
			case Headers::MESSAGE:
			{
				OutputDebugStringA(this->message);
				OutputDebugStringA("\n");
				
				MessageHeader* pMessage = new MessageHeader;
				memcpy(pMessage,message, sizeof(MessageHeader));

				const size_t size = strlen(pMessage->message) + 1;
				wchar_t* recievedMsg = new wchar_t[size];
				mbstowcs(recievedMsg, pMessage->message, size);
				
				sm::Vector3 p(pMessage->position[0], pMessage->position[1], pMessage->position[2]);
				std::wstring pos = L"\nPosition " + std::to_wstring((int)pMessage->position[0]) + L" "
					+ std::to_wstring((int)pMessage->message[1]) + L" "
					+std::to_wstring((int)pMessage->message[2]);

				OutputDebugStringW(L"\n\n");
				OutputDebugStringW(recievedMsg);
				OutputDebugStringW(pos.c_str());

				delete pMessage;
				message = nullptr;
				break;
			}
			case Headers::eCAMMESSAGE:
			{
				CameraData camMessage;
				memcpy((char*)&camMessage, message, mainHeader->msgLen);
				Camera* pcam = m_cams.CreateEmptyCamera(camMessage.cameraName);
				sm::Matrix projMatrix = *reinterpret_cast<sm::Matrix*>(camMessage.projMatrix);
				sm::Matrix viewMatrix = *reinterpret_cast<sm::Matrix*>(camMessage.viewMatrix);
				pcam->OverrideProjectionMatrix(projMatrix);
				pcam->OverrideViewMatrix(viewMatrix);

				break;
			}
			case Headers::eLOADTEXTURE:
			{
				NewTexture m;
				memcpy((char*)&m, message, mainHeader->msgLen);
				int index = mp_gApi->NameFindModel(m.meshName);
				THROW_POPUP_ERRORF(index != -1, "Namechanging: mesh not found");
				Model* pModel = mp_gApi->GetModelList()[index];
				std::string texturePath = ".";
				texturePath.append(m.texturePath);
				if (m.textureType == 1)
					m.textureType = 2; // NormalMap is texturetype 2 in this engine
				pModel->LoadTexture(texturePath, 0, TextureType(m.textureType));

				break;
			}
			case Headers::eNAMECHANGE:
			{
				NameChange m;
				memcpy((char*)&m, message, mainHeader->msgLen);
				int index = mp_gApi->NameFindModel(m.oldName);
				THROW_POPUP_ERRORF(index != -1, "Namechanging: mesh not found");

				mp_gApi->GetModelList()[index]->SetName(m.newName);

				break;
			}
			case Headers::eNEWMESH:
			{
				std::vector<Vertex3D> verts;
				std::vector<uint> indexes;
				NewMeshMessageStruct m;
				newMeshMessage(message, m, verts, indexes);

				mp_gApi->AddNewModel(m.meshName, verts, indexes);
				break;
			}
			case Headers::eNEWTOPOLOGY:
			{
				std::vector<Vertex3D> verts;
				std::vector<uint> indexes;
				NewMeshMessageStruct m;
				newMeshMessage(message, m, verts, indexes);

				mp_gApi->AddNewModel(m.meshName, verts, indexes);
				break;
			}
			case Headers::eOBJECTDRAG:
			{
				MoveObjectStruct m;
				memcpy((char*)&m, message, mainHeader->msgLen);
				int index = mp_gApi->NameFindModel(m.meshName);
				THROW_POPUP_ERRORF(index != -1, "eOBJECTDRAG: mesh not found");
				Model* pModel = mp_gApi->GetModelList()[index];
				
				sm::Matrix mat = *reinterpret_cast<sm::Matrix*>(m.matrix);

				pModel->OverrideWorldMatrix(mat);

				break;
			}
			case Headers::eVERTEXDRAG:
			{
				VertexDrag m;
				memcpy((char*)&m, message, mainHeader->msgLen);
				int index = mp_gApi->NameFindModel(m.meshName);
				THROW_POPUP_ERRORF(index != -1, "eVERTEXDRAG: mesh not found");
				Model* pModel = mp_gApi->GetModelList()[index];


				Vertex3D ptVert;
				ptVert.position.x = m.newVertex.position[0];
				ptVert.position.y = m.newVertex.position[1];
				ptVert.position.z = m.newVertex.position[2];
				ptVert.texCoord.x = m.newVertex.uv[0];
				ptVert.texCoord.y = m.newVertex.uv[1];
				ptVert.normal.x = m.newVertex.normal[0];
				ptVert.normal.y = m.newVertex.normal[1];
				ptVert.normal.z = m.newVertex.normal[2];
				pModel->ChangeVertex(m.vertexId, ptVert);
				break;
			}
			default:
				POPUP_ERRORF(false, "There is no message for enum(" + std::to_string(mainHeader->header) + ")");
				break;
			}
		}
		bool recievedSuccess = consumerBuffer->Recieve(message, mainHeader);
		if (recievedSuccess)
		{
			switch (mainHeader->header)
			{
			case Headers::MESSAGE:
			{
				OutputDebugStringA(message);
				OutputDebugStringA("\n");
				break;
			}
			case Headers::eCAMMESSAGE:
			{
				CameraData camMessage;
				memcpy((char*)&camMessage, message, mainHeader->msgLen);
				Camera* pcam = m_cams.CreateEmptyCamera(camMessage.cameraName);
				sm::Matrix projMatrix = *reinterpret_cast<sm::Matrix*>(camMessage.projMatrix);
				sm::Matrix viewMatrix = *reinterpret_cast<sm::Matrix*>(camMessage.viewMatrix);
				pcam->OverrideProjectionMatrix(projMatrix);
				pcam->OverrideViewMatrix(viewMatrix);

				break;
			}
			case Headers::eLOADTEXTURE:
			{
				NewTexture m;
				memcpy((char*)&m, message, mainHeader->msgLen);
				int index = mp_gApi->NameFindModel(m.meshName);
				THROW_POPUP_ERRORF(index != -1, "Namechanging: mesh not found");
				Model* pModel = mp_gApi->GetModelList()[index];
				std::string texturePath = ".";
				texturePath.append(m.texturePath);
				if (m.textureType == 1)
					m.textureType = 2; // NormalMap is texturetype 2 in this engine
				pModel->LoadTexture(texturePath, 0, TextureType(m.textureType));

				break;
			}
			case Headers::eNAMECHANGE:
			{
				NameChange m;
				memcpy((char*)&m, message, mainHeader->msgLen);
				int index = mp_gApi->NameFindModel(m.oldName);
				THROW_POPUP_ERRORF(index != -1, "Namechanging: mesh not found");

				mp_gApi->GetModelList()[index]->SetName(m.newName);

				break;
			}
			case Headers::eNEWMESH:
			{
				std::vector<Vertex3D> verts;
				std::vector<uint> indexes;
				NewMeshMessageStruct m;
				newMeshMessage(message, m, verts, indexes);

				mp_gApi->AddNewModel(m.meshName, verts, indexes);
				break;
			}
			case Headers::eNEWTOPOLOGY:
			{
				std::vector<Vertex3D> verts;
				std::vector<uint> indexes;
				NewMeshMessageStruct m;
				newMeshMessage(message, m, verts, indexes);

				mp_gApi->AddNewModel(m.meshName, verts, indexes);
				break;
			}
			case Headers::eOBJECTDRAG:
			{
				MoveObjectStruct m;
				memcpy((char*)&m, message, mainHeader->msgLen);
				int index = mp_gApi->NameFindModel(m.meshName);
				THROW_POPUP_ERRORF(index != -1, "eOBJECTDRAG: mesh not found");
				Model* pModel = mp_gApi->GetModelList()[index];
				
				sm::Matrix mat = *reinterpret_cast<sm::Matrix*>(m.matrix);

				pModel->OverrideWorldMatrix(mat);

				break;
			}
			case Headers::eVERTEXDRAG:
			{
				VertexDrag m;
				memcpy((char*)&m, message, mainHeader->msgLen);
				int index = mp_gApi->NameFindModel(m.meshName);
				THROW_POPUP_ERRORF(index != -1, "eVERTEXDRAG: mesh not found");
				Model* pModel = mp_gApi->GetModelList()[index];


				Vertex3D ptVert;
				ptVert.position.x = m.newVertex.position[0];
				ptVert.position.y = m.newVertex.position[1];
				ptVert.position.z = m.newVertex.position[2];
				ptVert.texCoord.x = m.newVertex.uv[0];
				ptVert.texCoord.y = m.newVertex.uv[1];
				ptVert.normal.x = m.newVertex.normal[0];
				ptVert.normal.y = m.newVertex.normal[1];
				ptVert.normal.z = m.newVertex.normal[2];
				pModel->ChangeVertex(m.vertexId, ptVert);
				break;
			}
			default:
				POPUP_ERRORF(false, "There is no message for enum(" + std::to_string(mainHeader->header) + ")");
				break;
			}
		}
		
}

	void PrimTech::HideCursor()
	{
		while(::ShowCursor(false) >= 0);
	}

	void PrimTech::ShowCursor()
	{
		while(::ShowCursor(true) < 0);
	}

	void PrimTech::Run()
	{
		double start = 0, deltatime = 0;

		bool running = true;
		while (running)
		{
			start = omp_get_wtime();
			Update((float)deltatime);
			mp_gApi->CalculateFps((float)deltatime);
			mp_gApi->UpdateScene((float)deltatime);
			mp_gApi->Render((float)deltatime);
			deltatime = omp_get_wtime() - start;

			running = m_window.processMsg();
		}

	}
}

