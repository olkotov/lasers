// Oleg Kotov

#include "prop_laser_reflector.h"
#include "spherecomponent.h"
#include "staticmeshcomponent.h"
#include "cameracomponent.h"
#include "inputcomponent.h"
#include "inputsystem.h"
#include "okoengine.h"

LINK_ENTITY_TO_CLASS( prop_laser_reflector, LaserReflector )

#define RAY_MATERIAL "materials/colors/red_unlit.mat"
#define BASE_MATERIAL "materials/colors/cyan.mat"
#define EMITTER_MATERIAL "materials/colors/gray.mat"

LaserReflector::LaserReflector()
{
	m_collisionComponent = createComponent<SphereComponent>( "receiver_volume" );

	setRootComponent( m_collisionComponent );

	m_baseMeshComponent = createComponent<StaticMeshComponent>();
	m_baseMeshComponent->setMesh( "models/primitives/sphere.json" );
	m_baseMeshComponent->setMaterial( BASE_MATERIAL );
	m_baseMeshComponent->attachTo( m_collisionComponent );

	m_emitterMeshComponent = createComponent<StaticMeshComponent>();
	m_emitterMeshComponent->setMesh( "models/laser/laser.json" );
	m_emitterMeshComponent->setMaterial( EMITTER_MATERIAL );
	m_emitterMeshComponent->setRelativePosition( Vector::forward * 0.45f );
	m_emitterMeshComponent->attachTo( m_collisionComponent );

	m_rayMeshComponent = createComponent<StaticMeshComponent>();
	m_rayMeshComponent->setMesh( "models/laser/laser_ray.json" );
	m_rayMeshComponent->setMaterial( RAY_MATERIAL );
	m_rayMeshComponent->setRelativeScale( Vector( 0.12f, 0.0f, 0.12f ) );
	m_rayMeshComponent->attachTo( m_emitterMeshComponent );

	m_cameraComponent = createComponent<CameraComponent>();
	m_cameraComponent->setBackgroundColor( Color( 24, 24, 24 ) );
	m_cameraComponent->setFov( 45.0f );
	m_cameraComponent->setNearClip( 1.0f );
	m_cameraComponent->setFarClip( 50.0f );
	m_cameraComponent->setWorldEulerAngles( Vector( -50.0f, 0.0f, 0.0f ) );

	setupInputComponent();

	possessedEvent += std::bind( &LaserReflector::possess, this, std::placeholders::_1 );
	unpossessedEvent += std::bind( &LaserReflector::unpossess, this, std::placeholders::_1 );
}

void LaserReflector::initializeInputBindings() const
{
	auto inputSystem = engine()->inputSystem();

	inputSystem->addAxisMapping( "Reflector_MoveForward", Input::Key::W, 1.0f );
	inputSystem->addAxisMapping( "Reflector_MoveForward", Input::Key::S, -1.0f );
	inputSystem->addAxisMapping( "Reflector_MoveRight", Input::Key::D, 1.0f );
	inputSystem->addAxisMapping( "Reflector_MoveRight", Input::Key::A, -1.0f );
	inputSystem->addAxisMapping( "Reflector_TurnAt", Input::Key::MouseX, -1.0f );

	inputSystem->addActionMapping( "Reflector_Sprint", Input::Key::LeftShift );
}

void LaserReflector::setupInputComponent()
{
	initializeInputBindings();

	m_inputComponent = createComponent<InputComponent>();

	m_inputComponent->bindAxis( "Reflector_MoveForward", this, &LaserReflector::moveForward );
	m_inputComponent->bindAxis( "Reflector_MoveRight", this, &LaserReflector::moveRight );
	m_inputComponent->bindAxis( "Reflector_TurnAt", this, &LaserReflector::turnAt );

	m_inputComponent->bindAction( "Reflector_Sprint", Input::EventType::Pressed, this, &LaserReflector::sprintStart );
	m_inputComponent->bindAction( "Reflector_Sprint", Input::EventType::Released, this, &LaserReflector::sprintEnd );

	m_inputComponent->setEnabled( false );
}

void LaserReflector::beginPlay()
{
	Super::beginPlay();

	Vector rotation = getRelativeEulerAngles();
	m_yaw = rotation.z;
}

void LaserReflector::tick( float deltaTime )
{
	Super::tick( deltaTime );

	move();
	rotate();

	updateCameraPosition();

	if ( m_suppliers.size() == 0 )
	{
		m_rayMeshComponent->setRelativeScale( Vector( 0.12f, 0.0f, 0.12f ) );
		return;
	}

	float rayLength = emitLaserRay( m_rayMeshComponent->getWorldPosition() );
	m_rayMeshComponent->setRelativeScale( Vector( 0.12f, rayLength, 0.12f ) );
}

void LaserReflector::updateCameraPosition() const
{
	Vector lookAtPosition = getWorldPosition();

	Quaternion cameraRotation = m_cameraComponent->getRelativeRotation();
	Vector cameraBackVector = cameraRotation.transformVector( Vector::back );
	float cameraArmLength = 18.0f;

	Vector cameraPosition = lookAtPosition + cameraBackVector * cameraArmLength;

	m_cameraComponent->setWorldPosition( cameraPosition );
}

void LaserReflector::moveForward( float value )
{
	m_movementDirection += Vector::forward * value;
}

void LaserReflector::moveRight( float value )
{
	m_movementDirection += Vector::right * value;
}

void LaserReflector::turnAt( float value )
{
	m_yaw += m_rotationSpeed * value;
}

void LaserReflector::sprintStart()
{
	m_movementSpeed = m_sprintSpeed;
}

void LaserReflector::sprintEnd()
{
	m_movementSpeed = m_normalSpeed;
}

void LaserReflector::move()
{
	m_movementDirection.normalize();
	float distance = m_movementSpeed * getDeltaTime();
	Vector movementDelta = m_movementDirection * distance;
	m_movementDirection = Vector::zero;

	translateLocal( movementDelta );
}

void LaserReflector::rotate()
{
	if ( m_yaw < -180.0f ) m_yaw += 360.0f;
	if ( m_yaw > 180.0f ) m_yaw -= 360.0f;

	setRelativeEulerAngles( Vector( 0.0f, 0.0f, m_yaw ) );
}

void LaserReflector::unpower( LaserNode* supplier )
{
	auto it = std::find( m_suppliers.begin(), m_suppliers.end(), supplier );

	if ( it != m_suppliers.end() )
	{
		std::iter_swap( it, m_suppliers.end() - 1 );
		m_suppliers.pop_back();
	}

	if ( m_suppliers.size() == 0 )
	{
		unpowerRecipients();
	}
}

void LaserReflector::possess( Controller* newController )
{
	m_inputComponent->setEnabled( true );
	m_cameraComponent->activate();
}

void LaserReflector::unpossess( Controller* oldController )
{
	m_inputComponent->setEnabled( false );
}

