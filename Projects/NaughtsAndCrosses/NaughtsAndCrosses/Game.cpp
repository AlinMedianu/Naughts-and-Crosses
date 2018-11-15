#include "d3d.h"
#include "Entities.h"

//link to some functions-variables in other files
extern GridRegionMessenger regionMessenger;

Grid grid;
Computer computer;
GameManager gameManger(grid.NumberOfSlots(), grid.GetSymbols(), &computer);

void Update(float dTime) //checks the state of the game (player win, computer win, tie or ongoing) every frame
{
	if (gameManger.result_ == GameManager::Win)
	{
		MessageBox(NULL, "You win!", "ATTENTION", MB_OK | MB_ICONEXCLAMATION);
		PostQuitMessage(0);
	}
	else if (gameManger.result_ == GameManager::Loss)
	{
		MessageBox(NULL, "You lose!", "ATTENTION", MB_OK | MB_ICONEXCLAMATION);
		PostQuitMessage(0);
	}
	else if (gameManger.result_ == GameManager::Tie)
	{
		MessageBox(NULL, "It's a tie!", "ATTENTION", MB_OK | MB_ICONEXCLAMATION);
		PostQuitMessage(0);
	}
}


void Paint(float dTime) //renders the graphics every frame
{
	grid.RenderLines(); //renders the lines of the grid
	if (regionMessenger.HasRegionChanged() && gameManger.turn_ == GameManager::Player) //receives the imput from the controller and validates it
	{
		gameManger.Assign(Cross, regionMessenger.CurrentRegion());
		gameManger.CheckForLoss();
	}
	grid.RenderSymbols(); //renders the possible naughts and crosses
}
