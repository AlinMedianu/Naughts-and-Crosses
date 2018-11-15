#ifndef ENTITIES_H
#define ENTITIES_H
#include <ctime>
#include <vector>

#include "D3DUtil.h"
#include "SimpleMath.h"
#include "Controller.h"

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;

extern Matrix gWorld;
extern void RenderCube();

class GridLine							//the lines of the grid as objects
{
	private:
		Matrix body_;
	public:
		enum Position { Left, Right, Top, Bottom };
		GridLine();
		GridLine(const Position&);
		void Render() const;
};

enum State { Empty, Cross, Naught };	//the state every position in the grid can have

class Symbol							//the naught or cross as objects
{
	private:
		short noBodyParts_;
		Matrix bodyParts_[4];
		State currentState_;
	public:
		Symbol();
		Symbol(const State&);
		bool IsEmpty() const;
		short GetNumberOfBodyParts() const;
		State CurrentState();
		Matrix* GetBodyParts();
};

class Grid								//the class that renders the symbols and grid lines
{
	private:
		static const short noSlots_ = 9;
		Symbol symbols_[noSlots_];
	public:
		Grid();
		void RenderSymbols();
		void RenderLines() const;
		short NumberOfSlots() const;
		Symbol* GetSymbols();
};

class Computer							//the "AI" class
{
	private:
		short noSlotsInARow_;
		Symbol **ppSymbols_;
		bool IsFullOf(const vector<State>&, const State&);
		short IsMissingOnceAtPosition(const vector<State>&, const State&);
		Region TryToConnect3(const State&);
	public:
		Computer();
		Computer(short, Symbol*);
		bool AbleToConnect3(const State&);
		Region Next();
};

class GameManager						//the class that manages the game's state(result) and who's turn it is
{
	private:
		short noSlots_;
		short round_;
		Symbol *pSymbols_;
		Computer *pComputer_;
	public:
		enum Turn { Player, AI } turn_;
		enum Result { Ongoing, Win, Loss, Tie } result_;
		GameManager(short, Symbol*, Computer*);
		void Assign(const State&, const Region&);
		void CheckForWin();
		void CheckForLoss();
		void CheckForTie();
};
#endif