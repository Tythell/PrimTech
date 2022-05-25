#include "CellGrid.h"
#include<vector>

CellGrid::CellGrid(unsigned x, unsigned y, unsigned z) :
	m_resolutions(x, y), m_cell(x, y), m_gridWidth(x), m_gridHeight(y), TIMELIMIT(.1f),
	NTILES(x* y), SEED(rand()), SECONDSEED(rand()), PERLININTENSITY(50.f), WATER_E_RATE(0.2f)
{
	int totalCells = x * y;
	m_output = new sCell[totalCells];
	m_state = new sCell[totalCells];

	memset(m_output, 0, totalCells * sizeof(sCell));
	memset(m_state, 0, totalCells * sizeof(sCell));

	//FillSquare(0, 0, 1, m_gridHeight, eSTONE);
	//FillSquare(m_gridWidth-2, 0, m_gridWidth-1, m_gridHeight, eSTONE);
	//FillSquare(0, 1, m_gridWidth, 2, eSTONE);
	//FillSquare(1, m_gridHeight - 2, m_gridWidth - 1, m_gridHeight - 1, eWATER);
	

	//const int n = 5;
	//for (int i = 1; i < n + 1; i++)
	//{
	//	int xCoord = m_gridWidth / (n+1);

	//	for (int j = 0; j < 1; j++)
	//	{
	//		int tile = j + (xCoord * i);
	//		SetTile(tile, m_gridHeight - 2, eWATER);
	//	}
	//	
	//}

	FillSquare(0, 0, m_gridWidth - 1, m_gridHeight - 2, eSTONE);
	FillSquare(m_gridWidth - 10, (m_gridHeight / 2) + 20, m_gridWidth - 1, (m_gridHeight / 2) + 24, eWATER);
	FillSquare(0, m_gridHeight-2, m_gridWidth, m_gridHeight, eBEDROCK);

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

void CellGrid::Update()
{
	auto cell = [&](int x, int y) { return m_state[y * m_gridWidth + x].type; };

	m_cell.SetViewProjM(mp_cam->GetProjM());
	//SetTile(70, 50, eSAND);

	int xLoopStart = 0;
	int xLoopEnd = m_gridWidth - 1;

	//if (rand() % 2)
	//{
	//	xLoopStart = m_gridWidth;
	//	xLoopEnd = 0;
	//}

	//for (int i = 0; i < m_gridWidth * m_gridHeight; i++)
	//{
	//	m_output[i] = m_state[i];
	//}
	for (int y = 0; y < m_gridHeight - 1; y++)
		for (int x = xLoopStart + 0; x < xLoopEnd + 1; x++)
		{
			int currentIdx = Coord(x, y);

			int material = cell(x, y);

			sCell tile = Tile(x, y);

			sm::Vector3 color;
			switch ((int)cell(x, y))
			{
			case eSTONE:
				SimulateStone(x, y, tile.clr);
				color = tile.clr;
				break;
			case eBEDROCK:
				SimulateStone(x, y, tile.clr);
				color = STONE_3F;
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
			//if(cell(x,y) != eAIR)
			m_cell.DrawCell(color, { (float)x,(float)y });

			//GameOfLife(x, y);
		}
}

void CellGrid::SaveImage(const char* path)
{
	const int dataLen = m_gridWidth * m_gridHeight;
	uint8_t* data = new uint8_t[dataLen];

	int stoneColor = int(STONE_1F * 255.f);
	for (int x = 0; x < m_gridWidth; x++)
		for (int y = 0; y < m_gridHeight; y++)
		{
			data[Coord(x, m_gridHeight - y - 1)] = 
				(m_state[Coord(x, y)].type == eSTONE || m_state[Coord(x, y)].type == eBEDROCK) ? 
				stoneColor : 0.f;
		}

	TextureMap::ExportCharToImage(path, data, m_gridWidth, m_gridHeight, 1);

	delete data;
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
			SetTile(x, y, material);
		}

}

void CellGrid::SetTile(int x, int y, int material, float hp)
{
	int coord = Coord(x, y);

	//float hp = 0.f;
	sm::Vector3 clr;

	switch (material)
	{
	case eAIR:
		break;
	case eSTONE:
	{
		float noise = abs(m_pnoise.Perlin2D(x + SEED, y + SEED, .09f));



		hp = noise * 10;
		//hp = pow(hp, 2);
		if (hp < .4f)
			hp = .7f;
		hp *= (float)(rand() % 1000) / 1000.f;
		hp *= 5;
		//hp = RandomNum(0.f, 8.f, 2);
		//hp = abs(m_pnoise.Perlin2D(x, y, .06f)) + .4f;// worlay???
		//hp *= 1.5f;
		//hp = pow(hp, 100);
		//if (hp >= 1.f)
		//	hp = 1.f;

		//hp = 1.f - hp;
		//hp = (m_pnoise.Perlin2D(x + SEED, y + SEED, .1f) + 1.f) * PERLININTENSITY;
		/*hp /= 2.f;*/
		clr = STONE_3F;
		break;
	}

	case eWATER:
		clr = MAGENTA_3F;
		break;
	case eSAND:
		clr = ORANGE_3F;
		break;

	case eBEDROCK:
		clr = STONE_3F;
		hp = FLT_MAX;
		break;
	}
	m_state[coord].type = material;
	m_state[coord].hp = hp;
	m_state[coord].clr = clr;
}

bool CellGrid::InBounds(int x, int y) const
{
	if (x < 0 || y < 0 || x > m_gridWidth - 1 || y > m_gridHeight - 1)
	{
		return false;

	}
	return true;
}

void CellGrid::SimulateWater(int x, int y)
{
	//auto cell = [&](int x, int y) { return m_state[y * m_gridWidth + x]; };

	if (x == 0)
		m_state[Coord(x, y)].type = eAIR;

	int r = (rand() % 2) ? -1 : 1;

	//right
	//if (InBounds(x, y - 1) && Cell(x, y - 1) == eAIR)
	//{
	//	m_state[Coord(x, y)].type = eAIR;
	//	m_state[Coord(x, y - 1)].type = eWATER;
	//}
	//else if (InBounds(x + r, y - 1) && Cell(x + r, y - 1) == eAIR)
	//{
	//	m_state[Coord(x, y)].type = eAIR;
	//	m_state[Coord(x + r, y - 1)].type = eWATER;
	//}
	//else
	//{
	//	if (InBounds(x - r, y - 0) && Cell(x - r, y - 0) == eAIR)
	//	{
	//		m_state[Coord(x, y)].type = eAIR;
	//		m_state[Coord(x - r, y - 0)].type = eWATER;
	//	}
	//	else if (InBounds(x + r, y - 0) && Cell(x + r, y - 0) == eAIR)
	//	{
	//		m_state[Coord(x, y)].type = eAIR;
	//		m_state[Coord(x + r, y)].type = eWATER;
	//	}
	//}

	//left
	if (InBounds(x - 1, y) && Cell(x - 1, y) == eAIR)
	{
		m_state[Coord(x, y)].type = eAIR;
		m_state[Coord(x - 1, y)].type = eWATER;
	}
	else if (InBounds(x - 1, y - r) && Cell(x - 1, y - r) == eAIR)
	{
		m_state[Coord(x, y)].type = eAIR;
		m_state[Coord(x - 1, y - r)].type = eWATER;
	}
	else if (InBounds(x - 1, y + r) && Cell(x - 1, y + r) == eAIR)
	{
		m_state[Coord(x, y)].type = eAIR;
		m_state[Coord(x - 1, y + r)].type = eWATER;
	}
	else
	{
		if (InBounds(x - 0, y + r) && Cell(x - 0, y + r) == eAIR)
		{
			m_state[Coord(x, y)].type = eAIR;
			m_state[Coord(x - 0, y + r)].type = eWATER;
		}
		else if (InBounds(x + 0, y - r) && Cell(x + 0, y - r) == eAIR)
		{
			m_state[Coord(x, y)].type = eAIR;
			m_state[Coord(x + 0, y - r)].type = eWATER;
		}
	}

}

void CellGrid::SimulateSand(int x, int y, sm::Vector3& clr)
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
	int r = (rand() % 2) ? 1 : -1;

	if (InBounds(x+1,y) && Cell(x + 1, y + 0) == eWATER)
		erosionRate += WATER_E_RATE * 1.0f;
	if (InBounds(x + 0, y + r) && Cell(x + 0, y + r) == eWATER)
		erosionRate += WATER_E_RATE * 2.f;
	if (InBounds(x + 0, y - r) && Cell(x + 0, y - r) == eWATER)
		erosionRate += WATER_E_RATE * 2.f;

	m_state[Coord(x, y)].hp -= erosionRate;
	//clr.x *= min(m_state[Coord(x, y)].hp, 1.f);
	//clr.y *= min(m_state[Coord(x, y)].hp, 1.f);
	//clr.z *= min(m_state[Coord(x, y)].hp, 1.f);

	float hp = m_state[Coord(x, y)].hp;

	clr.x *= hp;
	clr.y *= hp;
	clr.z *= hp;

	if (m_state[Coord(x, y)].hp <= 0.f)
		SetTile(x, y, eAIR);
}

void CellGrid::SimulatePerlinStone(int x, int y, sm::Vector3& clr)
{
}

int CellGrid::Coord(int x, int y) const
{
	return y * m_gridWidth + x;
}
