// Oleg Kotov

#include "prop_laser_receiver.h"
#include "staticmeshcomponent.h"
#include "boxcomponent.h"

LINK_ENTITY_TO_CLASS( prop_laser_receiver, LaserReceiver )

#define BASE_MATERIAL "materials/colors/whitesmoke.mat"
#define POWERED_MATERIAL "materials/colors/orange.mat"
#define UNPOWERED_MATERIAL "materials/colors/gray.mat"

LaserReceiver::LaserReceiver()
{
	m_baseMeshComponent = createComponent<StaticMeshComponent>();
	m_baseMeshComponent->setMesh( "models/primitives/cube.json" );
	m_baseMeshComponent->setMaterial( BASE_MATERIAL );

	setRootComponent( m_baseMeshComponent );

	m_baseCollisionComponent = createComponent<BoxComponent>();
	m_baseCollisionComponent->attachTo( m_baseMeshComponent );

	m_receiverMeshComponent = createComponent<StaticMeshComponent>();
	m_receiverMeshComponent->setMesh( "models/laser/laser.json" );
	m_receiverMeshComponent->setMaterial( UNPOWERED_MATERIAL );
	m_receiverMeshComponent->setRelativePosition( Vector( Vector::forward * 0.5f ) );
	m_receiverMeshComponent->attachTo( m_baseMeshComponent );

	m_receiverCollisionComponent = createComponent<BoxComponent>( "receiver_volume" );
	m_receiverCollisionComponent->setBoxExtent( Vector( 0.10f, 0.10f, 0.10f ) );
	m_receiverCollisionComponent->attachTo( m_receiverMeshComponent );
}

void LaserReceiver::power( LaserNode* supplier )
{
	Super::power( supplier );

	if ( m_suppliers.size() == 1 )
	{
		m_receiverMeshComponent->setMaterial( POWERED_MATERIAL );
		poweredEvent();
	}
}

void LaserReceiver::unpower( LaserNode* supplier )
{
	Super::unpower( supplier );

	if ( m_suppliers.size() == 0 )
	{
		m_receiverMeshComponent->setMaterial( UNPOWERED_MATERIAL );
		unpoweredEvent();
	}
}

