#pragma once
#include <string>

namespace Monolith{

	struct ImageData{
		unsigned char* pixels = nullptr;
		int width = 0;
		int height = 0;
		int channels = 0;

		bool isValid() const { return pixels != nullptr; }
	};

	ImageData LoadImage(const std::string& path, bool flipVertically = true);
	void FreeImage(ImageData& image);
}
