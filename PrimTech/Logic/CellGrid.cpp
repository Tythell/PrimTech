#include "CellGrid.h"
#include<vector>

CellGrid::CellGrid(unsigned x, unsigned y, unsigned z) :
	m_resolutions(x, y), m_cell(x, y), m_gridWidth(x), m_gridHeight(y), TIMELIMIT(.1f),
	NTILES(x * y)
{
	int totalCells = x * y;
	m_output = new sCell[totalCells];
	m_state = new sCell[totalCells];

	memset(m_output, 0, totalCells * sizeof(sCell));
	memset(m_state, 0, totalCells * sizeof(sCell));

	//for (int i = 0; i < m_gridWidth * m_gridHeight; i++)
		//m_state[i] = rand() % 2;

	FillSquare(0, 0, 1, m_gridHeight, eSTONE);
	FillSquare(m_gridWidth-2, 0, m_gridWidth-1, m_gridHeight, eSTONE);
	FillSquare(0, 1, m_gridWidth, 2, eSTONE);
	FillSquare(10, 40, 30, 60, eWATER);
	
	//SetTile(8, 31, eWATER);
	//SetTile(8, 32, eWATER);
	//SetTile(8, 33, eWATER);
	//for (int i = 30; i < 44; i+=1)
	//{
	//	SetTile(40, i, eWATER);
	//}
	FillSquare(1, 8, m_gridWidth -1, 30, eSTONE);
	//SetTile(24, 10, eAIR);
	//FillSquare(24, 25, 45, 33, eWATER);
	SetTile(24, 25, eWATER);

	srand((unsigned int)time(0));

	//TextureMap::CreateCharFromFile("White32.png", mp_tileHp, x, y, c);

	//Simulate(x / 2, 0);

	//TextureMap::ExportCharToImage("testExport.png", mp_tileHp, x, y, 1);
}

CellGrid::~CellGrid()
{
	delete[] m_output;
	delete[] m_state;
}

void CellGrid::InitRenderCell(ID3D11Device*& device, ID3D11DeviceContext*& dc)
{
	m_cell.Init(device, dc);
}

void CellGrid::Update(float dtime)
{
	auto cell = [&](int x, int y) { return m_state[y * m_gridWidth + x].type; };
	m_timer += dtime;

	m_cell.SetViewProjM(mp_cam->GetProjM());
	//SetTile(70, 50, eSAND);

	for (int i = 0; i < m_gridWidth * m_gridHeight; i++)
	{
		m_output[i] = m_state[i];
	}
	for (int y = 0; y < m_gridHeight - 1; y++)
		for (int x = 0; x < m_gridWidth - 1; x++)
		{
			int currentIdx = Coord(x, y);

			int material = cell(x, y);

			sCell tile = Tile(x, y);

			sm::Vector3 color;
			switch ((int)cell(x,y))
			{
			case eSTONE:
				SimulateStone(x, y, tile.clr);
				color = tile.clr;
				break;
			case eWATER:
				color = BLUE_3F;
				SimulateWater(x, y);
				break;
			case eSAND:
				color = ORANGE_3F;
				SimulateSand(x, y, tile.clr);
				break;
			}
			
			m_cell.DrawCell(color, { (float)x,(float)y });

			//GameOfLife(x, y);
		}
}

void CellGrid::SetCamP(Camera& c)
{
	mp_cam = &c;
}

void CellGrid::GameOfLife(int x, int y)
{
	int nNeighbours = Cell(x - 1, y - 1) + Cell(x - 0, y - 1) + Cell(x + 1, y - 1) +
		Cell(x - 1, y + 0) + 0 + Cell(x + 1, y + 0) +
		Cell(x - 1, y + 1) + Cell(x + 0, y + 1) + Cell(x + 1, y + 1);

	if (Cell(x, y) == 1)
		m_state[y * m_gridWidth + x].type = nNeighbours == 2 || nNeighbours == 3;
	else
		m_state[y * m_gridWidth + x].type = nNeighbours == 3;

	if (Cell(x, y) == 1)
		m_cell.DrawCell(WHITE_3F, { (float)x,(float)y });
}

int& CellGrid::Cell(int x, int y)
{
	return m_state[y * m_gridWidth + x].type;
}

