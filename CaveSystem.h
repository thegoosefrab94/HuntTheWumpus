#pragma once
#include <memory>	// only used to be able to use std::size_t

namespace Wump
{
	std::size_t NumRoll(std::size_t, std::size_t);
	class Player;	// foward declares
	class Wumpus;
	class Cave;

	class CaveSystem
	{
	public:
		CaveSystem();
		bool Init();
		void AttachPlayer(Player* pPlay) { m_pPlayer = pPlay; }	// attach the player to the cavesystem
		void Destroy();
		void Update();

		void PlayerShot();			// a command to send into the system that the player shot an arrow

		const Cave* Start() { return m_pStart; }
		~CaveSystem();
	private:
		bool InitSystem();
		Wumpus* m_pWumpus;
		Player* m_pPlayer;
		Cave* m_pStart;
		Cave* m_pEnd;	// only used in tracking the end
	};
}