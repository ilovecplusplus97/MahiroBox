#pragma once
#include <string>
#include <array>
#include <vector>
#include <filesystem>
#include <unordered_map>

struct Resource {
	std::array<QPixmap*, 2> left_hand;
	std::array<QPixmap*, 2> right_hand;
	std::vector<QPixmap*> mahiro;

	Resource();
	~Resource();
};

using ResourceList = std::unordered_map<std::wstring, Resource*>;

class ResourceManager final {
public:
	static ResourceManager* instance();
	static void destroy();

	Resource* load(const std::wstring& path);
	Resource* load(const std::wstring& path, const std::wstring& key);
	Resource* load(Resource* resource, const std::wstring& key);
	void remove(const std::wstring& key);
	void remove(const Resource* resource);
	Resource* get(const std::wstring& key) const;
	bool has(const std::wstring& key) const;
	bool has(const Resource* resource) const;
	void clear();
private:
	static ResourceManager* m_instance;

	ResourceList m_resource;

	ResourceManager() = default;
	~ResourceManager();
};

