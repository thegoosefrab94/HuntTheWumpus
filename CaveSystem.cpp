#include "CaveSystem.h"
#include "Cave.h"
#include "Actors.h"
#include <random>

namespace Wump
{
	std::size_t NumRoll(std::size_t min, std::size_t max)		// rolls a random number
	{
		static std::mt19937_64 gen;
		static std::uniform_int_distribution<std::size_t> dis(min, max);
		return dis(gen);
	}

	std::set<std::size_t> Cave::m_Set{};

	Cave::Cave() : m_Trap{ Trap::None }, left{ nullptr }, mid{ nullptr }, right{ nullptr }
	{
		m_Num = NumRoll(1, 20);
		while (m_Set.find(m_Num) != m_Set.end()) m_Num = NumRoll(1, 20);	// as long as the number is in the set keep rolling
		m_Set.insert(m_Num);	// add the number
	}
	bool Cave::IsTrapped() const
	{
		return (m_Trap == Trap::Pit || m_Trap == Trap::Bat || m_Trap == Trap::Wumpus);
	}
	Cave::~Cave()
	{
		if (this->left) this->left->right = nullptr;
		if (this->mid) this->mid->mid = nullptr;
		if (this->right) this->right->left = nullptr;
	}

	CaveSystem::CaveSystem() : m_pWumpus{ nullptr }, m_pPlayer{ nullptr }, m_pStart{ nullptr }, m_pEnd{ nullptr } {   }
	CaveSystem::~CaveSystem()	// start at the end and work back outwards
	{
		Destroy();
	}
	bool CaveSystem::Init()
	{
		if (InitSystem())
		{
			if (m_pWumpus)
				return true;
			else
				return false;
		}

		return false;
	}
	bool CaveSystem::InitSystem()	// TODO condense
	{
		m_pStart = new Cave(); // Start always points to the entrance
		if (!m_pStart)
			return false;
		Cave* pCurr = m_pStart;
		Cave* pBeg = pCurr;	// used to track to the first cave in this ring
		Cave* pOut = pCurr;	// used to track to the outer ring	

		std::size_t batRand = NumRoll(0, 3);
		for (std::size_t i = 0; i < 4; ++i)	// creates the outer ring
		{
			pCurr->left = new Cave();
			if (!pCurr->left)
			{
				m_pEnd = pCurr;
				return false;
			}
			if (i == batRand) pCurr->SetTrap(Cave::Bat);
			pCurr->left->right = pCurr;
			pCurr = pCurr->left;
		}
		// after loop connect the last cave to the beginning
		pCurr->left = pBeg;
		pBeg->right = pCurr;
		pCurr->mid = new Cave();	// starts the middle ring
		if (!pCurr->mid)
		{
			m_pEnd = pCurr;
			return false;
		}
		pCurr->mid->mid = pCurr;
		pCurr = pBeg = pCurr->mid;
		std::size_t pitRand = NumRoll(0, 8);
		batRand = NumRoll(0, 8);
		if (pitRand == batRand) pitRand = NumRoll(0, 8);

		for (std::size_t i = 0; i < 9; ++i)	// creates the inner ring
		{
			pCurr->left = new Cave();

			if (!pCurr->left)
			{
				m_pEnd = pCurr;
				return false;
			}
			pCurr->left->right = pCurr;
			if (!(i % 2) && i != 0)	// every other one
			{
				pCurr->mid = pOut;	// hook this one's middle to the one in the outer
				pOut->mid = pCurr;
				pOut = pOut->left;	// move to the left
			}
			if (i == pitRand) pCurr->SetTrap(Cave::Pit);	// randomly sets a trap
			if (i == batRand) pCurr->SetTrap(Cave::Bat);
			pCurr = pCurr->left;
		}
		pCurr->left = pBeg;
		pBeg->right = pCurr;
		pOut = pCurr;
		pCurr->mid = new Cave();	// innermost ring
		if (!pCurr->mid)
		{
			m_pEnd = pCurr;
			return false;
		}
		pCurr->mid->mid = pCurr;
		pCurr = pBeg = pCurr->mid;
		pOut = pOut->left->left;	// skip every other one
		pitRand = NumRoll(0, 3);

		for (std::size_t i = 0; i < 4; ++i)	// creates innermost ring
		{
			pCurr->left = new Cave();

			if (!pCurr->left)
			{
				m_pEnd = pCurr;
				return false;
			}

			pCurr->left->right = pCurr;

			if (i == pitRand) pCurr->SetTrap(Cave::Pit);	// two pits within the system
			pCurr = pCurr->left;
			pOut->mid = pCurr;
			pCurr->mid = pOut;
			pOut = pOut->left->left;
		}
		m_pWumpus = new Wumpus(pCurr);
		pCurr->left = pBeg;
		pBeg->right = pCurr;
		m_pEnd = pCurr;		// end is the last one created

		return true;
	}
	void CaveSystem::Destroy()
	{
		if (m_pStart && m_pEnd)
		{
			while (m_pEnd != m_pStart)
			{
				if (m_pEnd->left)	// if able to move left
				{
					m_pEnd = m_pEnd->left;	// move there and delete last
					delete m_pEnd->right;
				}
				else if (m_pEnd->mid)	// if able to move mid
				{
					m_pEnd = m_pEnd->mid;	// move and delete
					delete m_pEnd->mid;
				}
			}
			delete m_pWumpus;
			delete m_pStart;
			m_pStart = m_pEnd = nullptr;
		}
	}
	void CaveSystem::Update()
	{
		if (m_pPlayer->Here() == m_pWumpus->Here())	// if the player and wumpus meet
		{
			if (m_pWumpus->IsAlive())		// if the wumpus is still alive
			{
				m_pPlayer->Die();	// player dies
				std::cout << "The wumpus has killed you!\n";
			}
			else if(!m_pWumpus->IsAlive())
				m_pPlayer->Win();	// else then the wumpus is dead and the player wins
		}
		else
			return;
	}
	void CaveSystem::PlayerShot()
	{
		if (m_pWumpus->Here()->IsTrapped())	// if the player shoots and the wumpus is alive
		{
			m_pWumpus->WakeUp();	// wake him up
			auto cave = NumRoll(1, 3);
			switch (cave)	// move him
			{
			case 1:
				m_pWumpus->Move(m_pWumpus->Here()->ToLeft());
				break;
			case 2:
				m_pWumpus->Move(m_pWumpus->Here()->ToMiddle());
				break;
			default:
				m_pWumpus->Move(m_pWumpus->Here()->ToRight());
				break;
			}
		}
		else				// if its not trapped
			m_pWumpus->Die();	// kill him
	}
}