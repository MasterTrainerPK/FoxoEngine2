-- Called by the engine to initialize
function Init(ctx)
	print(_VERSION)

	local function makeRandomRange(min, max)
		return min + math.random() * (max - min);
	end

	for i=0, 10, 1
	do
		local scaScalar = makeRandomRange(1, 5);
		local pos = feVec3:new(makeRandomRange(-20, 20), makeRandomRange(-20, 20), makeRandomRange(-20, 20))
		local quat = feVec4:new(0, 0, 0, 0)
		local sca = feVec3:new(scaScalar, scaScalar, scaScalar)
		local transform = feTransform:new(pos, quat, sca)

		local entity = feApiCreateEntity(ctx)
		local component = feApiCreateComponent(entity, "Transform")
		feApiCreateComponent(entity, "MeshFilterComponent")

		if component ~= nil
		then
			feApiComponentTransformSet(component, transform)
		end
	end
end

function Update(ctx)
	
end

function Destroy(ctx)
	print("Destroy is called")
end