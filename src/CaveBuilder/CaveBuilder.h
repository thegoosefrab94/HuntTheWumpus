#pragma once
#include "../CaveSites/CaveSites.h"

namespace Wump
{
	class CaveBuilder {	// Builds a cave from individual parts
	public:
		CaveBuilder() : m_pCave(nullptr) {  }	// Almost all virtual functions incase a later subclass needs to change something
		virtual void CreateCave();
		virtual void CreateRoom(std::size_t num);
		virtual void CreateTunnel(std::size_t fromRoom, std::size_t toRoom, Direction dir);
		virtual void CreateWall();

		virtual Cave* GetCave() const { return m_pCave; }
		virtual ~CaveBuilder() {  }
	private:
		Cave* m_pCave;
	};
}
