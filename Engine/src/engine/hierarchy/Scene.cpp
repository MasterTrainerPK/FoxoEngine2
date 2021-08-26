#include "Scene.h"

feEntity::operator bool() const
{
	return !(m_Id == entt::null || !m_Scene);
}

bool feEntity::operator==(const feEntity& other) const
{
	return (m_Scene == other.m_Scene) && (m_Id == other.m_Id);
}

feEntity feScene::CreateEntity()
{
	return { this, m_Registry.create() };
}

void feScene::DestroyEntity(feEntity entity)
{
	if (entity.m_Scene == this) m_Registry.destroy(entity.m_Id);
}