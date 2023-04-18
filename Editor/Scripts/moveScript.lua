function OnTick()
	
	mover = Ent.GetComponent(Type.Transform)
	mover:Move(0.0003, 0, 0)

end