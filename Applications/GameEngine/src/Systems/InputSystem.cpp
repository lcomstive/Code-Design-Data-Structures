#include <config.h> // Raylib
#include <algorithm>
#include <GameEngine/Systems/InputSystem.hpp>
#include <GameUtilities/messagebus.hpp>
#include <GameUtilities/inline-profiler.hpp>

using namespace std;
using namespace Utilities;

const float MouseScrollThreshold = 0.1f;
const float GamepadAxisThreshold = 0.1f;

void InputSystem::Init()
{
	GAME_LOG_DEBUG("Input system ready");
}

void InputSystem::Update(float deltaTime)
{
	PROFILE_FN();
	
	// --- KEYBOARD BINDINGS --- //
	for (auto& mapping : m_KeyboardBindings)
	{
		if (IsKeyDown(mapping.first))
			for (auto& eventData : mapping.second)
				if ((short)eventData.second & (short)InputBindingState::Held)
					MessageBus::eventBus()->Send(eventData.first, DataStream().write<short>((short)InputBindingState::Held));
		if (IsKeyPressed(mapping.first))
			for (auto& eventData : mapping.second)
				if ((short)eventData.second & (short)InputBindingState::Down)
					MessageBus::eventBus()->Send(eventData.first, DataStream().write<short>((short)InputBindingState::Down));
		if (IsKeyReleased(mapping.first))
			for (auto& eventData : mapping.second)
				if ((short)eventData.second & (short)InputBindingState::Up)
					MessageBus::eventBus()->Send(eventData.first, DataStream().write<short>((short)InputBindingState::Up));
	}

	for(auto& mapping : m_MouseBindings)
	{
		if(IsMouseButtonDown(mapping.first))
			for(auto& eventData : mapping.second)
				if(eventData.second == InputBindingState::Held || eventData.second == InputBindingState::All)
					MessageBus::eventBus()->Send(eventData.first, DataStream().write<short>((short)InputBindingState::Held));
		if(IsMouseButtonPressed(mapping.first))
			for(auto& eventData : mapping.second)
				if(eventData.second == InputBindingState::Down || eventData.second == InputBindingState::All)
					MessageBus::eventBus()->Send(eventData.first, DataStream().write<short>((short)InputBindingState::Down));
		if(IsMouseButtonReleased(mapping.first))
			for(auto& eventData : mapping.second)
				if(eventData.second == InputBindingState::Up || eventData.second == InputBindingState::All)
					MessageBus::eventBus()->Send(eventData.first, DataStream().write<short>((short)InputBindingState::Up));
	}

	float mouseWheelValue = GetMouseWheelMove();
	if(abs(mouseWheelValue) > MouseScrollThreshold)
		for (auto& eventName : m_MouseScrollBindings)
			MessageBus::eventBus()->Send(eventName, DataStream().write(mouseWheelValue));

	// MAX_GAMEPADS defined in Raylib's config.h
	for (char i = 0; i < MAX_GAMEPADS; i++)
	{
		// --- GAMEPAD BUTTON BINDINGS --- //
		for (auto& mapping : m_GamepadBindings)
		{
			if (IsGamepadButtonDown(i, mapping.first))
				for (auto& eventData : mapping.second)
					if (eventData.second == InputBindingState::Held || eventData.second == InputBindingState::All)
						MessageBus::eventBus()->Send(eventData.first, DataStream().write<short>((short)InputBindingState::Held)->write(i));
			if (IsGamepadButtonPressed(i, mapping.first))
				for (auto& eventData : mapping.second)
					if (eventData.second == InputBindingState::Down || eventData.second == InputBindingState::All)
						MessageBus::eventBus()->Send(eventData.first, DataStream().write<short>((short)InputBindingState::Down)->write(i));
			if (IsGamepadButtonReleased(i, mapping.first))
				for (auto& eventData : mapping.second)
					if (eventData.second == InputBindingState::Up || eventData.second == InputBindingState::All)
						MessageBus::eventBus()->Send(eventData.first, DataStream().write<short>((short)InputBindingState::Up)->write(i));
		}

		// --- GAMEPAD AXIS BINDINGS --- //
		for (auto& mapping : m_GamepadAxisBindings)
		{
			float value = GetGamepadAxisMovement(i, mapping.first);
			if(abs(value) > GamepadAxisThreshold)
				for (auto& eventData : mapping.second)
					MessageBus::eventBus()->Send(eventData, DataStream().write(value)->write<char>(i));
		}
	}
}

