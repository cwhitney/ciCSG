//
//  BoundBox.h
//
//	Created by lars berg on 3/3/15.
//

#pragma once

#include <vector>
#include "cinder/Vector.h"
#include "cinder/CinderMath.h"
#include "cinder/Log.h"

namespace ciCSG
{
	class BoundBox
	{
	public:
		bool initialized;
		ci::vec3 minBound, maxBound;

		BoundBox() :
			initialized(false),
			minBound(ci::vec3()),
			maxBound(ci::vec3())
		{}

		~BoundBox()
		{}

		void addPoint(ci::vec3 p)
		{
			if (!initialized)
			{
				initialized = true;

				minBound = maxBound = p;
			}
			else
			{
				for (int i = 0; i < 3; i++)
				{
					minBound[i] = glm::min(p[i], minBound[i]);
					maxBound[i] = glm::max(p[i], maxBound[i]);
				}

				if (minBound.x > maxBound.x || minBound.y > maxBound.y || minBound.z > maxBound.z)
				{
					CI_LOG_I("minBound: ") << minBound;
					CI_LOG_I("maxBound: ") << maxBound;
				}
			}
		}

		void addPoints(std::vector<ci::vec3> points)
		{
			for (auto& p : points)	addPoint(p);
		}

		void addPoints(ci::vec3 a, ci::vec3 b, ci::vec3 c)
		{
			addPoint(a);
			addPoint(b);
			addPoint(c);
		}

		void clear()
		{
			initialized = false;
		}

		bool overlap(float minA, float maxA, float minB, float maxB)
		{
			if (maxA < minB || maxB < minA)
			{
				return false;
			}
			else if (minB <= minA && minA <= maxB)
			{
				return true;
			}
			else if (minA <= minB && minB <= maxA)
			{
				return true;
			}

			return false;
		}

		bool isPointInside(ci::vec3 p)
		{
			return minBound.x <= p.x && p.x <= maxBound.x && minBound.y <= p.y && p.y <= maxBound.y && minBound.z <= p.z && p.z <= maxBound.z;
			//			if( minBound.x <= p.x && p.x <= maxBound.x )
			//			{
			//				if( minBound.y <= p.y && p.y <= maxBound.y )
			//				{
			//					if( minBound.z <= p.z && p.z <= maxBound.z )
			//					{
			//						return true;
			//					}
			//					return false;
			//				}
			//				return false;
			//			}
			//			return false;
		}

		bool intersects(BoundBox& bb)
		{
			if (overlap(minBound.x, maxBound.x, bb.minBound.x, bb.maxBound.x))
			{
				if (overlap(minBound.y, maxBound.y, bb.minBound.y, bb.maxBound.y))
				{
					return overlap(minBound.z, maxBound.z, bb.minBound.z, bb.maxBound.z);
				}
				return false;
			}
			return false;
		}

		static bool intersectBoxes(BoundBox& a, BoundBox& b)
		{
			return a.intersects(b);
		}



	};
}