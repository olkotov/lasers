// Oleg Kotov

#pragma once

#include "laser_node.h"
#include "event.h"

class ChargeableLaserReceiver : public LaserNode
{
	using Super = LaserNode;

	using PoweredEvent = Event<>;
	using UnpoweredEvent = Event<>;

	using ChargedEvent = Event<>;
	using DischargedEvent = Event<>;

public:

	ChargeableLaserReceiver();

	void power( LaserNode* supplier ) override;
	void unpower( LaserNode* supplier ) override;

private:

	int8_t calculateProgress( float ratio, int8_t totalSegments ) const;
	void updateIndicator( float value ) const;

private:

	class BoxComponent* m_baseCollisionComponent = nullptr;
	class SphereComponent* m_receiverCollisionComponent = nullptr;

	class StaticMeshComponent* m_baseMeshComponent = nullptr;
	class StaticMeshComponent* m_receiverMeshComponent = nullptr;
	class StaticMeshComponent* m_progressMeshComponent = nullptr;

	class TimelineComponent* m_chargeTimeline = nullptr;

	int8_t m_segmentCount = 14;
	bool m_charged = false;

public:

	PoweredEvent poweredEvent;
	UnpoweredEvent unpoweredEvent;
	ChargedEvent chargedEvent;
	DischargedEvent dischargedEvent;
};

