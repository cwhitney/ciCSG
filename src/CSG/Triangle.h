//
//  Triangle.h
//
//  Created by lars berg on 3/3/15.
//

#pragma once

#include "cinder/Vector.h"
#include "cinder/gl/gl.h"
#include "cinder/PolyLine.h"
#include "cinder/TriMesh.h"

#include <LineSegment.h>
#include <vector>

namespace ciCSG
{	
	class Triangle
	{
	public:
		Triangle(ci::vec3 a, ci::vec3 b, ci::vec3 c);
		
		Triangle();
		~Triangle();
		
		ci::vec3* getPtr();
		
		ci::vec3& operator[]( int n );
		
		void set( ci::vec3 _a, ci::vec3 _b, ci::vec3 _c );
		
		void flip();
		
		void calcNormal();
		
		float getArea();
		
		float getAreaSquared();
		
		bool rayIntersect( ci::vec3 rayOrigin, ci::vec3 rayDir );
		
		bool intersectRay( ci::vec3 rayOrigin, ci::vec3 rayDir, ci::vec3* intersection = NULL );

		bool intersectRay( ci::vec3 rayOrigin, ci::vec3 rayDir, float epsilon, ci::vec3* intersection = NULL );
		
		ci::vec3 getCenter();
		
		void draw(bool useNormalForColor = true);
		
		ci::PolyLine3f toPolyline();
		
		Classification getClassification( ci::vec3 planeNormal, float planeW );
		
		void classifyWithPlane( ci::vec3 planeNormal, float planeW );
		
		std::vector<ci::vec3> intersectWithPlane( ci::vec3 planeNormal, float planeW );
		
		bool getIntersection( Triangle t, LineSegment* overlap );
		
		std::vector<Triangle> insert( ci::vec3 v );

		std::vector<Triangle> splitWithCoplanarSegment(ci::vec3 a, ci::vec3 b);
		
		std::vector<Triangle> splitWithCoplanarSegment(LineSegment segment);
		
		std::vector<Triangle> split( Triangle t );
		
		std::vector<Triangle> meshToTriangles(ci::TriMeshRef m);
		
		ci::vec3 a, b, c, centroid;
		ci::vec3 normal;
		float w;
		Classification classification;
	};
	
	
	//STATIC METHODS
	static std::vector<Triangle> meshToTriangles(ci::TriMeshRef m)
	{
		vector<Triangle> triangles;
		
		auto indices = m->getIndices();
		auto v = m->getPositions<3>();
		size_t numPos = m->getNumVertices();
		
		if(indices.size())
		{
			for(int i=0; i<indices.size(); i+=3)
			{
				triangles.push_back( Triangle( v[ indices[i] ], v[ indices[i+1] ], v[ indices[i+2] ] ) );
			}
		}
		else
		{
			for(int i=0; i<numPos; i+=3)
			{
				triangles.push_back( Triangle( v[i], v[i+1], v[i+2] ) );
			}
		}
		
		return triangles;
	}
}