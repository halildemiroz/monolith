#pragma once
#include <functional>
#include <unordered_map>
#include <typeindex>
#include <vector>
#include <algorithm>
#include <cstdint>

namespace Monolith {
	
	class EventBus {
		public:
			using SubscriptionID = uint32_t;
			
			template<typename T, typename Fn>
				SubscriptionID Subscribe(Fn&& callback){
					SubscriptionID id = m_nextID++;

					m_handlers[std::type_index(typeid(T))].push_back(
							Handler{
							id,
							[callback = std::forward<Fn>(callback)](const void* payload){
							callback(*static_cast<const T*>(payload));
							},
							true
							}
							);

					return id;
				}

			void Unsubscribe(SubscriptionID id){
				for(auto& pair : m_handlers)
					for(auto& handler : pair.second)
						if(handler.id == id)
							handler.alive = false;
				if(m_dispatchDepth == 0)
					Sweep();
			}

			template<typename T>
				void Emit(const T& event){
					auto it = m_handlers.find(std::type_index(typeid(T)));
					if(it == m_handlers.end())
						return;

					auto& handlers = it->second;
					size_t count = handlers.size();

					m_dispatchDepth++;
					for(size_t i = 0; i < count; ++i)
						if(handlers[i].alive)
							handlers[i].fn(&event);
					m_dispatchDepth--;

					if(m_dispatchDepth == 0)
						Sweep();
				}

			template<typename T>
				void Queue(const T& event){
					m_queue.push_back([this, event](){ Emit(event); });
				}

			void DispatchQueued(){
				std::vector<std::function<void()>> pending;
				pending.swap(m_queue);

				for(auto& dispatch : pending)
					dispatch();
			}

			void Clear(){
				m_handlers.clear();
				m_queue.clear();
			}

			size_t QueuedCount() const { return m_queue.size(); }

		private:
			struct Handler{
				SubscriptionID id;
				std::function<void(const void*)> fn;
				bool alive;
			};
			
			void Sweep(){
				for(auto& pair : m_handlers){
					auto& handlers = pair.second;
					handlers.erase(
							std::remove_if(handlers.begin(), handlers.end(),
								[](const Handler& handler){ return !handler.alive; }),
							handlers.end());
				}
			}

			std::unordered_map<std::type_index, std::vector<Handler>> m_handlers;
			std::vector<std::function<void()>> m_queue;
			SubscriptionID m_nextID = 1;
			int m_dispatchDepth = 0;
	};	
	
}
