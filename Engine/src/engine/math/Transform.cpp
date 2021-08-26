#include "Transform.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>

glm::mat4 feTransform::GetMatrix() const
{
	glm::mat4 mat = glm::identity<glm::mat4>();
	mat = glm::translate(mat, pos);
	mat *= glm::toMat4(quat);
	mat = glm::scale(mat, sca);
	return mat;
}

void feTransform::SetMatrix(const glm::mat4& mat)
{
	glm::vec3 skew;
	glm::vec4 perspective;

	glm::decompose(mat, sca, quat, pos, skew, perspective);
}

void feTransform::Rotate(float angle, const glm::vec3& axis)
{
	// Preferred way to do this, this function doesn't work for whatever reason
	// quat = glm::rotate(quat, angle, axis);

	// Performs a rotation on the entire transform, not used because it exposes position and scaling
	// SetMatrix(glm::rotate(GetMatrix(), angle, axis)); 

	// Convers the rotation to a matrix and uses that to apply the quaternion
	quat = glm::normalize(glm::toQuat(glm::rotate(glm::toMat4(quat), angle, axis)));
}

void feTransform::Validate()
{
	SetMatrix(GetMatrix());
}