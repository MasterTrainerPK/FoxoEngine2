#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

class feTransform final
{
public:
	glm::mat4 GetMatrix() const;
	void SetMatrix(const glm::mat4& mat);
	void Rotate(float angle, const glm::vec3& axis);

	/// This function removes and perspective and skew on the transformation
	/// This is typically a side effect that may happen when care isn't taken when directly modifying quaternion rotations
	void Validate();
public:
	glm::vec3 pos = glm::vec3(0, 0, 0);
	glm::quat quat = glm::quat(0, 0, 0, 0);
	glm::vec3 sca = glm::vec3(1, 1, 1);
};