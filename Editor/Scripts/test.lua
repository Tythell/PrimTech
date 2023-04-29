function OnStart()
	
	local mover = Ent.GetComponent(Type.Transform)
	mover:Move(0.0004, 1, 0)

end

function OnTick()
	
	local mover = Ent.GetComponent(Type.Transform)
	mover:Move(0.0004, 0, 0)

end