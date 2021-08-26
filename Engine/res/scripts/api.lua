feVec3 = { x = 0, y = 0, z = 0 }

function feVec3:new(o, x, y, z)
   o = o or {}
   setmetatable(o, self)
   self.__index = self
   self.x = x
   self.y = y
   self.z = z
   return o
end

function feVec3:print()
   print("x = " .. self.x .. ", y = " .. self.y .. ", z = " .. self.z)
end