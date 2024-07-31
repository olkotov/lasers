// Oleg Kotov

#pragma once

#include "pawn.h"
#include "gameplay.h"
#include "hitresult.h"
#include "primitivecomponent.h"

class LaserNode : public Pawn
{
public:

	virtual ~LaserNode() = default;

	float emitLaserRay( const Vector& startPosition = Vector::zero, float rayLength = 100.0f )
	{
		Vector start = ( startPosition.lengthSquared() == 0.0f ) ? getWorldPosition() : startPosition;
		Vector direction = getForwardDirection();
		Vector end = start + direction * rayLength;

		std::vector<HitResult> hits;
		raycastMultiple( hits, start, end );

		// sort by distance
		std::sort( hits.begin(), hits.end(), []( const HitResult& a, const HitResult& b )
		{
			return a.distance < b.distance;
		});

		std::set<LaserNode*> recipientsThisFrame;

		for ( const auto& hit : hits )
		{
			// obstacle
			if ( hit.component->getName() != "receiver_volume" )
			{
				rayLength = hit.distance;
				break;
			}

			// "opaque" node
			bool isOpaqueNode = 
				hit.entity->isA( "prop_laser_reflector" ) || 
				hit.entity->isA( "prop_laser_receiver" ) || 
				hit.entity->isA( "prop_laser_receiver_chargeable" );

			if ( isOpaqueNode )
			{
				auto node = static_cast<LaserNode*>( hit.entity );

				// parent can't be child
				if ( !isRecipientOf( node ) )
				{
					recipientsThisFrame.insert( static_cast<LaserNode*>( hit.entity ) );
				}

				rayLength = hit.distance;
				break;
			}

			// "transparent" node
			bool isTransparentNode = hit.entity->isA( "prop_laser_relay" );

			if ( isTransparentNode )
			{
				recipientsThisFrame.insert( static_cast<LaserNode*>( hit.entity ) );
				continue;
			}
		}

		// power and unpower recipients

		auto removedRecipients = set_diff( m_recipients, recipientsThisFrame );
		auto addedRecipients = set_diff( recipientsThisFrame, m_recipients );
		
		m_recipients = recipientsThisFrame;

		for ( const auto& recipient : removedRecipients )
		{
			recipient->unpower( this );
		}

		for ( const auto& recipient : addedRecipients )
		{
			recipient->power( this );
		}

		return rayLength;
	}

	virtual void power( LaserNode* supplier )
	{
		m_suppliers.push_back( supplier );
	}

	virtual void unpower( LaserNode* supplier )
	{
		auto it = std::find( m_suppliers.begin(), m_suppliers.end(), supplier );

		if ( it != m_suppliers.end() )
		{
			std::iter_swap( it, m_suppliers.end() - 1 );
			m_suppliers.pop_back();
		}

		unpowerRecipients();
	}

	void unpowerRecipients()
	{
		for ( const auto& recipient : m_recipients )
		{
			recipient->unpower( this );
		}

		m_recipients.clear();
	}

	virtual bool isRecipientOf( LaserNode* possibleSupplier ) const
	{
		for ( const auto& supplier : m_suppliers )
		{
			if ( supplier == possibleSupplier ) return true;
			
			if ( supplier->isRecipientOf( possibleSupplier ) )
			{
				return true;
			}
		}

		return false;
	}

public:

	std::vector<LaserNode*> m_suppliers; // parents
	std::set<LaserNode*> m_recipients; // children
};

