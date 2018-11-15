#include "Entities.h"

GridLine::GridLine(): body_(Matrix::CreateScale(0.5f, 8, 0.5f))
{

}

GridLine::GridLine(const Position& orientation)
{
	switch (orientation)
	{
		case Left:
			body_ = Matrix::CreateScale(0.5f, 8, 0.5f) * Matrix::CreateTranslation(-1.5f, 0, 0);
			break;
		case Right:
			body_ = Matrix::CreateScale(0.5f, 8, 0.5f) * Matrix::CreateTranslation(1.5f, 0, 0);
			break;
		case Top:
			body_ = Matrix::CreateScale(0.5f, 8, 0.5f) * Matrix::CreateRotationZ(PI * 0.5f) * Matrix::CreateTranslation(0, 1.5f, 0);
			break;
		case Bottom:
			body_ = Matrix::CreateScale(0.5f, 8, 0.5f) * Matrix::CreateRotationZ(PI * 0.5f) * Matrix::CreateTranslation(0, -1.5f, 0);
			break;
	}
}

void GridLine::Render() const
{
	gWorld = body_;
	RenderCube();
}

Symbol::Symbol() : noBodyParts_(0), currentState_(Empty)
{
		
}

Symbol::Symbol(const State& state) : noBodyParts_((short)state * 2), currentState_(state)
{
	if (noBodyParts_ == 2)		//the cross is made out of two transformed cubes
	{
		bodyParts_[0] = Matrix::CreateScale(0.25f, 2, 0.25f) * Matrix::CreateRotationZ(PI * 0.25f);
		bodyParts_[1] = Matrix::CreateScale(0.25f, 2, 0.25f) * Matrix::CreateRotationZ(-PI * 0.25f);
	}
	else if (noBodyParts_ == 4)	//the naught is made out of four transformed cubes
	{
		bodyParts_[0] = Matrix::CreateScale(0.25f, 1, 0.25f) * Matrix::CreateTranslation(-0.4f, 0, 0);
		bodyParts_[1] = Matrix::CreateScale(0.25f, 1, 0.25f) * Matrix::CreateRotationZ(PI * 0.5f) * Matrix::CreateTranslation(0, 0.65f, 0);
		bodyParts_[2] = Matrix::CreateScale(0.25f, 1, 0.25f) * Matrix::CreateTranslation(0.4f, 0, 0);
		bodyParts_[3] = Matrix::CreateScale(0.25f, 1, 0.25f) * Matrix::CreateRotationZ(PI * 0.5f) * Matrix::CreateTranslation(0, -0.65f, 0);
	}
}

bool Symbol::IsEmpty() const
{
	return !noBodyParts_;
}

short Symbol::GetNumberOfBodyParts() const
{
	return noBodyParts_;
}

State Symbol::CurrentState()
{
	return currentState_;
}

Matrix* Symbol::GetBodyParts()
{
	return bodyParts_;
}

Grid::Grid()			//create an empty grid
{
	const Symbol empty;
	for (short i = 0; i < noSlots_; i++)
		symbols_[i] = empty;
}

void Grid::RenderSymbols() 
{
	short noSlotsInARow = 3;
	for (short i = 0; i < noSlots_; i++)
	{
		Matrix* bodyParts = symbols_[i].GetBodyParts();
		for (short j = 0; j < symbols_[i].GetNumberOfBodyParts(); j++) 
		{
			//translates the symbols at the right coordinates, from the top left corner (-3, 3, 0) to the bottom right corner (3, -3, 0)
			gWorld = bodyParts[j] * Matrix::CreateTranslation((float)((i % noSlotsInARow - 1) * noSlotsInARow), (float)((i / noSlotsInARow - 1) * -noSlotsInARow), 0);
			RenderCube();
		}
	}
}

void Grid::RenderLines() const
{
	GridLine left(GridLine::Left), right(GridLine::Right), top(GridLine::Top), bottom(GridLine::Bottom);
	left.Render();
	right.Render();
	top.Render();
	bottom.Render();
}

short Grid::NumberOfSlots() const
{
	return noSlots_;
}

Symbol* Grid::GetSymbols() 
{
	return symbols_;
}

Computer::Computer() : ppSymbols_(nullptr)
{

}

Computer::Computer(short numberOfSlots, Symbol *pSymbols) : noSlotsInARow_((short)sqrt(numberOfSlots))	//converts the 1D array into a 2D one
{
	this->ppSymbols_ = new Symbol*[noSlotsInARow_];
	for(short i = 0; i < noSlotsInARow_; i++)
		this->ppSymbols_[i] = new Symbol[noSlotsInARow_];
	for (short i = 0; i < numberOfSlots; i++)
		this->ppSymbols_[i / noSlotsInARow_][i % noSlotsInARow_] = pSymbols[i];
}

bool Computer::IsFullOf(const vector<State>& values, const State& value) //checks to see if the vector of states only contains that specific state
{
	short numberOfValues = 0;
	for (State currentValue : values)
		if (currentValue == value)			//counts the position in the vector where the state "value" is present
			numberOfValues++;				
	if (numberOfValues == values.size()) 
		return true;
	return false;
}

short Computer::IsMissingOnceAtPosition(const vector<State>& values, const State& value)	//returns the empty position in the vector,
{																							//if there are multiple positions or none, it returns -1
	short numberOfValues = 0, position = -1;
	for (size_t i = 0; i < values.size(); i++)
		if (values.at(i) == value)			//counts the position in the vector where the state "value" is present
			numberOfValues++;				
		else if (values.at(i) == Empty && position == -1)	//finds the empty position
			position = (short)i;
		else						//if there are more empty positions, then it fails (returns -1)
		{
			position = -1;
			break;
		}
	return position;				//remains -1 in case no empty position was found
}

