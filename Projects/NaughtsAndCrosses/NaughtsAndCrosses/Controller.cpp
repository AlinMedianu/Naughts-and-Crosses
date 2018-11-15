#include <windows.h>

#include "Controller.h"

GridRegionMessenger::GridRegionMessenger() : regionChanged_(false), currentRegion_(NW), screen_({ 0,0,0,0 })
{

}

void GridRegionMessenger::changeRegion()		//this and the hasRegionChanged functions are used to get or set a bool
{												//so that the CurrentRegion function can be called only when 
	regionChanged_ = true;						//the player clicked on the screen instead of every frame
}

void GridRegionMessenger::SetRegion(const POINT& mousePosition)						//decides where on the screen, the player clicked
{
	if (mousePosition.x < screen_.left + (screen_.right - screen_.left) * 0.33333f)
		if (mousePosition.y < screen_.top + (screen_.bottom - screen_.top) * 0.33333f)
			currentRegion_ = NW;
		else if (mousePosition.y < screen_.top + (screen_.bottom - screen_.top) * 0.66667f)
			currentRegion_ = W;
		else
			currentRegion_ = SW;
	else if (mousePosition.x < screen_.left + (screen_.right - screen_.left) * 0.66667f)
		if (mousePosition.y < screen_.top + (screen_.bottom - screen_.top) * 0.33333f)
			currentRegion_ = N;
		else if (mousePosition.y < screen_.top + (screen_.bottom - screen_.top) * 0.66667f)
			currentRegion_ = C;
		else
			currentRegion_ = S;
	else
		if (mousePosition.y < screen_.top + (screen_.bottom - screen_.top) * 0.33333f)
			currentRegion_ = NE;
		else if (mousePosition.y < screen_.top + (screen_.bottom - screen_.top) * 0.66667f)
			currentRegion_ = E;
		else
			currentRegion_ = SE;
	changeRegion();
}

void GridRegionMessenger::SetScreen(const RECT& screen)
{
	screen_ = screen;
}

bool GridRegionMessenger::HasRegionChanged() const
{
	return regionChanged_;
}

Region GridRegionMessenger::CurrentRegion()
{
	regionChanged_ = false;
	return currentRegion_;
}