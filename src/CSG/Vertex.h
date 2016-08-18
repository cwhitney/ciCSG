//
//  Vertex.h
//
//  Created by lars berg on 3/3/15.
//

#pragma once

#include "cinder/Vector.h"
#include <Utils.h>

namespace ciCSG
{
	class Vertex : public ci::vec3
	{
	public:
		Vertex( float x=0, float y=0, float z=0) :
		ci::vec3(x, y, z),
		classification(UNDEFINED)
		{}
		
		Vertex( ci::vec3 v3 ) :
		ci::vec3( v3 ),
		classification(UNDEFINED)
		{}
		
		~Vertex()
		{}
		
		Classification classification;
	};
}