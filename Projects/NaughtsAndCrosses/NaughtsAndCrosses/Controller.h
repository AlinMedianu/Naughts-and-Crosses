#ifndef CONTROLLER_H
#define CONTROLLER_H
	enum Region {None = -1,  NW, N, NE, W, C, E, SW, S, SE }; //the 9 possible places on the grid, with the added default state of -1
	class GridRegionMessenger		//used to send the player's input from main.cpp to Game.cpp
	{
		private:
			bool regionChanged_;
			Region currentRegion_;
			RECT screen_;
			void changeRegion();
		public:
			GridRegionMessenger();
			void SetRegion(const POINT&);
			void SetScreen(const RECT&);
			bool HasRegionChanged() const;
			Region CurrentRegion();

	};
#endif
