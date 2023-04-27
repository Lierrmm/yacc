#include <std_include.hpp>

#include <game/game.hpp>

#include "time.hpp"

namespace utils
{
	namespace time
	{
		void Interval::update()
		{
			this->lastPoint = std::chrono::high_resolution_clock::now();
		}

		bool Interval::elapsed(std::chrono::nanoseconds nsecs)
		{
			return ((std::chrono::high_resolution_clock::now() - this->lastPoint) >= nsecs);
		}

		Point::Point() : lastPoint(game::native::Sys_Milliseconds())
		{

		}

		void Point::update()
		{
			this->lastPoint = game::native::Sys_Milliseconds();
		}

		int Point::diff(Point point)
		{
			return point.lastPoint - this->lastPoint;
		}

		bool Point::after(Point point)
		{
			return this->diff(point) < 0;
		}

		bool Point::elapsed(int milliseconds)
		{
			return (game::native::Sys_Milliseconds() - this->lastPoint) >= milliseconds;
		}
	}
}
