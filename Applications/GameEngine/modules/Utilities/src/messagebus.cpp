#include <iostream>
#include <GameUtilities/log.hpp>
#include <GameUtilities/messagebus.hpp>

#define MAX_MESSAGES_PER_UPDATE 20

#ifndef NDEBUG
// #define MESSAGEBUS_DEBUG
// const bool DefaultShouldBufferMessages = false;
const bool DefaultShouldBufferMessages = true;
#else
const bool DefaultShouldBufferMessages = true;
#endif

using namespace std;
using namespace Utilities;
using namespace chrono_literals;

MessageBus MessageBus::m_EventBus = MessageBus();

MessageBus::MessageBus() : m_BufferMessages(DefaultShouldBufferMessages)
{
	m_DispatchThreadShouldRun.store(true);
	m_DispatchThread = thread(&MessageBus::EventLoop, this);
}

MessageBus::~MessageBus()
{
	m_DispatchThreadShouldRun.store(false);
	m_DispatchThread.join();
}

MessageBus* MessageBus::eventBus() { return &m_EventBus; }

unsigned int MessageBus::AddReceiver(string event, function<void(DataStream)> callback)
{
	unsigned int id = 0;
	size_t hashedEvent = hash<string>()(event);

	if (m_Receivers.find(hashedEvent) == m_Receivers.end())
		m_Receivers.emplace(hashedEvent, map<unsigned int, function<void(DataStream)>>());

	while (m_Receivers[hashedEvent].find(++id) != m_Receivers[hashedEvent].end()) { }

	{
		lock_guard guard(m_ReceiverMutex);
		m_Receivers[hashedEvent].emplace(id, callback);
	}

	return id;
}

void MessageBus::RemoveReceiver(string event, unsigned int id)
{
	size_t hashedEvent = hash<string>()(event);

	if (m_Receivers.find(hashedEvent) == m_Receivers.end() ||
		m_Receivers[hashedEvent].find(id) == m_Receivers[hashedEvent].end())
		return;

	lock_guard guard(m_ReceiverMutex);
	m_Receivers[hashedEvent].erase(id);
}

void MessageBus::Send(string event) { Send(event, DataStream()); }
void MessageBus::Send(string event, DataStream data)
{
#ifdef MESSAGEBUS_DEBUG
	if(event != "LogInfo" && event != "LogDebug" && event != "LogWarning" && event != "LogError")
		GAME_LOG_DEBUG("MessageBus - Sending event '" + event + "'");
#endif

	Message msg(event, data);
	if (m_BufferMessages)
	{
		lock_guard guard(m_MessageMutex);
		m_Messages.push(msg);
	}
	else
		DispatchMessage(msg);
}

void MessageBus::SendBuffered(string event, DataStream data)
{
	Message msg(event, data);
	lock_guard guard(m_MessageMutex);
	m_Messages.push(msg);
}

void MessageBus::SendImmediate(string event, DataStream data) { DispatchMessage(Message(event, data)); }

void MessageBus::bufferMessages(bool buffer) { m_BufferMessages = buffer; }

void MessageBus::ForceFlush()
{
#ifdef MESSAGEBUS_DEBUG
	GAME_LOG_DEBUG("MessageBus - forced flush");
#endif

	vector<Message> updateMessages;

	int messagesToUpdate = MAX_MESSAGES_PER_UPDATE > m_Messages.size() ?
		(int)m_Messages.size() : MAX_MESSAGES_PER_UPDATE;

	for (int i = 0; i < messagesToUpdate; i++)
	{
		updateMessages.push_back(m_Messages.front());
		m_MessageMutex.lock();
		m_Messages.pop();
		m_MessageMutex.unlock();
	}

	for (int i = 0; i < (int)updateMessages.size(); i++)
		DispatchMessage(updateMessages[i]);
	updateMessages.clear();
}

void MessageBus::EventLoop()
{
	vector<Message> updateMessages;
	while (m_DispatchThreadShouldRun.load())
	{
		{
			int messagesToUpdate = MAX_MESSAGES_PER_UPDATE > m_Messages.size() ?
				(int)m_Messages.size() : MAX_MESSAGES_PER_UPDATE;

			for (int i = 0; i < messagesToUpdate; i++)
			{
				updateMessages.push_back(m_Messages.front());

				m_MessageMutex.lock();
				m_Messages.pop();
				m_MessageMutex.unlock();
			}
		}

		for (int i = 0; i < (int)updateMessages.size(); i++)
			DispatchMessage(updateMessages[i]);
		updateMessages.clear();

		this_thread::sleep_for(1ms);
	}
}

void MessageBus::DispatchMessage(Message m)
{
	if (m.event.empty())
		return;
#ifdef MESSAGEBUS_DEBUG
	if (m.event != "LogInfo" && m.event != "LogDebug" && m.event != "LogWarning" && m.event != "LogError")
		GAME_LOG_DEBUG("MessageBus - Dispatching event '" + m.event + "'");
#endif

	vector<function<void(DataStream)>> callbacks;

	size_t hashedEvent = hash<string>()(m.event);
	if (m_Receivers.find(hashedEvent) == m_Receivers.end())
		return;
	for (auto it = m_Receivers[hashedEvent].begin(); it != m_Receivers[hashedEvent].end(); it++)
		callbacks.push_back((*it).second);

	for(int i = 0; i < (int)callbacks.size(); i++)
		try { callbacks[i](m.data); } catch(bad_alloc& e) { GAME_LOG_ERROR("Exception during callback: " + string(e.what())); }
}