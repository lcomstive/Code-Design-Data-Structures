#pragma once
#include <map>
#include <queue>
#include <string>
#include <atomic>
#include <mutex>
#include <vector>
#include <functional>
#include <GameUtilities/data-stream.hpp>

using namespace std;

namespace Utilities
{
	class MessageBus
	{
	public:
		MessageBus();
		~MessageBus();

		void RemoveReceiver(string event, unsigned int ID);
		unsigned int AddReceiver(string event, function<void(DataStream)> callback);

		void Send(string event);
		void Send(string event, DataStream data);
		void SendImmediate(string event, DataStream data);

		void ForceFlush();

		void bufferMessages(bool buffer = true);

		// Effectively a singleton for generic use (e.g. logging, input)
		static MessageBus* eventBus();

	private:
		struct Message
		{
			string event;
			DataStream data;

			Message(string name, DataStream value) : event(name), data(value) { }
			
			static Message empty() { return Message("", DataStream()); }
		};

		bool m_BufferMessages;

		// key - hashed event name
		// value
		//		unsigned int - received ID
		//		function	 - event callback
		map<size_t, map<unsigned int, function<void(DataStream)>>> m_Receivers;
		queue<Message> m_Messages;

		mutex m_MessageMutex;
		mutex m_ReceiverMutex;
		thread m_DispatchThread;
		atomic_bool m_DispatchThreadShouldRun;

		static MessageBus m_EventBus;

		void EventLoop();
		void DispatchMessage(Message m);
	};
}