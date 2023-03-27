#pragma once
#include "string.hpp"
#include <game/game.hpp>

namespace utils::time
{
	class Interval
	{
	protected:
		std::chrono::high_resolution_clock::time_point lastPoint;

	public:
		Interval() : lastPoint(std::chrono::high_resolution_clock::now()) {}

		void update();
		bool elapsed(std::chrono::nanoseconds nsecs);
	};

	class Point
	{
	public:
		Point();

		void update();
		int diff(Point point);
		bool after(Point point);
		bool elapsed(int milliseconds);

	private:
		int lastPoint;
	};
}