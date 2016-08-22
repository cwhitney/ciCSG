//
//  ofxCSGUtils.h
//
//  Created by lars berg on 3/3/15.
//

#pragma once

#include "cinder/app/App.h"

using namespace std;

namespace ciCSG
{
	
	//STATIC VARS
	static float EPSILON = 1e-5;
	static float ONE_PLUS_EPSILON = EPSILON + 1;
	static float NEG_EPSILON = -EPSILON;
	
	enum Classification
	{
		UNDEFINED = 0,
		SPANNING = 1,
		FRONT = 2,
		BACK = 3,
		COPLANAR = 4
	};

	//STATIC METHODS
	template<class T>
	static T lerp(T a, T b, float k)
	{
		return a + (b - a) * k;
	}
	
	template<class T>
	static void appendVectors( vector<T>& a, vector<T>& b )
	{
		//a.reserve( a.size() + b.size() );
		a.insert( a.end(), b.begin(), b.end() );
	}
	
	static ci::vec3 normalFromPoints(ci::vec3 p0, ci::vec3 p1, ci::vec3 p2)
	{
		//return (p2 - p1).cross(p0 - p1).normalize();
		return glm::normalize(glm::cross(p2 - p1, p0 - p1));
	}
	
	static float areaOfTriangle(ci::vec3 p0, ci::vec3 p1, ci::vec3 p2)
	{
		//return (p2 - p1).cross(p0 - p1).length() * .5;
		return glm::length(glm::cross(p2 - p1, p0 - p1)) * 0.5f;
	}
	
	static float areaOfTriangleSquared(ci::vec3 p0, ci::vec3 p1, ci::vec3 p2)
	{
		//return (p2 - p1).cross(p0 - p1).lengthSquared() * .5;
		return glm::length2(glm::cross(p2 - p1, p0 - p1)) * 0.5f;
	}

	static float signedDistanceToPlane(ci::vec3 point, ci::vec3 planePos, ci::vec3 planeNormal)
	{
		//float dist = planeNormal.dot(point - planePos);
		float dist = glm::dot(planeNormal, point - planePos);
		return dist;
	}
	
	//http://geomalgorithms.com/a04-_planes.html
	static float distanceToPlane(ci::vec3 point, ci::vec3 planePos, ci::vec3 planeNormal)
	{
		float sb, sn, sd;
		
		//sn = -( planeNormal.dot(point - planePos) );
		//sd = planeNormal.dot(planeNormal);
		sn = -( glm::dot(planeNormal, point - planePos) );
		sd = glm::dot(planeNormal, planeNormal);
		
		sb = sn / sd;
		
		ci::vec3 B = point + sb * planeNormal;
		
		//return point.distance(B);
		return glm::distance(point, B);
	}
	
	static float distanceToPlaneSigned(ci::vec3 point, ci::vec3 planePos, ci::vec3 planeNormal)
	{
		//assumes planeNormal is a unit vector
		//return -(planeNormal.dot(point - planePos));
		return -( glm::dot(planeNormal, point - planePos));
		//	return -( doubleDot( planeNormal, point - planePos ) );
	}
	
	static Classification classifyPointWithPlane( ci::vec3 point, ci::vec3 planeNormal, float w )
	{
		//float t = planeNormal.dot(point) - w;
		float t = glm::dot(planeNormal,  point ) - w;
		return ( t < NEG_EPSILON ) ? BACK : (t > EPSILON) ? FRONT : SPANNING;
	}
	
	static Classification classifyPointWithPlane( ci::vec3 point, ci::vec3 planePos, ci::vec3 planeNormal)
	{
		auto d = distanceToPlaneSigned( point, planePos, planeNormal );
		
		if( d > EPSILON )	return BACK;
		else if( d < NEG_EPSILON )	return FRONT;
		
		return SPANNING;
	}
	
