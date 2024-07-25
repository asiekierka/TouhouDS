
function clone(obj)
	local result = {}
	for k,v in pairs(obj) do
		result[k] = v
	end
	return result
end

function extend(a, b)
	local result = {}
	for k,v in pairs(a) do
		result[k] = v
	end
	for k,v in pairs(b) do
		result[k] = v
	end
	return result
end

function yield(t)
	wait(t)
	coroutine.yield()
end
