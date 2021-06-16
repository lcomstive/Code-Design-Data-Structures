#pragma once
#include <map>
#include <queue>
#include <string>
#include <atomic>
#include <mutex>
#include <vector>
#include <functional>
#include <thread>
#include <GameUtilities/data-stream.hpp>

namespace Utilities
{
	class MessageBus
	{
	public:
		MessageBus();
		~MessageBus();

		void RemoveReceiver(std::string event, unsigned int ID);
		unsigned int AddReceiver(std::string event, std::function<void(DataStream)> callback);

		// Dispatches message on bus
		void Send(std::string event);

		// Dispatches message on bus
		void Send(std::string event, DataStream data);

		// Dispatches message on bus, forced to be delayed until next event loop
		void SendBuffered(std::string event, DataStream data);

		// Dispatches message on bus, forced to be sent immediately
		void SendImmediate(std::string event, DataStream data);

		void ForceFlush();

		void bufferMessages(bool buffer = true);

		// Effectively a singleton for generic use (e.g. logging, input)
		static MessageBus* eventBus();

	private:
		struct Message
		{
			std::string event;
			DataStream data;

			Message(std::string name, DataStream value) : event(name), data(value) { }
			
			static Message empty() { return Message("", DataStream()); }
		};

		bool m_BufferMessages;

		// key - hashed event name
		// value
		//		unsigned int - received ID
		//		std::function	 - event callback
		std::map<size_t, std::map<unsigned int, std::function<void(DataStream)>>> m_Receivers;
		std::queue<Message> m_Messages;

		std::mutex m_MessageMutex;
		std::mutex m_ReceiverMutex;
		std::thread m_DispatchThread;
		std::atomic_bool m_DispatchThreadShouldRun;

		static MessageBus m_EventBus;

		void EventLoop();
		void DispatchMessage(Message m);
	};
}