Region Computer::TryToConnect3(const State& state)					
{
	Region next = None;
	vector<State> row, column1, column2, column3, diagonal1, diagonal2;		//all the possible ways in which the "AI" can win or avoid losing 
	for (short i = 0; i < noSlotsInARow_; i++)
	{
		for (short j = 0; j < noSlotsInARow_; j++)
		{
			row.push_back(ppSymbols_[i][j].CurrentState());					//for less memory utilisation, the row vector is used for storing all the rows, one at a time
			if (next == None && row.size() == noSlotsInARow_)				//once the vector has all the elements of a row,
			{
				if (IsMissingOnceAtPosition(row, state) != -1)				//it fills in the gap (only if it is only one such gap)
					next = (Region)(i * noSlotsInARow_ + IsMissingOnceAtPosition(row, state));
				row.clear();												//the vector gets cleared so it can store the elements of the next row
			}
		}
		column1.push_back(ppSymbols_[i][0].CurrentState());							//the first column (left to right)
		column2.push_back(ppSymbols_[i][1].CurrentState());							//the second column (left to right)
		column3.push_back(ppSymbols_[i][2].CurrentState());							//the third column (left to right)
		diagonal1.push_back(ppSymbols_[i][i].CurrentState());						//the diagonal from the top left corner to the bottom right one
		diagonal2.push_back(ppSymbols_[i][noSlotsInARow_ - 1 - i].CurrentState());	//the diagonal from the top right corner to the bottom left one
	}
	//try to fill in the gap if no other previous move was made
	//------------------------------------------------------------------------------------------
	if (next == None && IsMissingOnceAtPosition(column1, state) != -1)
		next = (Region)(IsMissingOnceAtPosition(column1, state) * noSlotsInARow_);
	if (next == None && IsMissingOnceAtPosition(column2, state) != -1)
		next = (Region)(IsMissingOnceAtPosition(column2, state) * noSlotsInARow_ + 1);
	if (next == None && IsMissingOnceAtPosition(column3, state) != -1)
		next = (Region)(IsMissingOnceAtPosition(column3, state) * noSlotsInARow_ + 2);
	if (next == None && IsMissingOnceAtPosition(diagonal1, state) != -1)
		next = (Region)(IsMissingOnceAtPosition(diagonal1, state) * (noSlotsInARow_ + 1));
	if (next == None && IsMissingOnceAtPosition(diagonal2, state) != -1)
		next = (Region)((IsMissingOnceAtPosition(diagonal2, state) + 1) * (noSlotsInARow_ - 1));
	//------------------------------------------------------------------------------------------
	return next;
}

bool Computer::AbleToConnect3(const State& state)						
{																		
	bool connected = false;												
	vector<State> row, column1, column2, column3, diagonal1, diagonal2;		//very similar to the function TryToConnect3,
	for (short i = 0; i < noSlotsInARow_; i++)
	{
		for (short j = 0; j < noSlotsInARow_; j++)
		{
			row.push_back(ppSymbols_[i][j].CurrentState());
			if (row.size() == noSlotsInARow_)
			{
				connected = IsFullOf(row, state);							//but, this time, it checks for 3 connected regions, 
				row.clear();
			}																
		}																	//rather than trying to connect the third one
		column1.push_back(ppSymbols_[i][0].CurrentState());
		column2.push_back(ppSymbols_[i][1].CurrentState());
		column3.push_back(ppSymbols_[i][2].CurrentState());
		diagonal1.push_back(ppSymbols_[i][i].CurrentState());
		diagonal2.push_back(ppSymbols_[i][noSlotsInARow_ - 1 - i].CurrentState());
	}
	return connected || IsFullOf(column1, state) || IsFullOf(column2, state) || IsFullOf(column3, state) || IsFullOf(diagonal1, state) || IsFullOf(diagonal2, state);
}

Region Computer::Next()						//the "AI" makes a move
{
	Region next = None;
	next = TryToConnect3(Naught);			//tries to win
	if (next == None)
		next = TryToConnect3(Cross);		//tries to avoid losing
	if (next == None)
	{
		srand((unsigned int)time(0));	
		do
		{
			next = (Region)(rand() % 9);	//picks randomly, as there is no danger of losing, nor any way of wining in one move
		} while (!ppSymbols_[next / noSlotsInARow_][next % noSlotsInARow_].IsEmpty());
	}
	return next;
}

GameManager::GameManager(short numberOfSlots, Symbol *pSymbols, Computer *pComputer) : noSlots_(numberOfSlots), round_(0), turn_(Player), result_(Ongoing), pSymbols_(pSymbols), pComputer_(pComputer)
{

}

void GameManager::Assign(const State& state, const Region& region)
{																	//only one symbol can be placed at any position(region)
	if (pSymbols_[(short)region].IsEmpty() && region != None)		//also, "region != None" is in place so that the "AI" doesn't do an additional move after the player wins,
	{																//because the player always goes first, and the "AI" follows
		round_++;													
		CheckForTie();
		pSymbols_[(short)region] = state;
		*pComputer_ = { noSlots_, pSymbols_ };						//let the "AI" know how the grid looks like
		turn_ = (Turn)!turn_;										
		if (turn_ == AI)
		{
			CheckForWin();
			if(result_ == Ongoing)
				Assign(Naught, pComputer_->Next());					//the computer's next move
		}
	}
}

void GameManager::CheckForWin()
{
	if (pComputer_->AbleToConnect3(Cross) && result_ == Ongoing)
		result_ = Win;
}

void GameManager::CheckForLoss()
{
	if (pComputer_->AbleToConnect3(Naught) && result_ == Ongoing)
		result_ = Loss;
}

void GameManager::CheckForTie()
{
	if (round_ == 9)
		result_ = Tie;
}
