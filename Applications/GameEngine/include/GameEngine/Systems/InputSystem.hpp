#pragma once
#include <vector>
#include <raylib.h>
#include <functional>
#include <unordered_map>
#include <ECS/system.hpp>

#include <GameUtilities/data-stream.hpp>

enum class InputBindingState : short
{
	Down = 0x001,
	Held = 0x010,
	Up	 = 0x100,
	DownOrHeld = Held | Down,
	All	 = Down | Held | Up
};

class InputSystem : public ECS::System
{
public:
	void Init() override;
	void Update(float deltaTime) override;

	/*
	 * Binding Events (sent via MessageBus::eventBus)
	 *
	 * Keyboard Binding
	 * DataStream data
	 *		short currentState (InputBindingState)
	 *
	 * Gamepad Binding
	 * DataStream data
	 *		short currentState (InputBindingState)
	 *		char playerIndex
	 *
	 * Gamepad Axis Binding
	 * DataStream data
	 *		float value
	 *		char  playerIndex
	 */

	// Maps a key to trigger an event on change
	void Map(KeyboardKey key, std::string eventName, InputBindingState listenState = InputBindingState::All);
	void Map(KeyboardKey key, std::string eventName, std::function<void(Utilities::DataStream)> callback, InputBindingState listenState = InputBindingState::All);

	// Maps an axis to trigger an event on change
	void Map(GamepadAxis axis, std::string eventName);
	void Map(GamepadAxis axis, std::string eventName, std::function<void(Utilities::DataStream)> callback);

	// Maps a button to trigger an event on change
	void Map(GamepadButton button, std::string eventName, InputBindingState listenState = InputBindingState::All);
	void Map(GamepadButton button, std::string eventName, std::function<void(Utilities::DataStream)> callback, InputBindingState listenState = InputBindingState::All);

	// Maps a mouse button to trigger an event on change
	void Map(MouseButton button, std::string eventName, InputBindingState listenState = InputBindingState::All);
	void Map(MouseButton button, std::string eventName, std::function<void(Utilities::DataStream)> callback, InputBindingState listenState = InputBindingState::All);

	void MapMouseScroll(std::string eventName);
	void MapMouseScroll(std::string eventName, std::function<void(Utilities::DataStream)> callback);

	void UnmapMouseScroll(std::string& eventName);
	void Unmap(KeyboardKey key, std::string& eventName);
	void Unmap(GamepadAxis axis, std::string& eventName);
	void Unmap(GamepadButton button, std::string& eventName);
	void Unmap(MouseButton button, std::string& eventName);

private:
	std::vector<std::string> m_MouseScrollBindings;
	std::unordered_map<GamepadAxis,   std::vector<std::string>> m_GamepadAxisBindings;
	std::unordered_map<MouseButton,   std::unordered_map<std::string, InputBindingState>> m_MouseBindings;
	std::unordered_map<KeyboardKey,   std::unordered_map<std::string, InputBindingState>> m_KeyboardBindings;
	std::unordered_map<GamepadButton, std::unordered_map<std::string, InputBindingState>> m_GamepadBindings;
};
