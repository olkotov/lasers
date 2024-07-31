// Oleg Kotov

#include "prop_laser_relay.h"
#include "cylindercomponent.h"
#include "staticmeshcomponent.h"

LINK_ENTITY_TO_CLASS( prop_laser_relay, LaserRelay )

#define POWERED_MATERIAL "materials/colors/yellow.mat"
#define UNPOWERED_MATERIAL "materials/colors/gray.mat"

LaserRelay::LaserRelay()
{
	m_collisionComponent = createComponent<CylinderComponent>( "receiver_volume" );
	m_collisionComponent->setCylinderSize( 0.25f, 0.5f );

	setRootComponent( m_collisionComponent );

	m_meshComponent = createComponent<StaticMeshComponent>();
	m_meshComponent->setMesh( "models/primitives/cylinder.json" );
	m_meshComponent->setMaterial( UNPOWERED_MATERIAL );
	m_meshComponent->setRelativeScale( Vector( 0.5f, 0.5f, 0.5f ) );
	m_meshComponent->attachTo( m_collisionComponent );
}

void LaserRelay::power( LaserNode* supplier )
{
	Super::power( supplier );

	if ( m_suppliers.size() == 1 )
	{
		m_meshComponent->setMaterial( POWERED_MATERIAL );
		poweredEvent();
	}
}

void LaserRelay::unpower( LaserNode* supplier )
{
	Super::unpower( supplier );

	if ( m_suppliers.size() == 0 )
	{
		m_meshComponent->setMaterial( UNPOWERED_MATERIAL );
		unpoweredEvent();
	}
}

