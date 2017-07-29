#include <algorithm>
#include "PlayerUpdateSystem.hpp"
#include "World.hpp"
#include "TransformComponent.hpp"
#include "InputWorldComponent.hpp"
#include "PlayerControllerComponent.hpp"
#include "Rigidbody2DComponent.hpp"
#include "PowerupSystem.hpp"
#include "Timer.hpp"
#include "Physics2DWorldComponent.hpp"

using namespace Poly;

namespace SGJ
{
	void PlayerUpdateSystem::Update(World* world)
	{
		double deltaTime = TimeSystem::GetTimerDeltaTime(world, Poly::eEngineTimer::GAMEPLAY);
		double time = TimeSystem::GetTimerElapsedTime(world, Poly::eEngineTimer::GAMEPLAY);

		for (auto playerTuple : world->IterateComponents<PlayerControllerComponent, RigidBody2DComponent>())
		{
			RigidBody2DComponent* rigidbodyCmp = std::get<RigidBody2DComponent*>(playerTuple);
			PlayerControllerComponent* playerCmp = std::get<PlayerControllerComponent*>(playerTuple);

			Vector move(0, 0, 0);
			if (world->GetWorldComponent<InputWorldComponent>()->IsPressed(eKey::KEY_A) || world->GetWorldComponent<InputWorldComponent>()->IsPressed(eKey::LEFT))
				move -= Vector::UNIT_X;
			if (world->GetWorldComponent<InputWorldComponent>()->IsPressed(eKey::KEY_D) || world->GetWorldComponent<InputWorldComponent>()->IsPressed(eKey::RIGHT))
				move += Vector::UNIT_X;

			if (move.X < 0.5 && move.X > -0.5)
			{
				playerCmp->SetMoveVector(playerCmp->GetMoveVector() * -0.9);
			}
			else
			{
				move.X *= deltaTime * playerCmp->GetMovementSpeed();
				playerCmp->SetMoveVector(move);
			}

			if (world->GetWorldComponent<InputWorldComponent>()->IsPressed(eKey::SPACE) && playerCmp->AllowJump)
			{
				playerCmp->AllowJump = false;
				playerCmp->LastJumpTimeStart = time;
				Vector jump(0, 0, 0);
				jump.Y = playerCmp->GetJumpForce();
				rigidbodyCmp->ApplyImpulseToCenter(jump);
			}

			try
			{
				Physics2DWorldComponent* physicsWorldComponent = world->GetWorldComponent<Physics2DWorldComponent>();
				for (Physics2DWorldComponent::Collision col : physicsWorldComponent->GetCollidingBodies(rigidbodyCmp))
				{
					if (col.Normal.Dot(Vector::UNIT_Y) < -0.7)
					{
						playerCmp->LastAllowJumpChecked = 0;
						if (!playerCmp->AllowJump)
						{
							playerCmp->LastLandTimeStart = time;
						}
						playerCmp->AllowJump = true;
						break;
					}
				}
			}
			catch (std::exception)
			{

			}


			playerCmp->LastAllowJumpChecked += deltaTime;
			if (playerCmp->LastAllowJumpChecked > 0.5)
				playerCmp->AllowJump = false;


			PowerupSystem::ApplyPowerupsAndInput(world, playerCmp);

			TransformComponent* playerTrans = playerCmp->GetSibling<TransformComponent>();
			float timeSinceLastJump = time - playerCmp->LastJumpTimeStart;
			float timeSinceLastLand = time - playerCmp->LastLandTimeStart;

			if (timeSinceLastLand < timeSinceLastJump )
			{
				// stretch on jump anim
				float tX = 1.0f * timeSinceLastJump;
				float tY = 1.0f * timeSinceLastJump;
				float scaleX = Lerp(2.5f, 1.0f, Clamp(ElasticEaseOut(tX), 0.0f, 1.0f));
				float scaleY = Lerp(0.3f, 1.0f, Clamp(ElasticEaseOut(tY), 0.0f, 1.0f));
				playerTrans->SetLocalScale(playerTrans->GetGlobalRotation().GetConjugated() * Vector(scaleX, scaleY, 1.0f));
			}
			else
			{
				// stretch on jump anim
				float tX = 0.75f * timeSinceLastJump;
				float tY = 0.5f * timeSinceLastJump;
				float scaleX = Lerp(0.3f, 0.5f, Clamp(ElasticEaseOut(tX), 0.0f, 1.0f));
				float scaleY = Lerp(2.5f, 1.2f, Clamp(ElasticEaseOut(tY), 0.0f, 1.0f));
				playerTrans->SetLocalScale(playerTrans->GetGlobalRotation().GetConjugated() * Vector(scaleX, scaleY, 1.0f));
			}

		}
	}

	float PlayerUpdateSystem::ElasticEaseOut(float p)
	{
		return sin(-13 * 3.14 * (p + 1)) * pow(2, -10 * p) + 1;
	}

}
