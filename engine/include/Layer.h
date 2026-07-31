#pragma once

namespace Monolith{

	class Layer{
		public:
			virtual ~Layer() = default;

			virtual void OnAttach(){}
			virtual void OnDetach(){}
			virtual void OnUpdate(float deltaTime){}
			virtual void OnRender(){}
	};

}
