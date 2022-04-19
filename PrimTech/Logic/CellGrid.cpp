#include "CellGrid.h"

CellGrid::CellGrid(unsigned x, unsigned y, unsigned z):
	m_resolutions(x,y), m_cell(x,y), m_gridWidth(x), m_gridHeight(y)
{
	int totalCells = x * y;
	m_output = new int[totalCells];
	m_state = new int[totalCells];

	memset(m_output, 0, totalCells * sizeof(int));
	memset(m_state, 0, totalCells * sizeof(int));

	for (int i = 0; i < m_gridWidth * m_gridHeight; i++)
		m_state[i] = rand() % 2;

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

void CellGrid::InitRenderCell(ID3D11Device*& device, ID3D11DeviceContext*& dc)
{
	m_cell.Init(device, dc);
}

void CellGrid::Update(float dtime)
{
	auto cell = [&](int x, int y) { return m_output[y * m_gridWidth + x]; };

	m_cell.SetViewProjM(mp_cam->GetProjM());

	for (int i = 0; i < m_gridWidth * m_gridHeight; i++)
	{
		m_output[i] = m_state[i];
	}

	for (int x = 1; x < m_gridWidth - 1; x++)
		for (int y = 1; y < m_gridHeight - 1; y++)
		{
			//if (cell(x, y) == 1 || cell(x,y) < m_gridWidth * m_gridHeight)
			//{
			//	m_state[Coord(x, y)] = 0;
			//	m_state[Coord(x, y + 1)] = 1;
			//}
			int nNeighbours = cell(x - 1, y - 1) + cell(x - 0, y - 1) + cell(x + 1, y - 1) +
				cell(x - 1, y + 0) + 0 + cell(x + 1, y + 0) +
				cell(x - 1, y + 1) + cell(x + 0, y + 1) + cell(x + 1, y + 1);

			if (cell(x, y) == 1)
				m_state[y * m_gridWidth + x] = nNeighbours == 2 || nNeighbours == 3;
			else
				m_state[y * m_gridWidth + x] = nNeighbours == 3;

			if (cell(x, y) == 1)
				m_cell.DrawCell(WHITE_3F, {(float)x,(float)y});

		}
}

void CellGrid::SetCamP(Camera& c)
{
	mp_cam = &c;
}

uint8_t*& CellGrid::GetChar()
{
	return mp_tileHp;
}

int CellGrid::Coord(int x, int y) const
{
	return m_resolutions.x * y + x;
}

void CellGrid::refreshTiles(unsigned x, unsigned y)
{
	m_cTile = Coord(x, y);
	m_lTile = m_cTile - 1;
	m_rTile = m_cTile + 1;
	m_bTile = Coord(x, y + 1);
}
