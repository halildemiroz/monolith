#pragma once
#include <TextureHandle.h>
#include <string>

namespace Monolith{

	class Texture;

	class TextureLib{
		public:
			static TextureHandle Load(const std::string& path);
			static Texture& Get(TextureHandle handle);
	};

}
