#include "Grid.h"

CellGrid::CellGrid(unsigned x, unsigned y, unsigned c):
	m_resolutions(x,y)
{
	mp_tileGrid = new uint8_t[x * y * c];

	m_output = new int[x * y];
	m_state = new int[x * y];

	memset(m_output, 0, x * y * sizeof(int));
	memset(m_state, 0, x * y * sizeof(int));

	//TextureMap::CreateCharFromFile("White32.png", mp_tileHp, x, y, c);

	//Simulate(x / 2, 0);

	//TextureMap::ExportCharToImage("testExport.png", mp_tileHp, x, y, 1);
}

CellGrid::~CellGrid()
{
	delete[] m_output;
	delete[] m_state;

	delete mp_tileHp;
	delete mp_tileGrid;
}

void CellGrid::Update(float dtime)
{
	auto cell = [&](int x, int y) { return m_output[y * m_gridWidth + x]; };

	for (int i = 0; i < m_gridWidth * m_gridHeight; i++)
	{
		m_output[i] = m_state[i];
	}

	for (int x = 1; x < m_gridWidth - 1; x++)
		for (int y = 1; y < m_gridHeight - 1; y++)
		{

		}
}

uint8_t*& CellGrid::GetChar()
{
	return mp_tileHp;
}

int CellGrid::FindWeakNeighbour(int x, int y) const
{
	int tile = Coord(x, y);
	int leftNeighbour = Coord(x + 1, y);

	if (Coord(x + 1, y) < tile)
		return 4;
	return 0;
}

int CellGrid::Coord(int x, int y) const
{
	return m_resolutions.x * y + x;
}

void CellGrid::Simulate(int x, int y)
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

void CellGrid::refreshTiles(unsigned x, unsigned y)
{
	m_cTile = Coord(x, y);
	m_lTile = m_cTile - 1;
	m_rTile = m_cTile + 1;
	m_bTile = Coord(x, y + 1);
}
