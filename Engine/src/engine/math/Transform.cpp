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