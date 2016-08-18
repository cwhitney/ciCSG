//
//  Polygon.h
//
//  Created by lars berg on 3/3/15.
//

#pragma once

#include <vector>
#include "cinder\Vector.h"

#include <Triangle.h>
#include <BoundBox.h>

namespace ciCSG
{
	class Polygon
	{
	public:
		Polygon();
		
		Polygon( ci::vec3 a, ci::vec3 b, ci::vec3 c );
		
		~Polygon();
		
		void addTriangle( ci::vec3 a, ci::vec3 b, ci::vec3 c );
		
		void addTriangle( Triangle t );
		
		void set( ci::vec3 a, ci::vec3 b, ci::vec3 c );
		
		void setClassification( Classification classification );
		
		ci::vec3 getNormal();
		
		float getW();
		
		void clear();
		
		void flip();
		
		std::vector<ofPolyline> toPolylines();
		
		void split( Triangle& t );
		
		void split( Polygon& p );
		
		void split( ci::vec3 t0, ci::vec3 t1, ci::vec3 t2 );
		
		bool intersectRay( ci::vec3 rayOrigin, ci::vec3 rayDir, float epsilon, ci::vec3* intersection = NULL );
		
		void classifyRay( Polygon& p, int& intersectionCount );
		
		void classify(std::vector<Polygon>& polygons );
		
		void draw(bool useNormalForColor = true);
		
		
		//
		std::vector<Triangle> triangles;
		BoundBox bb;
		
		std::vector< Polygon* > splitters;
		
		bool wasSplit;
		
		int rayIntersectionCount;
		
		
		//for coplanar splitting
//		vector<ofPolyline> polylines;
//		vector<LineSegment> lineSegments;
	};
}