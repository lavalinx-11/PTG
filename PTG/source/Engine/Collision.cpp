#include "Engine/Collision.h"

bool collIgnore;

bool COLLISION::SphereSphereCollisionDetected(const Body* body1, const Body* body2) {
	Vec3 dist = body1->pos - body2->pos;
	Vec3 normal = VMath::normalize(dist);
	float veloDot = VMath::dot((body1->vel - body2->vel), normal);
	if (veloDot > 0) {
		return false;
	}
	else if (VMath::distance(body1->pos, body2->pos) < body1->rad + body2->rad) {
		std::cout << collIgnore << std::endl;
		return true;

	}
	else {
		return false;
	}
	
}

void COLLISION::SphereSphereCollisionResponse(Body* body1, Body* body2) {
		Vec3 dist = body1->pos - body2->pos;
		Vec3 normal = VMath::normalize(dist);
		//float veloDot = VMath::dot((body1->vel - body2->vel), normal);
		Vec3 velo = body1->vel - body2->vel;
		float Vrel = VMath::dot(velo, normal);
		std::cout << Vrel << std::endl;
		float bouncy = 1.0f;
		float J;
		J = (-(1 + bouncy) * Vrel) / ((1 / body1->mass) + (1 / body2->mass));
		
		body1->vel = body1->vel + ((J * normal) / body1->mass);
		body2->vel = body2->vel - ((J * normal) / body2->mass);
}

bool COLLISION::DoCollision(Body* body1, Body* body2) {
	Vec3 dist = body1->pos - body2->pos;
	Vec3 normal = VMath::normalize(dist);
	float veloDot = VMath::dot((body1->vel - body2->vel), normal);
	if (veloDot < 0) {
		return false;
	}
	else  {
		return true;
	}
}
