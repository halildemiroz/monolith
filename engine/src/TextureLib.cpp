#include "TextureHandle.h"
#include "imgui.h"
#include <Image.h>
#include <TextureLib.h>
#include <Texture.h>

#include <cstdint>
#include <vector>
#include <unordered_map>
#include <memory>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <deque>
#include <filesystem>

namespace Monolith{

	struct Entry{
		std::unique_ptr<Texture> texture;
		std::string path;
		uint32_t generation = 1;
		bool alive = false;
		bool ready = false;
		std::filesystem::file_time_type lastWrite{};
	};

	struct PendingJob{
		uint32_t index = 0;
		uint32_t generation = 0;
		std::string path;
		bool isReload = false;
	};

	struct DecodedJob{
		uint32_t index = 0;
		uint32_t generation = 0;
		ImageData image;
		bool isReload = false;
	};
	
	std::vector<Entry> s_entries;
	std::unordered_map<std::string, uint32_t> s_pathToIndex;
	std::unique_ptr<Texture> s_placeholder;

	TextureLib::Stats s_stats;

	std::thread s_worker;
	std::mutex s_mutex;
	std::condition_variable s_cv;
	std::deque<PendingJob> s_pending;
	std::deque<DecodedJob> s_completed;
	bool s_running = false;

#ifdef NDEBUG
	bool s_hotReload = false;
#else
	bool s_hotReload = true;
#endif
	float s_hotReloadTimer = 0.0f;
	constexpr float kHotReloadInterval = 0.5f;

	std::filesystem::file_time_type FileTime(const std::string& path){
		std::error_code ec;
		auto time = std::filesystem::last_write_time(path, ec);
		return ec ? std::filesystem::file_time_type{} : time;
	}

	void Enqeue(PendingJob job){
		{
			std::lock_guard<std::mutex> lock(s_mutex);
			s_pending.push_back(std::move(job));
		}
		s_cv.notify_one();
	}

	void WorkerMain(){
		for(;;){
			PendingJob job;
			{
				std::unique_lock<std::mutex> lock(s_mutex);
				s_cv.wait(lock, []{return !s_running || !s_pending.empty();});
				if(!s_running && s_pending.empty())
					return;
				job = std::move(s_pending.front());
				s_pending.pop_front();
			}

			DecodedJob done;
			done.index = job.index;
			done.generation = job.generation;
			done.isReload = job.isReload;
			done.image = LoadImage(job.path);

			std::lock_guard<std::mutex> lock(s_mutex);
			s_completed.push_back(std::move(done));
		}
	}
	
	uint32_t AllocateEntry(const std::string& path){
		uint32_t index = 0;
		for(uint32_t i = 0; i < s_entries.size(); ++i){
			if(!s_entries[i].alive){
				index = i + 1;
				break;
			}
		}

		if(index == 0){
			s_entries.emplace_back();
			index = static_cast<uint32_t>(s_entries.size());
		}

		Entry& entry = s_entries[index - 1];
		entry.path = path;
		entry.alive = true;
		entry.ready = false;
		entry.texture = std::make_unique<Texture>(1, 1, 0xffffffffu);

		s_pathToIndex[path] = index;
		return index;
	}

	void TextureLib::Init(){
		s_placeholder = std::make_unique<Texture>(1, 1, 0xffffffffu);
		{
			std::lock_guard<std::mutex> lock(s_mutex);
			s_running = true;
		}
		s_worker = std::thread(WorkerMain);
	}

	void TextureLib::Shutdown(){
		{
			std::lock_guard<std::mutex> lock(s_mutex);
			s_running = false;
		}
		s_cv.notify_all();
		if(s_worker.joinable())
			s_worker.join();

		{
			std::lock_guard<std::mutex> lock(s_mutex);
			for(auto& job : s_completed)
				FreeImage(job.image);
			s_completed.clear();
			s_pending.clear();
		}
		s_entries.clear();
		s_pathToIndex.clear();
		s_placeholder.reset();
	}

