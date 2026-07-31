#include <Input.h>
#include <SDL.h>
#include <cstring>

namespace Monolith{

	namespace {
		SDL_Scancode ToScancode(Key key){
			switch(key){
				case Key::W:      return SDL_SCANCODE_W;
				case Key::A:      return SDL_SCANCODE_A;
				case Key::S:      return SDL_SCANCODE_S;
				case Key::D:      return SDL_SCANCODE_D;
				case Key::Escape: return SDL_SCANCODE_ESCAPE;
			}
			return SDL_SCANCODE_UNKNOWN;
		}
	}

	bool Input::s_current[KeyCount]  = {};
	bool Input::s_previous[KeyCount] = {};

	void Input::Update(){
		std::memcpy(s_previous, s_current, sizeof(s_current));

		const Uint8* state = SDL_GetKeyboardState(nullptr);

		for(int i = 0; i < KeyCount; ++i){
			SDL_Scancode sc = ToScancode(static_cast<Key>(i));
			s_current[i] = state[sc] != 0;
		}
	}

	bool Input::isKeyDown(Key key){
		return s_current[static_cast<int>(key)];
	}

	bool Input::isKeyPressed(Key key){
		int i = static_cast<int>(key);
		return s_current[i] && !s_previous[i];
	}
}

