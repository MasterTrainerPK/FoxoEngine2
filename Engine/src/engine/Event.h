#pragma once

#include <typeinfo>
#include <functional>
#include <unordered_map>
#include <string_view>
#include <vector>

/// Event system that uses type erasure to allow easy way to listen for functions and to dispatch them
/// This class uses member function pointers
class feEventDispatcher final
{
private:
	// The function is a wrapper for the raw member function pointer
	// listener is the pointer to the object listening
	struct feMemberPair final
	{
		std::function<void(void*, const void*)> function;
		void* listener;
	};
public:
	/// Sets up a member function to recieve a specific event.
	/// The template types can be automatically deduced by the input params.
	///
	/// @param listener The object that wants to recieve events
	/// @param function the member function pointer in which to recieve events
	template<typename t_ListenerType, typename t_EventType>
	void Subscribe(t_ListenerType* listener, void(t_ListenerType::* function)(const t_EventType&))
	{
		// Since we use type erasure to store everything together, everything gets casted to void* or void(*)() for listeners and functions respectivly
		// Function pointers must be casted to their proper type before calling them to ensure proper behaviour
		// std::function is used to store the capture for the function, since we cannot pass it into the params
		// This must be defined in this function as well so that the template parameters are avilable to the function
		std::function<void(void*, const void*)> wrapper = [function](void* listener, const void* event)
		{
			// This wrapper gets invoked when a matching event type is dispatched
			// We cast the listener and event to their real pointer types
			// We then call the member function pointer using the casted listener param
			// This will invoke the function with the correct params and correct signiture
			(*static_cast<t_ListenerType*>(listener).*function)(*static_cast<const t_EventType*>(event));
		};
		
		// We use runtime type information to deduce information about the event types only
		// These get stored as keys in the lookup tabke
		std::string_view name = typeid(t_EventType).name();

		// If no entry in the table exists for this event type yet, create one
		if (m_Funcs.find(name) == m_Funcs.end()) m_Funcs.emplace(std::make_pair(name, std::vector<feMemberPair>()));

		// Retrive the vector of member pairs in the table and insert this function into it
		auto& vector = m_Funcs.find(name)->second;
		vector.push_back(feMemberPair{ wrapper, listener });
	}

	/// Removes all registered events from lister
	/// @param listener The listener in which to remove event listeners
	template<typename t_ListenerType>
	void Unsubscribe(t_ListenerType* listener)
	{
		// We use iterators here without any incrementing in the for loop directly
		// This allows us to delete values from the table and from the vector while iterating
		// We must iterate over all event types to be sure all instances are removed
		for (auto funcIt = m_Funcs.begin(); funcIt != m_Funcs.end();)
		{
			// The vector for the event type
			auto& value = funcIt->second;

			// Iterate over the vector for this type of event
			for (auto vecIt = value.begin(); vecIt != value.end();)
			{
				// If this index in this vector has the same literal pointer value as the param then remove it
				if (vecIt->listener == listener) vecIt = value.erase(vecIt);
				else ++vecIt;
			}

			// After we remove all required elements, we can deallocate the vector if its size is 0
			if (value.empty()) funcIt = m_Funcs.erase(funcIt);
			else ++funcIt;
		}
	}

	/// Dispatches an event to all listeners that subscribed to it
	/// The main template parameter takes the type of event to construct
	/// We also take any number of arguments to construct the event in place locally with perfect forwarding
	template<typename t_EventType, typename... t_Args>
	void Dispatch(t_Args&&... args) const
	{
		// Get the name of the event we want to create
		std::string_view name = typeid(t_EventType).name();

		// Get an iterator to the name in the table
		auto result = m_Funcs.find(name);

		// If the event type isnt in the table, then no listeners are subscribed to this event
		// Thus we can return now and be done
		if (result == m_Funcs.end()) return;

		// Construct the actual event using the parameters
		t_EventType event = t_EventType{ std::forward<t_Args>(args)... };

		// At this point we know the table has a vector for the function types, let's get that
		auto& vector = result->second;

		// Iterate over all member pairs in the vector, the function is the wrapper as defined in Subscribe
		// This function takes a void* for the listener, in which the wrapper will cast to the proper type
		// We also pass it the ptr to the event itself, this will be casted to the correct type before its recieved by the listeners
		for (const auto& [function, listener] : vector) function(listener, &event);
	}
private:
	std::unordered_map<std::string_view, std::vector<feMemberPair>> m_Funcs;
};