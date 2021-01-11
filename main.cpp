#include <iostream>
#include <iomanip>
#include <map>
#include <string>
#include "./src/CaveSystem/CaveSystem.h"


const std::string moveCommand{ "move" };
const std::string shootCommand{ "shoot" };
const std::string endCommand{ "exit" };
const std::string helpCommand{ "help" };
bool exited = false;

const std::string north{ "north" };
const std::string south{ "south" };
const std::string east{ "east" };
const std::string west{ "west" };

std::map<std::string, Wump::Direction> directionMap{ { north, Wump::Direction::north },
{ south, Wump::Direction::south },{ east, Wump::Direction::east },{ west, Wump::Direction::west } };

void GetInput(Wump::Player&);
void PrintIntro();
void PrintHelp();

int main()
{
	using namespace Wump;
	CaveSystem system{ std::cout };
	CaveBuilder builder;
	system.CreateSystem(builder);
	Player* player;
	bool goAgain = true;

	do {
		PrintIntro();
		system.Restart();
		player = new Player{ std::cout, system.Start() };
		system.AttachPlayer(player);
		while (!system.Done()) {
			player->Sense();
			GetInput(*player);
		}
		if (!exited) {
			std::cout << "Would you like to play again?(yes or no)\n";
			std::string answer;
			std::cin >> answer;
			if (answer == "no" || answer == "No") goAgain = false;
		}
		else
			goAgain = false;
	} while (goAgain);

	std::cout << "Goodbye!\n";
	return 0;
}

void PrintIntro() {
	std::cout << "Welcome adventurer. You have been tasked with the difficult challenge of destroying the\n";
	std::cout << " horrible creature known as the Wumpus, a horrible smelling creature that is torturing the village\n";
	std::cout << "Be careful while navigating within the cave. There are bottomless pits,\n";
	std::cout << " giant bats that will move you to a random cave, and of course the Wumpus itself.\n";
	std::cout << "To navigate simply type " << moveCommand << " and enter the direction you wish to go.\n";
	std::cout << "To fire an arrow, type " << shootCommand << " and the direction you wish\n";
	std::cout << "Remember you only have a small amount of arrows\n";
	std::cout << "Goodluck!\n";
	std::cout << "To get a list of possible commands type \"" << helpCommand << "\"\n";
}

void GetInput(Wump::Player& player) {
	std::string command;
	std::cin >> command;
	if (command == moveCommand) {
		std::string direction;
		std::cin >> direction;
		if (direction != north && direction != south
			&& direction != east && direction != west) {
			std::cout << "Must enter a valid command and direction.\n";
		}
		else
			player.Move(directionMap[direction]);
	}
	else if (command == shootCommand) {
		std::string direction;
		std::cin >> direction;
		if (direction != north && direction != south
			&& direction != east && direction != west) {
			std::cout << "Must enter a valid command and direction.\n";
		}
		else
			player.Shoot(directionMap[direction]);
	}
	else if (command == endCommand) {
		player.Kill();
		exited = true;
	}
	else if (command == helpCommand) {
		PrintHelp();
	}
	else {
		std::cout << "Must enter a valid command.\n";
	}
}

void PrintHelp() {
	using std::cout;
	using std::endl;
	const int maxWidth = 50;
	const int commandWidth = 10;
	const int descriptionWidth = maxWidth - commandWidth;
	cout << std::left;
	cout << std::setw(commandWidth) << "COMMAND" << std::setw(descriptionWidth) <<"DESCRIPTION\n";
	cout << std::setw(maxWidth) << std::setfill('-') << "\n" << std::setfill(' ') << endl;
	cout << std::setw(commandWidth) << moveCommand << std::setw(descriptionWidth) << "Typing this command followed by a direction will allow you to navigate your character through the cave system\n" << endl;
	cout << std::setw(commandWidth) << shootCommand << std::setw(descriptionWidth) << "Typing this command followed by a direction will fire one of your arrows (provided you have any left) in the direction you specify. This will kill any enemy that is in the cave of your direction, including the Wumpus!\n" << endl;
	cout << std::setw(commandWidth) << " " << std::setw(descriptionWidth) << "\tPossible directions to type are " << north << ", " << south << ", " << east << ", and " <<  west << endl << endl;
	cout << std::setw(commandWidth) << endCommand << std::setw(descriptionWidth) << "Typing this command will end the game\n" << endl;
	cout << std::setw(commandWidth) << helpCommand << std::setw(descriptionWidth) << "Typing this will display this information again\n" << endl;
}