sCell& CellGrid::Tile(int x, int y)
{
	return m_state[Coord(x, y)];
}

void CellGrid::FillSquare(int x1, int y1, int x2, int y2, int material)
{
	float hp = 0.f;
	sm::Vector3 clr;

	for (int x = x1; x < x2; x++)
		for (int y = y1; y < y2; y++)
		{
			switch (material)
			{
			case eAIR:
				break;
			case eSTONE:
				//hp = rand() % 6 + 1;
				//hp = Perlin::perlinn(x, y);
				clr = STONE_3F;
				break;
			case eWATER:
				clr = MAGENTA_3F;
				break;
			case eSAND:
				clr = ORANGE_3F;
				break;
			}
			m_state[y * m_gridWidth + x].type = (float)material;
			m_state[Coord(x, y)].hp = hp;
			m_state[Coord(x, y)].clr = clr;
		}

}

void CellGrid::SetTile(int x, int y, int material, float hp)
{
	int coord = Coord(x, y);
	m_state[coord].type = material;
	switch (material)
	{
	case eSTONE:
		m_state[coord].hp = rand() % 2 + 1;
		break;
	}
}

bool CellGrid::InBounds(int x, int y) const
{
	if (x < 0 || y < 0 || x > m_gridWidth - 1 || y > m_gridHeight - 1 )
	{
		return false;

	}
	return true;
}

void CellGrid::SimulateWater(int x, int y)
{
	//auto cell = [&](int x, int y) { return m_state[y * m_gridWidth + x]; };

	int r = (rand() % 2) ? 1 : -1;

	if (InBounds(x,y-1) && Cell(x, y - 1) == eAIR)
	{
		m_state[Coord(x, y)].type = eAIR;
		m_state[Coord(x, y -1)].type = eWATER;
	}
	else if (Cell(x + r, y - 1) == eAIR)
	{
		m_state[Coord(x, y)].type = eAIR;
		m_state[Coord(x + r, y - 1)].type = eWATER;
	}
	else if (Cell(x - r, y - 1) == eAIR)
	{
		m_state[Coord(x, y)].type = eAIR;
		m_state[Coord(x - r, y - 1)].type = eWATER;
	}
	else if (Cell(x + r, y - 0) == eAIR)
	{
		m_state[Coord(x, y)].type = eAIR;
		m_state[Coord(x + r, y)].type = eWATER;
	}
	else if (Cell(x + r, y - 0) == eAIR)
	{
		m_state[Coord(x, y)].type = eAIR;
		m_state[Coord(x + r, y)].type = eWATER;
	}
}

void CellGrid::SimulateSand(int x, int y, sm::Vector3 &clr)
{
	if (Cell(x, y - 1) == eAIR)
	{
		m_state[Coord(x, y)].type = eAIR;
		m_state[Coord(x, y - 1)].type = eSAND;
	}
	else if (Cell(x - 1, y - 1) == 0)
	{
		m_state[Coord(x, y)].type = eAIR;
		m_state[Coord(x - 1, y - 1)].type = eSAND;
	}
	else if (Cell(x + 1, y - 1) == 0)
	{
		m_state[Coord(x, y)].type = eAIR;
		m_state[Coord(x + 1, y - 1)].type = eSAND;
	}
}

void CellGrid::SimulateStone(int x, int y, sm::Vector3& clr)
{
	float erosionRate = 0.f;

	if (Cell(x, y + 1) == eWATER)
		erosionRate += WATER_E_RATE;
	if (Cell(x + 1, y) == eWATER)
		erosionRate += WATER_E_RATE / 4;
	if (Cell(x - 1, y) == eWATER)
		erosionRate += WATER_E_RATE / 4;

	m_state[Coord(x, y)].hp -= erosionRate;
	clr.x *= min(m_state[Coord(x, y)].hp, 1.f);
	clr.y *= min(m_state[Coord(x, y)].hp, 1.f);
	clr.z *= min(m_state[Coord(x, y)].hp, 1.f);

	if (m_state[Coord(x, y)].hp <= 0.f)
		SetTile(x, y, eAIR);
}

int CellGrid::Coord(int x, int y) const
{
	return y * m_gridWidth + x;
}
