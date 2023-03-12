libHDiffPatch = {
	source = path.join(dependencies.basePath, "HDiffPatch/libHDiffPatch"),
}

function libHDiffPatch.import()
	links {
		"libHDiffPatch"
	}

	libHDiffPatch.includes()
end

function libHDiffPatch.includes()
	includedirs {
		path.join(libHDiffPatch.source, "HDiff"),
		path.join(libHDiffPatch.source, "HPatch"),
		path.join(libHDiffPatch.source, "HPatchLite"),
	}
end

function libHDiffPatch.project()
	project "libHDiffPatch"
		language "C++"
		cppdialect "C++11"

		libHDiffPatch.includes()

		files {
			path.join(libHDiffPatch.source, "HDiff/**.h"),
			path.join(libHDiffPatch.source, "HDiff/**.c"),
			path.join(libHDiffPatch.source, "HDiff/**.cpp"),

			path.join(libHDiffPatch.source, "HPatch/**.h"),
			path.join(libHDiffPatch.source, "HPatch/**.c"),

			path.join(libHDiffPatch.source, "HPatchLite/**.h"),
			path.join(libHDiffPatch.source, "HPatchLite/**.c"),
		}

		warnings "Off"
		kind "StaticLib"
end

table.insert(dependencies, libHDiffPatch)
