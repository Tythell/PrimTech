#pragma once
#include <vector>
#include"../Math/Math.h"
#include "../Graphics/Texture.h"

class StoneGrid
{
public:
	StoneGrid(unsigned x, unsigned y, unsigned c = 1);
	~StoneGrid();


	uint8_t*& GetChar();
private:
	d::XMINT2 m_resolutions;
	int FindWeakNeighbour(int x, int y) const;

	void refreshTiles(unsigned x, unsigned y);
	int m_cTile = 0;
	int m_rTile = 0;
	int m_lTile = 0;
	int m_bTile = 0;

	int Coord(int x, int y) const;
	uint8_t* mp_tileHp;
	uint8_t* mp_tileGrid;
	void Simulate(int x, int y);
};