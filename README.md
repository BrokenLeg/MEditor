# MEditor
Every self-respecting c++ graphics programmer has renderer application using OpenGL and ImGui, so I decided to catch up

Project goals
1. Loading/Saving scenes and models (scene graph)
2. Editing object properties in toolbar (UI) and in editor (Triad, Mouse Picking)
3. Orbit camera and panning
4. Materials and lights
   
# Day 1

Added
1. Mesh (transform, name, color)
2. Camera (position, front/up/right, fov)
3. ImGui toolbar for renaming Mesh and editing it's transform

![day1](https://github.com/BrokenLeg/MEditor/assets/68334150/d2ac3c62-2cbe-47f8-aa20-3621c982e4a5)

# Day 2-3

Added
1. Model loading (.dae preffered as it preserves hierarchy)
2. Scene hierarchy window - can select node and modify it's local transformation

![day 2-3](https://github.com/BrokenLeg/MEditor/assets/68334150/85877ddc-6ee3-4bbd-b9b4-6ee6a463fa89)

# Day 4-5

Added
1. Basic ambient and diffuse lighting (no wireframe)
2. Transform component
3. Rotation around world axis using WS-AD-QE

![day4-5](https://github.com/BrokenLeg/MEditor/assets/68334150/ac3c9ab6-7ec8-4ac9-99a3-1d51b75d14cd)

# Day 6

Added
1. Light and Material
2. Editiong of Light and Material in ImGui tabs
3. More flexible scene, can add lights as nodes, not all nodes contain meshes

![day 6-7](https://github.com/BrokenLeg/MEditor/assets/68334150/481044a1-6563-4715-b76c-5bc443181927)