void InputSystem::Map(KeyboardKey key, string eventName, InputBindingState triggerState)
{
	if (m_KeyboardBindings.find(key) == m_KeyboardBindings.end())
		m_KeyboardBindings.emplace(key, unordered_map<string, InputBindingState>()); // Add to bindings

	// Add event to bindings
	m_KeyboardBindings[key].emplace(eventName, triggerState);
}

void InputSystem::Map(KeyboardKey key, string eventName, std::function<void(Utilities::DataStream)> callback, InputBindingState listenState)
{
	Map(key, eventName, listenState);
	MessageBus::eventBus()->AddReceiver(eventName, callback);
}

void InputSystem::Map(GamepadAxis axis, string eventName)
{
	if (m_GamepadAxisBindings.find(axis) == m_GamepadAxisBindings.end())
		m_GamepadAxisBindings.emplace(axis, vector<string>()); // Add to bindings

	// Add event to bindings
	m_GamepadAxisBindings[axis].push_back(eventName);
}

void InputSystem::Map(GamepadAxis axis, string eventName, function<void(Utilities::DataStream)> callback)
{
	Map(axis, eventName);
	MessageBus::eventBus()->AddReceiver(eventName, callback);
}


void InputSystem::Map(GamepadButton button, string eventName, InputBindingState triggerState)
{
	if (m_GamepadBindings.find(button) == m_GamepadBindings.end())
		m_GamepadBindings.emplace(button, unordered_map<string, InputBindingState>()); // Add to bindings

	// Add event to bindings
	m_GamepadBindings[button].emplace(eventName, triggerState);
}

void InputSystem::Map(GamepadButton button, string eventName, function<void(Utilities::DataStream)> callback, InputBindingState listenState)
{
	Map(button, eventName, listenState);
	MessageBus::eventBus()->AddReceiver(eventName, callback);
}

void InputSystem::Map(MouseButton button, string eventName, InputBindingState listenState)
{
	if(m_MouseBindings.find(button) == m_MouseBindings.end())
		m_MouseBindings.emplace(button, unordered_map<string, InputBindingState>());

	// Add event to bindings
	m_MouseBindings[button].emplace(eventName, listenState);
}

void InputSystem::Map(MouseButton button, string eventName, function<void(Utilities::DataStream)> callback, InputBindingState listenState)
{
	Map(button, eventName, listenState);
	MessageBus::eventBus()->AddReceiver(eventName, callback);
}

void InputSystem::MapMouseScroll(string eventName) { m_MouseScrollBindings.push_back(eventName); }

void InputSystem::MapMouseScroll(string eventName, function<void(Utilities::DataStream)> callback)
{
	MapMouseScroll(eventName);
	MessageBus::eventBus()->AddReceiver(eventName, callback);
}

void InputSystem::Unmap(KeyboardKey key, string& eventName)
{
	if (m_KeyboardBindings.find(key) == m_KeyboardBindings.end())
		return; // Isn't mapped to any events
	m_KeyboardBindings[key].erase(eventName);
}

void InputSystem::Unmap(GamepadAxis axis, string& eventName)
{
	if (m_GamepadAxisBindings.find(axis) == m_GamepadAxisBindings.end())
		return; // Isn't mapped to any events
	vector<string>& events = m_GamepadAxisBindings[axis];
	events.erase(std::remove(events.begin(), events.end(), eventName), events.end());
}

void InputSystem::Unmap(GamepadButton button, string& eventName)
{
	if (m_GamepadBindings.find(button) != m_GamepadBindings.end())
		m_GamepadBindings[button].erase(eventName); // Check if mapped to any events
}

void InputSystem::Unmap(MouseButton button, string& eventName)
{
	if (m_MouseBindings.find(button) != m_MouseBindings.end())
		m_MouseBindings[button].erase(eventName); // Check if mapped to any events
}

void InputSystem::UnmapMouseScroll(std::string& eventName) { m_MouseScrollBindings.erase(remove(m_MouseScrollBindings.begin(), m_MouseScrollBindings.end(), eventName), m_MouseScrollBindings.end()); }
