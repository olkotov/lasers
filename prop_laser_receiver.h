// Oleg Kotov

#pragma once

#include "laser_node.h"
#include "event.h"

class LaserReceiver : public LaserNode
{
	using Super = LaserNode;

	using PoweredEvent = Event<>;
	using UnpoweredEvent = Event<>;

public:

	LaserReceiver();

	void power( LaserNode* supplier ) override;
	void unpower( LaserNode* supplier ) override;

private:

	class BoxComponent* m_baseCollisionComponent = nullptr;
	class BoxComponent* m_receiverCollisionComponent = nullptr;

	class StaticMeshComponent* m_baseMeshComponent = nullptr;
	class StaticMeshComponent* m_receiverMeshComponent = nullptr;

public:

	PoweredEvent poweredEvent;
	UnpoweredEvent unpoweredEvent;
};

