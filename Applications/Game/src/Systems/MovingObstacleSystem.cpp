#include <Systems/MovingObstacleSystem.hpp>
#include <Components/ObstacleComponent.hpp>
#include <ECS/components/transform-component.hpp>

using namespace ECS;
using namespace Utilities;

void MovingObstacleSystem::Update(float deltaTime)
{
	auto pairs = GetWorld()->GetComponents<ObstacleComponent, TransformComponent>();
	for(auto& pair : pairs)
	{
		TransformComponent* transform = pair.second.second;
		transform->Position.x -= MoveSpeed * deltaTime;

		if(transform->Position.x < (transform->Scale.x * -2.5f))
			GetWorld()->DestroyEntity(pair.first);
	}
}