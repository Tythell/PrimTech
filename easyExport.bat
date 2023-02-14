if not exist exportFolder\ (
	mkdir exportFolder\
)
cd exportFolder

cd ..\PrimTech\
if not exist ..\exportFolder\Assets\ (
	mkdir ..\exportFolder\Assets\
)
if not exist ..\exportFolder\Scenes\ (
	mkdir ..\exportFolder\Scenes\
)

copy Scenes\ ..\exportFolder\Scenes\
copy ..\x64\Release\ ..\exportFolder\
xcopy Assets\ ..\exportFolder\Assets\ /E