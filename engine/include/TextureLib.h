#pragma once
#include <TextureHandle.h>
#include <cstdint>
#include <string>

namespace Monolith{

	class Texture;

	class TextureLib{
		public:
			struct Stats{
				uint32_t loadRequests = 0;
				uint32_t cacheHits = 0;
				uint32_t gpuUploads = 0;
				uint32_t hotReloads = 0;
				uint32_t pendingAsync = 0;
			};

			static void Init();
			static void Shutdown();

			static TextureHandle Load(const std::string& path);
			static TextureHandle LoadAsync(const std::string& path);

			static bool IsValid(TextureHandle handle);
			static Texture& Get(TextureHandle handle);
			static void Unload(TextureHandle handle);

			static void Update(float deltaTime);

			static void SetHotReloadEnabled(bool enabled);
			static bool IsHotReloadEnabled();

			static const Stats& GetStats();
	};

}
