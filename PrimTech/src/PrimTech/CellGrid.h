#pragma once
#include <vector>
#include"../Math/Math.h"
#include "../Graphics/Texture.h"
#include"../Graphics/RenderCell.h"
#include"../Colors.h"
#include "../Graphics/Noise/PerlinNoise.h"
#include<ctime>

struct sCell
{
	float hp = 0.f;
	int type = 0;
	d::XMINT2 velocity = {0,0};
	sm::Vector3 clr = {0,0,0};
};

class CellGrid
{
public:
	CellGrid(unsigned x, unsigned y, unsigned z = 1);
	~CellGrid();

	void InitRenderCell(ID3D11Device*& device, ID3D11DeviceContext*& dc);
	void Update();

	void SaveImage(const char* path);

	void SetCamP(Camera& c);
private:
	void GameOfLife(int x, int y);
	int& Cell(int x, int y);
	sCell& Tile(int x, int y);
	void FillSquare(int x1, int y1, int x2, int y2, int material);
	void SetTile(int x, int y, int material, float hp = 1.f);
	bool InBounds(int x, int y) const;

	void SimulateWater(int x, int y);
	void SimulateSand(int x, int y, sm::Vector3& clr);
	void SimulateStone(int x, int y, sm::Vector3& clr);
	void SimulatePerlinStone(int x, int y, sm::Vector3& clr);

	enum Materials { eAIR, eSTONE, eWATER, eSAND, eBEDROCK };

	Camera* mp_cam;
	d::XMINT2 m_resolutions;

	int m_gridWidth, m_gridHeight;

	int Coord(int x, int y) const;

	sCell* m_output;
	sCell* m_state;

	float m_timer = 0.f;
	const float TIMELIMIT;

	//RenderCell m_cell;
	const int NTILES;

	const float WATER_E_RATE;

	PerlinWikipedia m_pnoise;

	const int SECONDSEED;
	const int SEED;
	const int PERLININTENSITY;
};