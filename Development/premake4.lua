
-- A solution contains projects, and defines the available configurations
solution "GUISandbox"
	configurations { "Debug", "Release" }
	platforms { "native" }
	
	-- A project defines one build target
	project "GUISandbox"
		kind "ConsoleApp"
		language "C++"
		files {
			"Source/**.hpp",
			"Source/**.cpp"
		}
		
		includedirs { "Include/" } 
		
		defines({ "UNICODE", "GLEW_STATIC" })
		
		configuration "Debug"
			if os.get() == "windows" then
				links { "opengl32", "glu32", "glew", "sfml-system-d", "sfml-window-d", "sfml-graphics-d" }
			end

			if os.get() == "linux" then
				links { "GL", "GLU", "GLEW", "sfml-system", "sfml-window", "sfml-graphics" }
			end

			flags { "Symbols" }
		
		configuration "Release"
			if os.get() == "windows" then
				links { "opengl32", "glu32", "glew", "sfml-system", "sfml-window", "sfml-graphics" }
			end

			if os.get() == "linux" then
				links { "GL", "GLU", "GLEW", "sfml-system", "sfml-window", "sfml-graphics" }
			end

			flags { "Optimize" }


-- From http://industriousone.com/topic/how-get-current-configuration
-- iterate over all solutions
for sln in premake.solution.each() do
   -- iterate over all projects in the solution
	for pi = 1, #sln.projects do
		-- make this the active project
		prj = sln.projects[pi]
		project(prj.name)
 
		-- iterate over all configurations
		for ci = 1, #sln.configurations do
			-- make this the active configuration
			cfgname = sln.configurations[ci]
			configuration(cfgname)
 
			-- do my custom stuff
			targetdir(path.join(prj.basedir, "../Binaries/" .. prj.name .. "/" .. cfgname))
			objdir(path.join(prj.basedir, "../Temp/" .. prj.name .. "/" .. cfgname))
		end
	end
end
