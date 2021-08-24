#include "Debug.h"

#include <glad/gl.h>

#include "Util.h"

feDebugGroup::feDebugGroup(std::string_view message, unsigned int id)
{
	if (feRenderUtil::GetSupportedVersion() >= 43) glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, id, -1, message.data());
}

feDebugGroup::~feDebugGroup()
{
	if (feRenderUtil::GetSupportedVersion() >= 43) glPopDebugGroup();
}