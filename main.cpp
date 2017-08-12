#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include "HuntTheWumpus.h"

struct Token
{
	Token() = default;
	Token(char tt) : cave{ 0 }, type{ tt } {   }
	Token(char tt, int cc) : cave{ cc }, type{ tt } {  }
	int cave;
	char type;
};

class Stream
{
public:
	Stream(std::istream& is) : m_Stream{ is }, m_Data{ {} }, m_bFull{ false } {   }
	Stream(const Stream&) = delete;
	Stream& operator=(const Stream&) = delete;
	Token get();
	void putback(Token t);
	void ignore(char);
private:
	std::istream& m_Stream;
	Token m_Data;
	bool m_bFull;
};

void Stream::putback(Token t)
{
	if (m_bFull) throw std::runtime_error("putback() into full stream");
	m_Data = t;
	m_bFull = true;
}
void Stream::ignore(char c)
{
	if (m_bFull && c == m_Data.type)
	{
		m_bFull = false;
		return;
	}
	m_bFull = false;
	char ch;
	while (m_Stream >> ch)
		if (ch == c) return;
}

constexpr char move_c = 'm';			// xx_c is "the xx character"
constexpr char shoot_c = 's';
constexpr char cave_c = 'c';
constexpr char cont_c = '-';
constexpr char end_c = ';';
constexpr char quit_c = 'q';
const std::string move_w = "move";		// xx_w is "the xx word"
const std::string shoot_w = "shoot";

Token Stream::get()
{
	if (m_bFull)
	{
		m_bFull = false;
		return m_Data;
	}
	char ch;
	m_Stream >> ch;
	switch (ch)
	{
		case end_c: case quit_c: case cont_c:
			return Token{ ch };
		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
		{
			m_Stream.putback(ch);
			int cv;
			m_Stream >> cv;
			return Token{ cave_c, cv };
		}
		default:
		{
			if(isalpha(ch))
			{
				std::string ss;
				ss += ch;
				while (m_Stream.get(ch) && isalpha(ch)) ss += ch;
				m_Stream.putback(ch);
				if (ss == move_w)
				{
					int cv;
					m_Stream >> cv;
					return Token{ move_c, cv };
				}
				if (ss == shoot_w)
					return Token{ shoot_c };
			}
			throw std::runtime_error("Bad command");
		}
	}
}

using namespace std;

Stream str(cin);

vector<int> shots()
{
	vector<int> ret;
	while (true)
	{
		Token t = str.get();
		switch (t.type)
		{
			case cave_c:
			{
				ret.push_back(t.cave);
				break;
			}
			case cont_c:
				break;
			default:
			{
				str.putback(t);
				return ret;
			}
		}
	}
}
vector<int> toCave()
{
	while (true)
	{
		Token t = str.get();
		switch (t.type)
		{
		case move_c:
			return vector<int> {t.cave};
		case shoot_c:
			return shots();
		default:
			throw std::runtime_error("Cannot read command");
		}
	}
}

void cleanUp()
{
	str.ignore(end_c);
}
bool getCommands(shared_ptr<Wump::Player> pPlay, shared_ptr<Wump::CaveSystem> pSys)
{
	if (!pPlay)
	{
		cout << "Invalid player pointer\n";
		return false;
	}
	if (!pSys)
	{
		cout << "Invalid system pointer\n";
		return false;
	}

	while(true)
	try {
		Token t = str.get();
		while (t.type == end_c) t = str.get();
		if (t.type == quit_c) break;
		str.putback(t);
		if (t.type == move_c)
		{
			auto cav = toCave();
			int moveCave = cav.at(0);
			if (moveCave == pPlay->Here()->ToLeft()->Number())	// test each cave number to the one given
			{
				cout << "Move to cave " << moveCave << '\n' << endl;
				pPlay->Move(pPlay->Here()->ToLeft());
				return true;
			}
			else if (moveCave == pPlay->Here()->ToMiddle()->Number())
			{
				cout << "Move to cave " << moveCave << '\n' << endl;
				pPlay->Move(pPlay->Here()->ToMiddle());
				return true;
			}
			else if (moveCave == pPlay->Here()->ToRight()->Number())
			{
				cout << "Move to cave " << moveCave << '\n' << endl;
				pPlay->Move(pPlay->Here()->ToRight());
				return true;
			}
			else
				throw std::runtime_error("Not a valid Cave");
		}
		if (t.type == shoot_c)
		{
			auto vec = toCave();	
			if (vec.size() > 3)		// only three caves to choose from
				vec.resize(3);
			for (int i : vec)
			{
				if (i == pPlay->Here()->ToLeft()->Number())
				{
					cout << "Shoot cave " << i << '\n' << endl;
					pPlay->Shoot(pPlay->Here()->ToLeft());
					pSys->PlayerShot();
				}
				if (i == pPlay->Here()->ToMiddle()->Number())
				{
					cout << "Shoot cave " << i << '\n' << endl;
					pPlay->Shoot(pPlay->Here()->ToMiddle());
					pSys->PlayerShot();
				}
				if (i == pPlay->Here()->ToRight()->Number())
				{
					cout << "Shoot cave " << i << '\n' << endl;
					pPlay->Shoot(pPlay->Here()->ToRight());
					pSys->PlayerShot();
				}
			}
			return true;
		}
	}
	catch (runtime_error& e)
	{
		cerr << e.what() << '\n';
		cleanUp();
	}
	return false;
}

void printIntro();
int keepOpen(const string& word = "")
{
	cout << "Enter " << word << " to exit.";
	for (string s; cin >> s;)
	{
		if (s == word) return 1;
	}
	return 1;
}

int main()
try {
	shared_ptr<Wump::CaveSystem> pCaveSystem(new Wump::CaveSystem);
	if (pCaveSystem)
	{
		if (pCaveSystem->Init())
		{
			auto* cv = pCaveSystem->Start();
			shared_ptr<Wump::Player> pPlayer(new Wump::Player(cv));
			if (pPlayer)
			{
				pCaveSystem->AttachPlayer(pPlayer.get());
				printIntro();
				while (pPlayer->IsAlive() && !pPlayer->IsDone())
				{
					pPlayer->ThisCave();
					pPlayer->Sense();
					if (!getCommands(pPlayer, pCaveSystem))
						break;
					pCaveSystem->Update();
				}
			}
		}
	}
	keepOpen("~~");
	return 0;
}
catch (exception& e)
{
	cerr << e.what() << std::endl;
	keepOpen("~~");
}
catch (...)
{
	cerr << "Error encountered" << std::endl;
	keepOpen("~~");
}

void printIntro()
{
	cout << "Hello brave adventurer!\n";
	cout << "You have been tasked with the mission of hunting the Wumpus,\n";
	cout << " the terrible monster that has been tormenting the village.\n";
	cout << "To accomplish this, you must traverse his terrible cave.\n";
	cout << "It is riddled with traps such as bottomless pits, giant bats that will take you,";
	cout << " and move you to another part of the cave, and of course the Wumpus himself.\n";
	cout << "If you feel a breeze that means there is a pit near by.\n";
	cout << "If you hear chirping, it is not birds, it is a bat.\n";
	cout << "And of course if you smell something foul, the Wumpus is near by.\n";
	cout << "Do not forget, you only have a few arrows, so use them wisely.\n";
	cout << "To move enter the command '"<< move_w << "' followed by the number cave you would like to move to.\n";
	cout << "To shoot enter the command '" << shoot_w << "'";
	cout << "\nfollowed by 3 or less numbers divided by the character '" << cont_c << endl;
	cout << "Enter " << quit_c << " to quit anytime.\n\n";
}