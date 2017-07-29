#include "Game.hpp"

#include <TimeWorldComponent.hpp>
#include <CameraComponent.hpp>
#include <TransformComponent.hpp>
#include <MeshRenderingComponent.hpp>
#include <FreeFloatMovementComponent.hpp>
#include <Physics2DColliders.hpp>
#include <Rigidbody2DComponent.hpp>
#include <PostprocessSettingsComponent.hpp>
#include <Core.hpp>
#include <DeferredTaskSystem.hpp>

#include "ViewportWorldComponent.hpp"
#include "GameManagerWorldComponent.hpp"
#include "GameManagerSystem.hpp"
#include "Level.hpp"
#include "GroundComponent.hpp"
#include "ObstacleComponent.hpp"
#include "PlayerControllerComponent.hpp"
#include "PlayerUpdateSystem.hpp"
#include "CameraMovementSystem.hpp"
#include "GameDebugWorldComponent.hpp"
#include "GameDebugSystem.hpp"

using namespace Poly;

DEFINE_GAME(SGJGame)

void SGJGame::Init()
{
	// Register Game Components
	gEngine->RegisterComponent<SGJ::GroundComponent>((int)eGameComponents::GROUND);
	gEngine->RegisterComponent<SGJ::ObstacleComponent>((int)eGameComponents::OBSTACLE);
	gEngine->RegisterComponent<SGJ::PlayerControllerComponent>((int)eGameComponents::PLAYER);
	gEngine->RegisterWorldComponent<SGJ::GameManagerWorldComponent>((int)eGameWorldComponents::GAME_MGR);
	gEngine->RegisterWorldComponent<SGJ::GameDebugWorldComponent>((int)eGameWorldComponents::GAME_DEBUG);
	

	gEngine->RegisterGameUpdatePhase(SGJ::GameManagerSystem::Update);
	gEngine->RegisterGameUpdatePhase(SGJ::PlayerUpdateSystem::Update);
	gEngine->RegisterGameUpdatePhase(SGJ::CameraMovementSystem::CameraMovementUpdatePhase);
	gEngine->RegisterGameUpdatePhase(SGJ::Debug::DebugSystemPhase);
	
	
	DeferredTaskSystem::AddWorldComponentImmediate<SGJ::GameManagerWorldComponent>(gEngine->GetWorld());
	DeferredTaskSystem::AddWorldComponentImmediate<SGJ::GameDebugWorldComponent>(gEngine->GetWorld());

	SGJ::GameManagerSystem::PrepareNonlevelObjects(gEngine->GetWorld());
		
	UniqueID Camera = gEngine->GetWorld()->GetWorldComponent<SGJ::GameManagerWorldComponent>()->Camera;
	gEngine->GetWorld()->GetWorldComponent<ViewportWorldComponent>()->SetCamera(0, gEngine->GetWorld()->GetComponent<Poly::CameraComponent>(Camera));
	DeferredTaskSystem::AddComponent<PostprocessSettingsComponent>(gEngine->GetWorld(), Camera);
	
	// load levels
	SGJ::GameManagerSystem::LoadLevel(gEngine->GetWorld(), "Levels/Level1.csv");
	SGJ::GameManagerSystem::SpawnLevel(gEngine->GetWorld(), 0);
};

void SGJGame::Deinit()
{
	SGJ::GameManagerSystem::Cleanup(gEngine->GetWorld());
}