	TextureHandle TextureLib::Load(const std::string& path){
		s_stats.loadRequests++;

		auto it = s_pathToIndex.find(path);
		if(it != s_pathToIndex.end()){
			s_stats.cacheHits++;
			return TextureHandle{it->second, s_entries[it->second - 1].generation};
		}

		uint32_t index = AllocateEntry(path);
		Entry& entry = s_entries[index - 1];

		ImageData image = LoadImage(path);
		if(image.isValid()){
			entry.texture->SetData(image.pixels, image.width, image.height, image.channels);
			FreeImage(image);
			entry.ready = true;
			s_stats.gpuUploads++;
		}
		entry.lastWrite = FileTime(path);

		return TextureHandle{ index, entry.generation };
	}

	TextureHandle TextureLib::LoadAsync(const std::string &path){
		s_stats.loadRequests++;

		auto it = s_pathToIndex.find(path);
		if(it != s_pathToIndex.end()){
			s_stats.cacheHits++;
			return TextureHandle{ it->second, s_entries[it->second - 1].generation };
		}

		uint32_t index = AllocateEntry(path);
		Entry& entry = s_entries[index - 1];
		entry.lastWrite = FileTime(path);

		Enqeue(PendingJob {index, entry.generation, path, false});
		return TextureHandle{ index, entry.generation };
	}

	bool TextureLib::IsValid(TextureHandle handle){
		if(handle.index == 0 || handle.index > s_entries.size())
			return false;

		const Entry& entry = s_entries[handle.index - 1];
		return entry.alive && entry.generation == handle.generation;
	}

	Texture& TextureLib::Get(TextureHandle handle){
		if(!IsValid(handle))
			return *s_placeholder;
		return *s_entries[handle.index - 1].texture;
	}

	void TextureLib::Unload(TextureHandle handle){
		if(!IsValid(handle))
			return;

		Entry& entry = s_entries[handle.index - 1];
		s_pathToIndex.erase(entry.path);

		entry.texture.reset();
		entry.path.clear();
		entry.alive = false;
		entry.ready = false;
		entry.generation++;
	}

	void TextureLib::Update(float deltaTime){
		for(;;){
			DecodedJob job;
			{
				std::lock_guard<std::mutex> lock(s_mutex);
				if(s_completed.empty())
					break;
				job = std::move(s_completed.front());
				s_completed.pop_front();
			}

			if(IsValid(TextureHandle{job.index, job.generation}) && job.image.isValid()){
				Entry& entry = s_entries[job.index - 1];
				entry.texture->SetData(job.image.pixels, job.image.width, job.image.height, job.image.channels);
				entry.ready = true;
				s_stats.gpuUploads++;
				if(job.isReload)
					s_stats.hotReloads++;
			}

			FreeImage(job.image);
		}
		{
			std::lock_guard<std::mutex> lock(s_mutex);
			s_stats.pendingAsync = static_cast<uint32_t>(s_pending.size() + s_completed.size());
		}

		if(!s_hotReload)
			return;

		s_hotReloadTimer += deltaTime;
		if(s_hotReloadTimer < kHotReloadInterval)
			return;
		s_hotReloadTimer = 0.0f;

		for(uint32_t i = 0; i < s_entries.size(); ++i){
			Entry& entry = s_entries[i];
			if(!entry.alive || entry.path.empty())
				continue;

			auto time = FileTime(entry.path);
			if(time != entry.lastWrite){
				entry.lastWrite = time;
				Enqeue(PendingJob{ i + 1, entry.generation, entry.path, true });
			}
		}
	}

	void TextureLib::SetHotReloadEnabled(bool enabled){ s_hotReload = enabled; }
	bool TextureLib::IsHotReloadEnabled(){ return s_hotReload; }

	const TextureLib::Stats& TextureLib::GetStats(){ return s_stats; }
}
