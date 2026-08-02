#include "TextureHandle.h"
#include <TextureLib.h>
#include <Texture.h>
#include <vector>
#include <unordered_map>
#include <memory>

namespace Monolith{

	namespace{
		std::vector<std::unique_ptr<Texture>> s_textures;
		std::unordered_map<std::string, TextureHandle> s_pathToHandle;
	}

	TextureHandle TextureLib::Load(const std::string& path){
		auto it = s_pathToHandle.find(path);
		if(it != s_pathToHandle.end())
			return it->second;

		s_textures.push_back(std::make_unique<Texture>(path));

		TextureHandle handle;
		handle.id = static_cast<uint32_t>(s_textures.size());

		s_pathToHandle[path] = handle;
		return handle;
	}

	Texture& TextureLib::Get(TextureHandle handle){
		return *s_textures[handle.id - 1];
	}

}
