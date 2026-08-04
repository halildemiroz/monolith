#include <Image.h>

#define STBI_THREAD_LOCAL thread_local
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <iostream>

namespace Monolith{

	ImageData LoadImage(const std::string &path, bool flipVertically){
		stbi_set_flip_vertically_on_load_thread(flipVertically ? 1 : 0);

		ImageData image;
		image.pixels = stbi_load(path.c_str(), &image.width, &image.height, &image.channels, 0);
		if(!image.pixels)
			std::cerr << "Failed to load image: " << path << " (" << stbi_failure_reason() << ")" << std::endl;
		return image;
	}

	void FreeImage(ImageData& image){
		if(image.pixels){
			stbi_image_free(image.pixels);
			image.pixels = nullptr;
		}
		image.width = image.height = image.channels = 0;
	}

}
