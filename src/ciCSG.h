//
//  ciCSG.h
//
//  Created by lars berg on 3/3/15.
//

#pragma once

#include "cinder/app/App.h"
#include "cinder/Log.h"
#include "cinder/PolyLine.h"

#include <CSG/Polygon.h>

namespace ciCSG
{
	static void addPolygonsToMesh(ci::TriMesh& m, vector<ciCSG::Polygon>& polygons)
	{
		for(auto& p: polygons)
		{
			for(auto& t: p.triangles)
			{
				if(t.classification == FRONT)
				{
					//m.addVertex( t.a );
					//m.addVertex( t.b );
					//m.addVertex( t.c );
					m.appendPosition(t.a);
					m.appendPosition(t.b);
					m.appendPosition(t.c);
				}
			}
		}
	}
	
	static vector<ciCSG::Polygon> meshToPolygons(ci::TriMesh& m)
	{
		vector<ciCSG::Polygon> polygons;
		
		auto indices = m.getIndices();
		//auto v = m.getVertices();
		auto v = m.getPositions<3>();
		size_t numPos = m.getNumVertices();
		
		if(indices.size())
		{
			for(int i=0; i<indices.size(); i+=3)
			{
				polygons.push_back( ciCSG::Polygon( v[ indices[i] ], v[ indices[i+1] ], v[ indices[i+2] ] ) );
			}
		}
		else
		{
			for(int i=0; i<numPos; i+=3)
			{
				polygons.push_back( ciCSG::Polygon( v[i], v[i+1], v[i+2] ) );
			}
		}
		
		
		return polygons;
	}
	
	static std::vector<ci::PolyLine3f> polygonsToPolylines( std::vector<Polygon>& polygons )
	{
		std::vector<ci::PolyLine3f> polylines;
		for(auto& p: polygons)
		{
			auto pl = p.toPolylines();
			
			polylines.insert(polylines.end(), pl.begin(), pl.end() );
		}
		
		return polylines;
	}
	
	
	static void meshBoolean( ci::TriMesh& a, ci::TriMesh& b, ci::TriMesh& m, bool flipA, bool flipB)
	{
		
		// get our polygons
		auto polygonsA = meshToPolygons( a );
		auto polygonsB = meshToPolygons( b );
		
		auto orig_polygonsA = polygonsA;
		auto orig_polygonsB = polygonsB;
		
		auto startTime = ci::app::getElapsedSeconds();
		
		//split the polygons with eachother
		int rayIntersectionCount = 0;
		for(int i=0; i<polygonsA.size(); i++)
		{
			auto& pa = polygonsA[i];
			rayIntersectionCount = 0;
			
			for( auto& pb: polygonsB )
			{
				if( pa.bb.intersects( pb.bb ))
				{
					pa.split( pb );
					
					pb.splitters.push_back( &orig_polygonsA[i] );
				}
				
				
				//classify FRONT||BACK where we can
				if(pa.triangles.size() == 1)
				{
					pa.classifyRay( pb, pa.rayIntersectionCount );
				}
				
				pb.classifyRay( orig_polygonsA[i], pb.rayIntersectionCount );
			}
		}
		
		for(auto& pb: polygonsB)
		{
			for( auto& pa: pb.splitters )
			{
				if( pa->bb.intersects( pb.bb ))
				{
					pb.split( *pa );
				}
			}
			
			pb.wasSplit = pb.triangles.size() > 1;
		}
		
		CI_LOG_I( "ciCSG::meshBoolean", "split time: " + to_string((getElapsedSeconds() - startTime)) );
		
		//classy the triangles
		startTime = ci::app::getElapsedSeconds();
		ci::vec3 rayDir(0,1,0);

		for(auto& p: polygonsA)
		{
			if(p.triangles.size() > 1)
			{
				p.classify( orig_polygonsB );
			}
			else
			{
				p.setClassification( p.rayIntersectionCount % 2 ? BACK : FRONT );
			}
		}

		for(auto& p: polygonsB)
		{
			if(p.triangles.size() > 1)
			{
				p.classify( orig_polygonsA );
			}
			else
			{
				p.setClassification( p.rayIntersectionCount % 2 ? BACK : FRONT );
			}
		}
		CI_LOG_I( "ciCSG::meshBoolean", "classify time: " + to_string((ci::app::getElapsedSeconds() - startTime)) );
		
		//flip em
		if(flipA == true) {
			for(auto& p: polygonsA)	p.flip();
		}
		
		if(flipB){
			for(auto& p: polygonsB)	p.flip();
		}
		
		startTime = ci::app::getElapsedSeconds();
		//add the polygons to out outMesh
		m.clear();
		addPolygonsToMesh( m, polygonsA );
		addPolygonsToMesh( m, polygonsB );
		CI_LOG_I( "ciCSG::meshBoolean", "create mesh time: " + to_string((ci::app::getElapsedSeconds() - startTime)) );
	}
	
	static void meshUnion( ci::TriMesh& a, ci::TriMesh& b, ci::TriMesh& outMesh )
	{
		meshBoolean( a, b, outMesh, false, false );
	}
	
	static void meshIntersection(ci::TriMesh& a, ci::TriMesh& b, ci::TriMesh& outMesh )
	{
		meshBoolean( a, b, outMesh, true, true );
	}
	
	static void meshDifference(ci::TriMesh& a, ci::TriMesh& b, ci::TriMesh& outMesh )
	{
		meshBoolean( a, b, outMesh, false, true );
	}
}