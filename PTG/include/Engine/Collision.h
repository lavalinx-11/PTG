#pragma once
#include "Body.h"
// Instead of a class, let’s put all the collision equations in a namespace.
// I got this idea from the book "Beautiful C++: 30 Core Guidelines for Writing 
// Clean, Safe, and Fast Code" 

namespace COLLISION {
	
	// Checks if two spheres have collided and return true or false.  
	// We are assuming that all Body objects are spheres.  
	// I know it’s weird, we’ll fix it next course 
	// Notice the “const” keyword here.   
	// That is a promise to the compilar that we won’t change the Body objects.  
	// We are just detecting collisions after all 
	bool SphereSphereCollisionDetected(const Body* body1, const Body* body2);

	// If a collision has occurred, we need to immediately change the velocity 
	// of both spheres.  
	// We will definitely be changing the objects so no “const” keywords here  
	void SphereSphereCollisionResponse(Body* body1, Body* body2);

	bool DoCollision(Body* body1, Body* body2);
}