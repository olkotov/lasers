// Oleg Kotov

#include "prop_laser_receiver_chargeable.h"
#include "staticmeshcomponent.h"
#include "boxcomponent.h"
#include "spherecomponent.h"
#include "timelinecomponent.h"

LINK_ENTITY_TO_CLASS( prop_laser_receiver_chargeable, ChargeableLaserReceiver )

#define BASE_MATERIAL "materials/colors/whitesmoke.mat"
#define POWERED_MATERIAL "materials/colors/blacksmoke.mat"
#define UNPOWERED_MATERIAL "materials/colors/gray_2.mat"

ChargeableLaserReceiver::ChargeableLaserReceiver()
{
	m_baseMeshComponent = createComponent<StaticMeshComponent>();
	m_baseMeshComponent->setMesh( "models/primitives/cube.json" );
	m_baseMeshComponent->setMaterial( BASE_MATERIAL );

	setRootComponent( m_baseMeshComponent );

	m_baseCollisionComponent = createComponent<BoxComponent>();
	m_baseCollisionComponent->attachTo( m_baseMeshComponent );

	m_receiverMeshComponent = createComponent<StaticMeshComponent>();
	m_receiverMeshComponent->setMesh( "models/primitives/sphere.json" );
	m_receiverMeshComponent->setMaterial( UNPOWERED_MATERIAL );
	m_receiverMeshComponent->setRelativePosition( Vector::forward * 0.525f );
	m_receiverMeshComponent->setRelativeScale( Vector( 0.2f, 0.2f, 0.2f ) );
	m_receiverMeshComponent->attachTo( m_baseMeshComponent );

	m_receiverCollisionComponent = createComponent<SphereComponent>( "receiver_volume" );
	m_receiverCollisionComponent->setSphereRadius( 0.1f );
	m_receiverCollisionComponent->attachTo( m_receiverMeshComponent );

	m_progressMeshComponent = createComponent<StaticMeshComponent>();
	m_progressMeshComponent->setMesh( "models/indicator/geometry.json" );

	for ( int8_t i = 0; i < m_segmentCount; ++i )
	{
		m_progressMeshComponent->setMaterial( UNPOWERED_MATERIAL, i );
	}

	m_progressMeshComponent->setRelativePosition( Vector::forward * 0.525f );
	m_progressMeshComponent->setRelativeEulerAngles( Vector( -90.0f, 180.0f, 0.0f ) );
	m_progressMeshComponent->attachTo( m_baseMeshComponent );

	m_chargeTimeline = createComponent<TimelineComponent>();
	m_chargeTimeline->setDuration( 3.0f );

	m_chargeTimeline->updateEvent += std::bind( &ChargeableLaserReceiver::updateIndicator, this, std::placeholders::_1 );

	m_chargeTimeline->finishedEvent += [&]()
	{
		m_receiverMeshComponent->setMaterial( POWERED_MATERIAL );
		m_charged = true;
		chargedEvent();
	};
}

void ChargeableLaserReceiver::power( LaserNode* supplier )
{
	Super::power( supplier );

	if ( m_suppliers.size() == 1 )
	{
		m_chargeTimeline->playFromStart();
		poweredEvent();
	}
}

void ChargeableLaserReceiver::unpower( LaserNode* supplier )
{
	Super::unpower( supplier );

	if ( m_suppliers.size() == 0 )
	{
		m_chargeTimeline->stop();
		updateIndicator( 0.0f );

		m_receiverMeshComponent->setMaterial( UNPOWERED_MATERIAL );
		unpoweredEvent();

		if ( m_charged )
		{
			m_charged = false;
			dischargedEvent();
		}
	}
}

int8_t ChargeableLaserReceiver::calculateProgress( float ratio, int8_t totalSegments ) const
{
	if ( ratio <= 0.0f )
	{
		return 0;
	}
	else if ( ratio > 1.0f )
	{
		return totalSegments;
	}
	else
	{
		return (int8_t)( ratio * totalSegments );
	}
}

void ChargeableLaserReceiver::updateIndicator( float value ) const
{
	int8_t activeSegments = calculateProgress( value, m_segmentCount );

	for ( int8_t i = 0; i < m_segmentCount; ++i )
	{
		if ( i < activeSegments )
		{
			m_progressMeshComponent->setMaterial( POWERED_MATERIAL, i );
		}
		else
		{
			m_progressMeshComponent->setMaterial( UNPOWERED_MATERIAL, i );
		}
	}
}

