//
//  Polygon.cpp
//  example
//
//  Created by lars berg on 3/10/15.
//
//

#include "Polygon.h"

using namespace ci;
using namespace std;

namespace ciCSG
{
	Polygon::Polygon() :
	rayIntersectionCount( 0 )
	{}
	
	Polygon::Polygon( ci::vec3 a, ci::vec3 b, ci::vec3 c ) :
	rayIntersectionCount( 0 )
	{
		addTriangle(a, b, c);
	}
	
	Polygon::~Polygon()
	{}
	
	void Polygon::addTriangle( ci::vec3 a, ci::vec3 b, ci::vec3 c )
	{
		bb.addPoints( a, b, c );
		triangles.push_back( Triangle( a, b, c ) );
	}
	
	void Polygon::addTriangle( Triangle t )
	{
		bb.addPoints( t.a, t.b, t.c );
		triangles.push_back( t );
	}
	
	void Polygon::set( ci::vec3 a, ci::vec3 b, ci::vec3 c )
	{
		clear();
		addTriangle( Triangle( a, b, c ) );
	}
	
	void Polygon::setClassification( Classification classification )
	{
		for(auto& t: triangles)
		{
			t.classification = classification;
		}
	}
	
	ci::vec3 Polygon::getNormal()
	{
		return triangles[0].normal;
	}
	
	float Polygon::getW()
	{
		return triangles[0].w;
	}
	
	void Polygon::clear()
	{
		bb.clear();
		triangles.clear();
	}
	
	void Polygon::flip()
	{
		for(auto& t: triangles)	t.flip();
	}
	
	vector<PolyLine3f> Polygon::toPolylines()
	{
		vector<PolyLine3f> polylines;
		
		for(auto& t: triangles)
		{
			PolyLine3f p;
			/*
			p.addVertex( t.a );
			p.addVertex( t.b );
			p.addVertex( t.c );
			*/
			p.push_back(t.a);
			p.push_back(t.b);
			p.push_back(t.c);
			p.setClosed(true);
			
			polylines.push_back( p );
		}
		
		return polylines;
	}
	
	void Polygon::split( Triangle& t )
	{
		vector<Triangle> splitTriangles;
		for(auto& tri: triangles)
		{
			auto subd = tri.split( t );
			splitTriangles.insert( splitTriangles.end(), subd.begin(), subd.end() );
		}
		
		triangles = splitTriangles;
	}
	
	void Polygon::split( Polygon& p )
	{
		////TODO: if they're coplanar we should split them differnetly
		//float nDot = getNormal().dot( p.getNormal() );
		//if( false && abs(nDot) >= 1 - EPSILON )
		//{
		//	cout << "CSG::Polygon::split() - we should create a coplanar split method." << endl;
		//}
		//else
		//{
			//otherwise split the triangles individually
			for( auto& t: p.triangles )
			{
				split( t );
			}
		//}
	}
	
	void Polygon::split( ci::vec3 t0, ci::vec3 t1, ci::vec3 t2 )
	{
		Polygon p( t0, t1, t2 );
		
		split( p );
	}
	
	bool Polygon::intersectRay( ci::vec3 rayOrigin, ci::vec3 rayDir, float epsilon, ci::vec3* intersection )
	{
		for(auto& t: triangles)
		{
			if( t.intersectRay( rayOrigin, rayDir, epsilon, intersection ) )
			{
				return true;
			}
		}
		
		return false;
	}
	
	void Polygon::classifyRay( Polygon& p, int& intersectionCount )
	{
		ci::vec3 rayDir = getNormal();
		for(auto& t: triangles)
		{
			ci::vec3 rayOrigin = t.getCenter();
			
			if( p.intersectRay( rayOrigin, rayDir, EPSILON, NULL ) )
			{
				intersectionCount++;
			}
		}
	}
	
	void Polygon::classify( vector<Polygon>& polygons )
	{
		ci::vec3 rayDir = getNormal();
		for(auto& t: triangles)
		{	
			ci::vec3 rayOrigin = t.getCenter();
			
			int intersectionCount = 0;
			
			for( auto& p: polygons )
			{
				if( p.intersectRay( rayOrigin, rayDir, EPSILON, NULL ) )
				{
					intersectionCount++;
				}
			}
			
			t.classification = intersectionCount % 2 ? ciCSG::BACK : ciCSG::FRONT;
		}
	}
	
	void Polygon::draw(bool useNormalForColor )
	{
		for(auto& t: triangles)
		{
			t.draw(useNormalForColor);
		}
	}
	
}