	//barycentric coords
	//http://www.blackpawn.com/texts/pointinpoly/
	static bool getBaryCentricCoords(ci::vec3 p, ci::vec3 t0, ci::vec3 t1, ci::vec3 t2, float &u, float &v, float& w)
	{
		// Compute vectors
		ci::vec3 v0 = t2 - t0;
		ci::vec3 v1 = t1 - t0;
		ci::vec3 v2 = p - t0;
		
		// Compute dot products
		float dot00 = glm::dot(v0, v0);// v0.dot(v0);
		float dot01 = glm::dot(v0, v1);// v0.dot( v1 );
		float dot02 = glm::dot(v0, v2);// v0.dot( v2 );
		float dot11 = glm::dot(v1, v1);// v1.dot( v1 );
		float dot12 = glm::dot(v1, v2);// v1.dot( v2 );
		
		float denom = (dot00 * dot11 - dot01 * dot01);
		
		if ( denom == 0 )
		{
			//TODO: what's the right thing to do here?
			u = v = w = 0;
			return false;
		}
		
		// Compute barycentric coordinates
		float invDenom = 1.f / denom;
		u = (dot11 * dot02 - dot01 * dot12) * invDenom;
		v = (dot00 * dot12 - dot01 * dot02) * invDenom;
		w = 1. - u - v;
		
		return true;
	}
	
	static bool getBaryCentricCoords(ci::vec3 p, ci::vec3 t0, ci::vec3 t1, ci::vec3 t2, float &u, float &v)
	{
		float w;
		return getBaryCentricCoords(p, t0, t1, t2, u, v, w);
	}
	
	static ci::vec3 closestPointOnLineSegment(ci::vec3 p, ci::vec3 l0, ci::vec3 l1)
	{
		ci::vec3 diff = p - l0;
		ci::vec3 dir = l1 - l0;
		//float u = diff.dot(dir) / dir.dot(dir);
		float u = glm::dot(diff, dir) / glm::dot(dir, dir);
		
		if ( u < 0. )	return l0;
		else if( u > 1. )	return l1;
		
		return l0 + dir * u;
	}
	
	
	//http://paulbourke.net/geometry/pointlineplane/lineline.c
	static bool LineLineIntersect( ci::vec3 p1,ci::vec3 p2,ci::vec3 p3,ci::vec3 p4, ci::vec3 *pa = NULL, ci::vec3 *pb = NULL )
	{
		ci::vec3 p13, p43, p21;
		double d1343,d4321,d1321,d4343,d2121;
		double numer,denom;
		
		p13 = p1 - p3;
		p43 = p4 - p3;
		
		if (abs(p43.x) < EPSILON && abs(p43.y) < EPSILON && abs(p43.z) < EPSILON)	return false;
		
		p21 = p2 - p1;
		
		if (abs(p21.x) < EPSILON && abs(p21.y) < EPSILON && abs(p21.z) < EPSILON)	return false;
		
		d1343 = p13.x * p43.x + p13.y * p43.y + p13.z * p43.z;
		d4321 = p43.x * p21.x + p43.y * p21.y + p43.z * p21.z;
		d1321 = p13.x * p21.x + p13.y * p21.y + p13.z * p21.z;
		d4343 = p43.x * p43.x + p43.y * p43.y + p43.z * p43.z;
		d2121 = p21.x * p21.x + p21.y * p21.y + p21.z * p21.z;
		
		denom = d2121 * d4343 - d4321 * d4321;
		if (abs(denom) < EPSILON)	   return false;
		
		numer = d1343 * d4321 - d1321 * d4343;
		
		double mua = numer / denom;
		double mub = (d1343 + d4321 * mua) / d4343;
		
		if( pa != NULL)
		{
			*pa = p1 + ci::vec3(mua) * p21;
		}
		if( pb != NULL )
		{
			*pb = p3 + ci::vec3(mub) * p43;
		}
		
		return true;
	}
	
	static float getLineSegmentUValue(ci::vec3 l0, ci::vec3 l1, ci::vec3 p)
	{
		ci::vec3 diff = p - l0;
		ci::vec3 dir = l1 - l0;
		
		if(l0 == l1)
		{
			return 0;
		}
		
		//return diff.dot(dir) / dir.dot(dir);
		return glm::dot(diff, dir) / glm::dot(dir, dir);
	}
	
	static bool isPointInLineSegment(ci::vec3 l0, ci::vec3 l1, ci::vec3 p)
	{
		float u = getLineSegmentUValue( l0, l1, p );
		return  u >= NEG_EPSILON && u <= ONE_PLUS_EPSILON;
	}
	
	static bool intersectLineSegments(ci::vec3 a0, ci::vec3 a1, ci::vec3 b0, ci::vec3 b1, ci::vec3* intersection=NULL)
	{
		ci::vec3 p;
		
		LineLineIntersect(a0, a1, b0, b1, &p);
		
		if( isPointInLineSegment(a0, a1, p) )
		{
			*intersection = p;
			return true;
		}
		
		return false;
	}
	
