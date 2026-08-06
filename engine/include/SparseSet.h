#pragma once
#include <Entity.h>
#include <cstdint>
#include <sys/types.h>
#include <vector>
#include <cassert>
#include <utility>

namespace Monolith {

	class IComponentPool{
		public:
			virtual ~IComponentPool() = default;
			virtual void Remove(uint32_t entityIndex) = 0;
			virtual bool Has(uint32_t entityIndex) const = 0;
			virtual size_t Size() const = 0;
			virtual const std::vector<uint32_t>& Entities() const = 0;
	};

	template<typename T>
		class ComponentPool : public IComponentPool{
			public:
				T& Add(uint32_t entityIndex, const T& component){
					if(Has(entityIndex)){
						Get(entityIndex) = component;
						return Get(entityIndex);
					}
					
					if(entityIndex >= m_sparse.size())
						m_sparse.resize(entityIndex + 1, 0);

					m_dense.push_back(entityIndex);
					m_components.push_back(component);
					m_sparse[entityIndex] = static_cast<uint32_t>(m_dense.size());

					return m_components.back();
				}

				void Remove(uint32_t entityIndex) override{
					if(!Has(entityIndex))
						return;

					uint32_t slot = m_sparse[entityIndex] - 1;
					uint32_t last = static_cast<uint32_t>(m_dense.size() - 1);

					m_dense[slot] = m_dense[last];
					m_components[slot] = std::move(m_components[last]);
					m_sparse[m_dense[slot]] = slot + 1;

					m_dense.pop_back();
					m_components.pop_back();
					m_sparse[entityIndex] = 0;
				}

				bool Has(uint32_t entityIndex) const override{
					return entityIndex < m_sparse.size() && m_sparse[entityIndex] != 0;
				}

				T& Get(uint32_t entityIndex){
					assert(Has(entityIndex));
					return m_components[m_sparse[entityIndex] - 1];
				}

				size_t Size() const override{ return m_dense.size(); }
				const std::vector<uint32_t>& Entities() const override{ return m_dense; }

				std::vector<T>& Components(){ return m_components; }
		
			private:
				std::vector<uint32_t> m_sparse;
				std::vector<uint32_t> m_dense;
				std::vector<T> m_components;
		};

}
