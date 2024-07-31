// Oleg Kotov

#pragma once

#include "laser_node.h"

class Controller;

class LaserReflector : public LaserNode
{
	using Super = LaserNode;

public:

	LaserReflector();

	void beginPlay() override;
	void tick( float deltaTime ) override;

	void initializeInputBindings() const;
	void setupInputComponent();

	void updateCameraPosition() const;

	void moveForward( float value );
	void moveRight( float value );
	void turnAt( float value );

	void sprintStart();
	void sprintEnd();

	void move();
	void rotate();

	void unpower( LaserNode* supplier ) override;

	// pawn
	void possess( Controller* newController );
	void unpossess( Controller* oldController );

private:

	class SphereComponent* m_collisionComponent = nullptr;
	class CameraComponent* m_cameraComponent = nullptr;
	class InputComponent* m_inputComponent = nullptr;

	class StaticMeshComponent* m_baseMeshComponent = nullptr;
	class StaticMeshComponent* m_emitterMeshComponent = nullptr;
	class StaticMeshComponent* m_rayMeshComponent = nullptr;

	float m_normalSpeed = 3.0f;
	float m_sprintSpeed = 6.0f;
	float m_movementSpeed = m_normalSpeed;
	float m_rotationSpeed = 0.1f;

	Vector m_movementDirection;
	float m_yaw = 0.0f; // z
};

