#include "Grid.h"

StoneGrid::StoneGrid(unsigned x, unsigned y, unsigned c):
	m_resolutions(x,y)
{
	mp_tileGrid = new uint8_t[x * y * c];

	TextureMap::CreateCharFromFile("White32.png", mp_tileHp, x, y, c);

	Simulate(x / 2, 0);

	TextureMap::ExportCharToImage("testExport.png", mp_tileHp, x, y, 1);
}

StoneGrid::~StoneGrid()
{
	delete mp_tileHp;
	delete mp_tileGrid;
}

uint8_t*& StoneGrid::GetChar()
{
	return mp_tileHp;
}

int StoneGrid::FindWeakNeighbour(int x, int y) const
{
	int tile = Coord(x, y);
	int leftNeighbour = Coord(x + 1, y);

	if (Coord(x + 1, y) < tile)
		return 4;
	return 0;
}

int StoneGrid::Coord(int x, int y) const
{
	return m_resolutions.x * y + x;
}

void StoneGrid::Simulate(int x, int y)
{
	refreshTiles(x, y);

	if (m_bTile > m_resolutions.x * m_resolutions.y)
		return;
	else
	{
		while (mp_tileHp[m_cTile] > 0)
		{
			mp_tileHp[m_cTile] -= 20;
		}

	}



}

void StoneGrid::refreshTiles(unsigned x, unsigned y)
{
	m_cTile = Coord(x, y);
	m_lTile = m_cTile - 1;
	m_rTile = m_cTile + 1;
	m_bTile = Coord(x, y + 1);
}
