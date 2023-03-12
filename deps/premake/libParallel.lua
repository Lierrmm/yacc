libParallel = {
	source = path.join(dependencies.basePath, "HDiffPatch"),
}

function libParallel.import()
	links {
		"libParallel"
	}

	libParallel.includes()
end

function libParallel.includes()
	includedirs {
		path.join(libParallel.source, "libParallel"),
	}
end

function libParallel.project()
	project "libParallel"
		language "C++"
		cppdialect "C++11"

		files {
			path.join(libParallel.source, "libParallel/*.h"),
			path.join(libParallel.source, "libParallel/*.cpp"),
		}

		warnings "Off"
		kind "StaticLib"
end

table.insert(dependencies, libParallel)
