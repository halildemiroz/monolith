#pragma once
#include <Entity.h>
#include <SparseSet.h>

#include <memory>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <typeinfo>
#include <array>
#include <tuple>

namespace Monolith{
	
	class Registry{
		public:
			Entity Create(){
				uint32_t index;
				if(!m_free.empty()){
					index = m_free.back();
					m_free.pop_back();
				}else{
					m_generations.push_back(1);
					index = static_cast<uint32_t>(m_generations.size());
				}
				return Entity{ index, m_generations[index - 1] };
			}

			bool IsAlive(Entity entity) const {
				return entity.index != 0
					&& entity.index <= m_generations.size()
					&& m_generations[entity.index - 1] == entity.generation;
			}

			void Destroy(Entity entity){
				if(IsAlive(entity))
					m_pendingDestroy.push_back(entity);
			}

			void Flush(){
				for(Entity entity : m_pendingDestroy){
					if(!IsAlive(entity))
						continue;

					for(auto& pair : m_pools)
						pair.second->Remove(entity.index);

					m_generations[entity.index - 1]++;
					m_free.push_back(entity.index);
				}
				m_pendingDestroy.clear();
			}

			template<typename T>
				T& Add(Entity entity, const T& component = T{}){
					return Pool<T>().Add(entity.index, component);
				}

			template<typename T>
				bool Has(Entity entity){
					auto* pool = FindPool<T>();
					return pool && pool->Has(entity.index);
				}

			template<typename T>
				T& Get(Entity entity){
					return Pool<T>().Get(entity.index);
				}

			template<typename T>
				void Remove(Entity entity){
					if(auto* pool = FindPool<T>())
						pool->Remove(entity.index);
				}

			template<typename... Ts, typename Fn>
				void Each(Fn&& fn){
					static_assert(sizeof...(Ts) > 0, "Each requires at least one component type");

					std::tuple<ComponentPool<Ts>*...> typed{ FindPool<Ts>()... };
					std::array<IComponentPool*, sizeof...(Ts)> pools{ FindPool<Ts>()... };

					for(auto* pool : pools)
						if(!pool) return;

					IComponentPool* smallest = pools[0];
					for(auto* pool : pools)
						if(pool->Size() < smallest->Size())
							smallest = pool;

					const std::vector<uint32_t>& entities = smallest->Entities();

					for(size_t i = 0; i < entities.size(); ++i){
						uint32_t index = entities[i];

						bool matches = true;
						for(auto* pool : pools){
							if(!pool->Has(index)){
								matches = false;
								break;
							}
						}
						if(!matches)
							continue;

						Entity entity{ index, m_generations[index - 1] };
						fn(entity, std::get<ComponentPool<Ts>*>(typed)->Get(index)...);
					}
				}

			size_t AliveCount() const { return m_generations.size() - m_free.size(); }

		private:
			template<typename T>
				ComponentPool<T>& Pool(){
					std::type_index type(typeid(T));
					auto it = m_pools.find(type);
					if(it == m_pools.end())
						it = m_pools.emplace(type, std::make_unique<ComponentPool<T>>()).first;
					return *static_cast<ComponentPool<T>*>(it->second.get());
				}
			
			template<typename T>
				ComponentPool<T>* FindPool(){
					auto it = m_pools.find(std::type_index(typeid(T)));
					return it == m_pools.end() ? nullptr : static_cast<ComponentPool<T>*>(it->second.get());
				}

			std::vector<uint32_t> m_generations;
			std::vector<uint32_t> m_free;
			std::vector<Entity> m_pendingDestroy;
			std::unordered_map<std::type_index, std::unique_ptr<IComponentPool>> m_pools;
	};

}

