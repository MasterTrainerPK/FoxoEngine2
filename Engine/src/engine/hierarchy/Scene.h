#pragma once

#include <entt/entity/registry.hpp>

struct feScene;

struct feEntity final
{
	template<typename t_Component, typename... t_Args>
	t_Component& CreateComponent(t_Args&&... args)
	{
		return m_Scene->m_Registry.template emplace<t_Component>(m_Id, std::forward<t_Args>(args)...);
	}

	template<typename t_Component>
	t_Component& GetComponent()
	{
		return m_Scene.m_Registry.template get<t_Component>(m_Id);
	}

	operator bool() const;
	bool operator==(const feEntity& other) const;

	feScene* m_Scene = nullptr;
	entt::entity m_Id = entt::null;
};

struct feScene final
{
	feEntity CreateEntity();
	void DestroyEntity(feEntity entity);

	entt::registry m_Registry;
};