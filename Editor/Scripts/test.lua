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
	local meshh = Ent.AddComponent(Type.MeshRef, "sphere.obj")
	
	--meshh:ChangeMesh("sphere.obj")
end

function OnTick()
	
	local dt = DeltaTime
	local y = Input.KeyDown("W");
	y = y + -Input.KeyDown("S")
	local z = Input.KeyDown("D")
	z = z - Input.KeyDown("A")

	Ent.Move(z * dt * 2, 2 * dt, y * dt * 2)
end