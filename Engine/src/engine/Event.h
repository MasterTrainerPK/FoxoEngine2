#pragma once

#include <typeinfo>
#include <functional>
#include <unordered_map>
#include <string_view>
#include <vector>

class feEventDispatcher final
{
private:
	struct feMemberPair final
	{
		std::function<void(void*, const void*)> function;
		void* listener;
	};
public:
	template<typename t_ListenerType, typename t_EventType>
	void Subscribe(t_ListenerType* listener, void(t_ListenerType::* function)(const t_EventType&))
	{
		std::function<void(void*, const void*)> wrapper = [function](void* listener, const void* event)
		{
			(*static_cast<t_ListenerType*>(listener).*function)(*static_cast<const t_EventType*>(event));
		};

		std::string_view name = typeid(t_EventType).name();

		if (m_Funcs.find(name) == m_Funcs.end()) m_Funcs.emplace(std::make_pair(name, std::vector<feMemberPair>()));

		auto& vector = m_Funcs.find(name)->second;
		vector.push_back(feMemberPair{ wrapper, listener });
	}

	template<typename t_ListenerType>
	void Unubscribe(t_ListenerType* listener)
	{
		for (auto funcIt = m_Funcs.begin(); funcIt != m_Funcs.end();)
		{
			auto& value = funcIt->second;

			for (auto vecIt = value.begin(); vecIt != value.end();)
			{
				if (vecIt->listener == listener) vecIt = value.erase(vecIt);
				else ++vecIt;
			}

			if (value.empty()) funcIt = m_Funcs.erase(funcIt);
			else ++funcIt;
		}
	}

	template<typename t_EventType, typename... t_Args>
	void Dispatch(t_Args&&... args) const
	{
		std::string_view name = typeid(t_EventType).name();

		auto result = m_Funcs.find(name);

		if (result == m_Funcs.end()) return;

		t_EventType event = t_EventType{ std::forward<t_Args>(args)... };

		auto& vector = result->second;

		for (const auto& [function, listener] : vector) function(listener, &event);
	}
private:
	std::unordered_map<std::string_view, std::vector<feMemberPair>> m_Funcs;
};