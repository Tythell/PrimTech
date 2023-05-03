--Global list:
--DeltaTime
--Ent. Ptr		AddComponent		GetComponent		FreeComponent		Move Rotate		Scale		SetPotision		SetRotation		SetScale
--Type.		MeshRef		Camera		Light		PhysBody
--mesh ChangeModel
--Camera
--
--
--

function OnStart()
	
	--Ent.Move(0.0004, 1, 0)
	local meshh = Ent.AddComponent(Type.MeshRef, "cube.txt")
	
	--meshh:ChangeMesh("sphere.obj")
end

function OnTick()
	
	local dt = DeltaTime
	local y = KeyDown("W");
	y = y + -KeyDown("S")
	local z = KeyDown("D")
	z = z - KeyDown("A")

	Ent.Move(z * dt * 2, 2 * dt, y * dt * 2)
end