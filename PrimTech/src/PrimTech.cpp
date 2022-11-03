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

		if (mp_gApi) delete mp_gApi;
	}

	void PrimTech::Init(LPCWSTR windowName, HINSTANCE hInstance, std::wstring windowClass, unsigned int width, unsigned int height)
	{
		m_window.init(windowName, hInstance, windowClass, width, height);

		m_window.SetInputP(m_kb);

		//m_cellCam.SetPosition(0.f, 0.f, -1.f);
		m_cams.Init({ (int)width, (int)height }, CamFlags_SAME_NAME_EQUALS_SAMECAM);
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


#define HandleMsg(m) memcpy((char*)&m, message, mainHeader->msgLen)
#define HandleMsgType(m, type) memcpy((char*)&m, message, sizeof(type))
	void PrimTech::Update(const float& dt)
	{
		if (m_kb.IsKeyDown(m_shutDownKey))
			m_window.ShutDown();


		while (consumerBuffer->Recieve(message, mainHeader))
		{
			switch (this->mainHeader->header)
			{
			case Headers::eCAMMESSAGE:
			{
				CameraData camMessage;
				HandleMsg(camMessage);
				Camera* pcam = m_cams.CreateEmptyCamera(camMessage.cameraName);
				sm::Matrix projMatrix = *reinterpret_cast<sm::Matrix*>(camMessage.projMatrix);
				sm::Matrix viewMatrix = *reinterpret_cast<sm::Matrix*>(camMessage.viewMatrix);
				pcam->OverrideProjectionMatrix(projMatrix);
				pcam->OverrideViewMatrix(viewMatrix);
				m_cams.SetCurrentCamera(camMessage.cameraName);

				break;
			}
			case Headers::eNAMECHANGE:
			{
				NameChange m;
				HandleMsg(m);
				int index = mp_gApi->NameFindModel(m.oldName);
				if (index == -1)
				{
					index = m_cams.GetCameraIndex(m.oldName);
					if (index == -1)
					{
						Material* pMat = ResourceHandler::GetMaterial(m.oldName);
						if (pMat)
							pMat->SetName(m.newName);
					}
					else
						m_cams.GetCameraAdress(index)->SetName(m.newName);
				}
				else
				{
					Model* pMod = mp_gApi->GetModelList()[index];
					pMod->SetName(m.newName);
				}
				//THROW_POPUP_ERRORF(index != -1, "Namechanging: mesh not found");

				break;
			}
			case Headers::eNEWMESH:
			{
				NewMeshMessageStruct m;
				HandleMsg(m);
				std::vector<Vertex3D> verts;
				std::vector<uint> indexes;
				if (true) // accept mesh from message
				{
					
					//newMeshMessage(message, m, verts, indexes);

					for (int i = 0; i < m.numVertices; i++)
					{
						MayaVertex mv;
						HandleMsgType(mv, MayaVertex);
						Vertex3D vert(mv);
						verts.emplace_back(vert);
					}

					for (int i = 0; i < m.numIndexes; i++)
					{
						uint index = 0;
						HandleMsgType(index, uint);
						indexes.emplace_back(index);
					}

					if (m.numIndexes <= 0) // If there is no index buffer
					{
						for (int i = 0; i < verts.size(); i++)
							indexes.emplace_back(i);
					}
				}
				else // make triangle
				{
					verts.resize(3);

					verts[0].position = { -.5f, -.5f, 0.f };
					verts[1].position = { 0.0f, .5f, 0.f };
					verts[2].position = { 0.5f, -.5f, 0.f };

					verts[0].texCoord = { 0.f, 0.f };
					verts[1].texCoord = { .5f, 1.f };
					verts[2].texCoord = { 1.f, 0.f };

					for (int i = 0; i < 3; i++)
						indexes.emplace_back(i);

					for (int i = 0; i < 3; i++)
						verts[i].normal = { 0.f, 0.f, 1.f };
					
				}
				
				mp_gApi->AddNewModel(m.meshName, verts, indexes);
				
				break;
			}
			case Headers::eNEWTOPOLOGY:
			{
				//std::vector<Vertex3D> verts;
				//std::vector<uint> indexes;
				//NewMeshMessageStruct m;
				//newMeshMessage(message, m, verts, indexes);

				//mp_gApi->AddNewModel(m.meshName, verts, indexes);
				break;
			}
			case Headers::eOBJECTDRAG:
			{
				MoveObjectStruct m;
				memcpy((char*)&m, message, mainHeader->msgLen);
				int index = mp_gApi->NameFindModel(m.meshName);
				THROW_POPUP_ERRORF((index != -1), "eOBJECTDRAG: mesh not found");
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
				THROW_POPUP_ERRORF((index != -1), "eVERTEXDRAG: mesh not found");
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
			case Headers::eDELETENODE:
			{
				DeleteMesh m;
				HandleMsg(m);
				
				for (int i = 0; i < mp_gApi->GetModelList().size(); i++)
				{
					if (mp_gApi->GetModelList()[i]->GetName() == std::string(m.meshName))
					{
						mp_gApi->DeleteModel(i);
						break;
					}
				}
				break;
			}
			case Headers::eMaterialCreated:
			{
				MaterialMessage m;
				HandleMsg(m);

				Material* pMat = ResourceHandler::AddMaterial(std::string(m.mtrlName), false);
				mp_gApi->GiveMtrlBuffer(pMat);
				pMat->SetAmbient({ m.ambient[0], m.ambient[1], m.ambient[2], 1.f});
				pMat->SetDiffuseClr(sm::Vector3(m.color));
				std::string diffuse = "." + std::string(m.diffuseName);
				if (diffuse != ".")
					pMat->LoadTexture(diffuse, TextureType::eDiffuse);

				std::string normal = "." + std::string(m.normalName);
				if (normal != ".")
					pMat->LoadTexture(diffuse, TextureType::eNormal);
				
				break;
			}
			case Headers::eMaterialConnected:
			{
				MtrlConnectionMsg m;
				HandleMsg(m);

				std::string mtrlName = m.mtrlName;
				std::string meshName = m.meshName;

				int modelIndex = mp_gApi->NameFindModel(meshName);
				
				if (modelIndex != -1)
					mp_gApi->GetModelList()[modelIndex]->AssignMaterial(mtrlName, false); // is false because we send invalid mtrl-names on purpose
				
				POPUP_ERRORF((m.meshName[0] != '-') && (modelIndex != -1), meshName + " does not exist");

				break;
			}
			default:
				POPUP_ERRORF(false, "There is no message for enum(" + std::to_string(mainHeader->header) + ")");
				break;
			}
			delete message;
		}

	}

	void PrimTech::HideCursor()
	{
		while (::ShowCursor(false) >= 0);
	}

	void PrimTech::ShowCursor()
	{
		while (::ShowCursor(true) < 0);
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