//
//  LineSegment.h
//
//  Created by lars berg on 3/3/15.
//

#pragma once

#include <vector>

#include "cinder/Vector.h"
#include "Utils.h"

namespace ciCSG
{
	class LineSegment
	{
	public:
		LineSegment( ci::vec3 a, ci::vec3 b);
		
		LineSegment();
		
		void set( ci::vec3 p0, ci::vec3 p1 );
		
		void expandToPoint( ci::vec3 p );
		
		float lengthSquared();
		
		bool subtract( LineSegment segment );
		
		bool subtract( ci::vec3 p0, ci::vec3 p1 );
		
		bool trimToTriangle( ci::vec3 ta, ci::vec3 tb, ci::vec3 tc);
		
		ci::vec3* getPtr();
		
		ci::vec3& operator[]( int n );
		
		ci::vec3 a, b, normal;
	};
}