	static bool splitLineSegmentWithPlane( ci::vec3 l0, ci::vec3 l1, ci::vec3 planeNormal, float w, ci::vec3* intersection)
	{
		auto c0 = classifyPointWithPlane( l0, planeNormal, w);
		auto c1 = classifyPointWithPlane( l1, planeNormal, w);
		
		if( c0 != c1 )
		{
			//float k = (w - planeNormal.dot(l0)) / planeNormal.dot(l1 - l0);
			float k = (w - glm::dot(planeNormal, l0)) / glm::dot(planeNormal, l1 - l0 );
			
			*intersection = lerp( l0, l1, ci::clamp(k, 0.f, 1.f) ); // the clamp fixed some errors where k > 1
			
			return true;
		}
		
		return false;
	}
	
	static int intersectLineSegmentPlane(ci::vec3 p0, ci::vec3 p1, ci::vec3 planePos, ci::vec3 planeNormal, ci::vec3* intersection = NULL)
	{
		auto d0 = distanceToPlaneSigned( p0, planePos, planeNormal );
		auto d1 = distanceToPlaneSigned( p1, planePos, planeNormal );
		
		if( (d0 >= EPSILON && d1 >= EPSILON) || ( d0 <= NEG_EPSILON && d1 <= NEG_EPSILON ) )
//		if( (d0 > 0 && d1 > 0) || ( d0 < 0 && d1 < 0 ) )
		{
			//no intersection
			return 0;
		}
		if( d0 == 0 && d1 == 0 )
		{
			//it's coplanar
			if( intersection != NULL )
			{
				*intersection = p0;
			}
			return 2;
		}
		
		//it's a hit
		if( intersection != NULL )
		{
			//lerp using the distance to plane values
			*intersection = lerp( p0, p1, d0 / (d0 - d1) );
		}
		return 1;
	}
	
	
	static bool isPointInTriangle(ci::vec3 p, ci::vec3 a, ci::vec3 b, ci::vec3 c, ci::vec3 normal )
	{
		if( fabs( distanceToPlaneSigned( p, a, normal ) ) > EPSILON )	return false;
		
		float u, v, w, epsilon = NEG_EPSILON; // 0; // EPSILON; //
		
		if( getBaryCentricCoords( p, a, b, c, u, v, w ) )
		{
			return u > epsilon && v > epsilon && w > epsilon;
		}
		
		return false;
	}
	
	static bool isPointOnPlane( ci::vec3 p, ci::vec3 planeNormal, float w, float epsilon = EPSILON)
	{
		//float t = planeNormal.dot(p) - w;
		float t = glm::dot(planeNormal, p) - w;
		return abs(t) > epsilon;
	}
	
	static bool isPointInTriangle(ci::vec3 p, ci::vec3 a, ci::vec3 b, ci::vec3 c, ci::vec3 normal, float epsilon )
	{
		float u, v, w;
		
		if( getBaryCentricCoords( p, a, b, c, u, v, w ) )
		{
			return u > epsilon && v > epsilon && w > epsilon;
		}
		
		return false;
	}
	
	static bool isPointInTriangle(ci::vec3 p, ci::vec3 a, ci::vec3 b, ci::vec3 c)
	{
		return isPointInTriangle( p, a, b, c, normalFromPoints(a, b, c) );
	}
	
	
	//derived from Akira-Hayasaka's ofxRayTriangleIntersection
	//	https://github.com/Akira-Hayasaka/ofxRayTriangleIntersection/blob/master/src/ofxRayTriangleIntersection.h
	//	assume ray direction is normalized
	static bool intersectRayTriangle(ci::vec3 rayOrigin, ci::vec3 rayDir, ci::vec3 t0, ci::vec3 t1, ci::vec3 t2, ci::vec3* intersection=NULL)
	{
		//ci::vec3 normal = (t2 - t1).cross(t0 - t1).normalize();
		ci::vec3 normal = glm::normalize( glm::cross(t2 - t1,t0 - t1) );
		float vn = glm::dot(rayDir, normal);
		
		ci::vec3 diff = rayOrigin - t0;
		//float xpn = diff.dot(normal);
		float xpn = glm::dot(diff, normal);
		float distance = -xpn / vn;
		
		if (distance < 0) return false; // behind ray origin. fail
		
		ci::vec3 hitPos = rayDir * distance + rayOrigin;
		
		if(isPointInTriangle(hitPos, t0, t1, t2))
		{
			//it's a hit
			if(intersection!= NULL)
			{
				*intersection = hitPos;
			}
			return true;
		}
		
		//nada
		return false;
	}
}