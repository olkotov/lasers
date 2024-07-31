// Oleg Kotov

#pragma once

#include "laser_node.h"

class LaserEmitter : public LaserNode
{
	using Super = LaserNode;

public:

	LaserEmitter();

	void tick( float deltaTime ) override;

	void activate();
	void deactivate();

	void power( LaserNode* supplier ) override {}
	void unpower( LaserNode* supplier ) override {}

private:

	class BoxComponent* m_collisionComponent = nullptr;

	class StaticMeshComponent* m_baseMeshComponent = nullptr;
	class StaticMeshComponent* m_emitterMeshComponent = nullptr;
	class StaticMeshComponent* m_rayMeshComponent = nullptr;

	bool m_activated = false;
};

