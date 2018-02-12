target = NewTarget("game")

function target:configure(settings)
  settings.target = self

	settings.cc.includes:Add("/usr/include")
	settings.link.libs:Add("GL")
	settings.link.libs:Add("GLU")
	settings.link.libs:Add("GLEW")
	settings.link.libs:Add("glfw")

  local src = CollectRecursive(PathJoin(src_dir, "*.cpp"))
  local obj = Compile(settings, src)
  local bin = Link(settings, "resize", obj)
	--AddDependency(bin, "libglew", "libglfw", "libglm")

  AddDependency(self.name, bin)
	DefaultTarget("game")

	PseudoTarget("run")
	AddDependency("run", "game")
	AddJob("run", "running '"..bin.."'", bin)
end
