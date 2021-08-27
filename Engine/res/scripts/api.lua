feVec3 = {}
feVec3.__index = feVec3

function feVec3:new(x, y, z)
	o = {}
	setmetatable(o, self)
	o.x = x
	o.y = y
	o.z = z
	return o
end

function feVec3:str()
	return "feVec3(x = " .. self.x .. ", y = " .. self.y .. ", z = " .. self.z .. ")"
end

feVec4 = {}
feVec4.__index = feVec4

function feVec4:new(x, y, z, w)
	o = {}
	setmetatable(o, self)
	o.x = x
	o.y = y
	o.z = z
	o.w = w
	return o
end

function feVec4:str()
	return "feVec4(x = " .. self.x .. ", y = " .. self.y .. ", z = " .. self.z .. ", w = " .. self.w .. ")"
end

feTransform = {}
feTransform.__index = feTransform

function feTransform:new(pos, quat, sca)
	o = {}
	setmetatable(o, self)
	o.pos = pos
	o.quat = quat
	o.sca = sca
	return o
end

function feTransform:str()
	return "feTransform(pos = " .. self.pos:str() .. ", quat = " .. self.quat:str() .. " sca = " .. self.sca:str() .. ")"
end