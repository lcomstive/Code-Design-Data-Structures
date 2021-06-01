#include <Systems/MovingObstacleSystem.hpp>
#include <Components/ObstacleComponent.hpp>

using namespace ECS;

void MovingObstacleSystem::Update(float deltaTime)
{
	auto pairs = world()->GetComponents<ObstacleComponent, TransformComponent>();
	for(auto& pair : pairs)
	{
		TransformComponent* transform = pair.second.second;
		transform->Position.x -= MoveSpeed * deltaTime;

		if(transform->Position.x < (transform->Scale.x * -2.5f))
			world()->DestroyEntity(pair.first);
	}
}