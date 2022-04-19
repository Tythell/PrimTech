#pragma once
#include <vector>
#include"../Math/Math.h"
#include "../Graphics/Texture.h"
#include"../Graphics/RenderCell.h"
#include"../Colors.h"

class CellGrid
{
public:
	CellGrid(unsigned x, unsigned y, unsigned z = 1);
	~CellGrid();

	void InitRenderCell(ID3D11Device*& device, ID3D11DeviceContext*& dc);
	void Update(float dtime);

	void SetCamP(Camera& c);
	uint8_t*& GetChar();
private:
	void GameOfLife(int x, int y);
	int& Cell(int x, int y);

	Camera* mp_cam;
	d::XMINT2 m_resolutions;

	int m_gridWidth, m_gridHeight;

	void refreshTiles(unsigned x, unsigned y);
	int m_cTile = 0;
	int m_rTile = 0;
	int m_lTile = 0;
	int m_bTile = 0;

	int Coord(int x, int y) const;
	uint8_t* mp_tileHp;
	uint8_t* mp_tileGrid;

	int* m_output;
	int* m_state;

	float m_timer = 0.f;
	const float TIMELIMIT;

	RenderCell m_cell;
};