#pragma once
#define KeyCount 5

namespace Monolith{

	enum class Key {W, A, S, D, Escape};

	class Input{
		public:
			static bool isKeyDown(Key key);     	
			static bool isKeyPressed(Key key); 
		private:
			static bool s_current[KeyCount];
			static bool s_previous[KeyCount];
			friend class App;
			static void Update();
	};
}
