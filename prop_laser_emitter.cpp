// Oleg Kotov

#include "prop_laser_emitter.h"
#include "boxcomponent.h"
#include "staticmeshcomponent.h"

LINK_ENTITY_TO_CLASS( prop_laser_emitter, LaserEmitter )

#define BASE_MESH_MATERIAL "materials/colors/orange.mat"
#define EMITTER_MATERIAL "materials/colors/gray.mat"
#define RAY_MATERIAL "materials/colors/red_unlit.mat"

LaserEmitter::LaserEmitter()
{
	m_baseMeshComponent = createComponent<StaticMeshComponent>();
	m_baseMeshComponent->setMesh( "models/primitives/cube.json" );
	m_baseMeshComponent->setMaterial( BASE_MESH_MATERIAL );

	setRootComponent( m_baseMeshComponent );

	m_collisionComponent = createComponent<BoxComponent>();
	m_collisionComponent->attachTo( m_baseMeshComponent );

	m_emitterMeshComponent = createComponent<StaticMeshComponent>();
	m_emitterMeshComponent->setMesh( "models/laser/laser.json" );
	m_emitterMeshComponent->setMaterial( EMITTER_MATERIAL );
	m_emitterMeshComponent->setRelativePosition( Vector::forward * 0.5f );
	m_emitterMeshComponent->attachTo( m_baseMeshComponent );

	m_rayMeshComponent = createComponent<StaticMeshComponent>();
	m_rayMeshComponent->setMesh( "models/laser/laser_ray.json" );
	m_rayMeshComponent->setMaterial( RAY_MATERIAL );
	m_rayMeshComponent->setRelativeScale( Vector( 0.12f, 0.0f, 0.12f ) );
	m_rayMeshComponent->attachTo( m_emitterMeshComponent );
}

void LaserEmitter::tick( float deltaTime )
{
	Super::tick( deltaTime );

	if ( !m_activated )
	{
		m_rayMeshComponent->setRelativeScale( Vector( 0.12f, 0.0f, 0.12f ) );
		return;
	}

	float rayLength = emitLaserRay( m_rayMeshComponent->getWorldPosition() );
	m_rayMeshComponent->setRelativeScale( Vector( 0.12f, rayLength, 0.12f ) );	
}

void LaserEmitter::activate()
{
	m_activated = true;
}

void LaserEmitter::deactivate()
{
	m_activated = false;
	unpowerRecipients();
}

