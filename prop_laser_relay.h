// Oleg Kotov

#pragma once

#include "laser_node.h"
#include "event.h"

class LaserRelay : public LaserNode
{
	using Super = LaserNode;

	using PoweredEvent = Event<>;
	using UnpoweredEvent = Event<>;

public:

	LaserRelay();

	void power( LaserNode* supplier ) override;
	void unpower( LaserNode* supplier ) override;

private:

	class CylinderComponent* m_collisionComponent = nullptr;
	class StaticMeshComponent* m_meshComponent = nullptr;

public:

	PoweredEvent poweredEvent;
	UnpoweredEvent unpoweredEvent;
};

