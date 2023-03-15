#include <std_include.hpp>
#include <loader/module_loader.hpp>
#include "game/game.hpp"
#include "console.hpp"
#include "scheduler.hpp"

#include <utils/concurrency.hpp>
#include <utils/hook.hpp>
#include <utils/string.hpp>
#include <utils/io.hpp>
#include "command.hpp"

class MapDumper
{
public:
	MapDumper(game::native::GfxWorld* world) : world_(world)
	{
	}

	void dump()
	{
		if (!this->world_) return;

		console::info("Exporting '%s'...\n", this->world_->baseName);

		this->parseVertices();
		this->parseFaces();
		this->parseStaticModels();

		this->write();
	}

private:
	struct Vertex
	{
		game::native::vec3_t coordinate;
		game::native::vec2_t texture;
		game::native::vec3_t normal;
	};

	struct Face
	{
		int a{};
		int b{};
		int c{};
	};

	struct FaceList
	{
		std::vector<Face> indices{};
	};

	class File
	{
	public:
		File() {}

		File(const std::string& file)
		{
			utils::io::write_file(file, {});
			this->stream_ = std::ofstream(file, std::ofstream::out);
		}

		void append(const std::string& str)
		{
			this->stream_.write(str.data(), str.size());
		}

	private:
		std::ofstream stream_{};
	};

	game::native::GfxWorld* world_{};
	std::vector<Vertex> vertices_{};
	std::unordered_map<game::native::Material*, FaceList> faces_{};
	std::vector<game::native::Material*> facesOrder_{};

	File object_{};
	File material_{};

	void transformAxes(game::native::vec3_t& vec) const
	{
		std::swap(vec[0], vec[1]);
		std::swap(vec[1], vec[2]);
	}

	void parseVertices()
	{
		console::info("Parsing vertices...\n");

		for (unsigned int i = 0; i < this->world_->vertexCount; ++i)
		{
			const auto* vertex = &this->world_->vd.vertices[i];

			Vertex v{};

			v.coordinate[0] = vertex->xyz[0];
			v.coordinate[1] = vertex->xyz[1];
			v.coordinate[2] = vertex->xyz[2];
			this->transformAxes(v.coordinate);

			v.texture[0] = vertex->texCoord[0];
			v.texture[1] = -vertex->texCoord[1];

			game::native::Vec3UnpackUnitVec(vertex->normal, v.normal);
			this->transformAxes(v.normal);

			this->vertices_.push_back(v);
		}
	}

	void parseFaces()
	{
		console::info("Parsing faces...\n");

		for (unsigned int i = 0; i < this->world_->dpvs.staticSurfaceCount; ++i)
		{
			const auto* surface = &this->world_->dpvs.surfaces[i];

			const unsigned int vertOffset = surface->tris.firstVertex + 1;
			const unsigned int indexOffset = surface->tris.baseIndex;

			// Fuck cube maps for now
			if (this->findImage(surface->material, "colorMap")->mapType == 5) continue;

			auto& f = this->getFaceList(surface->material);

			for (unsigned short j = 0; j < surface->tris.triCount; ++j)
			{
				Face face{};
				face.a = this->world_->indices[indexOffset + j * 3 + 0] + vertOffset;
				face.b = this->world_->indices[indexOffset + j * 3 + 1] + vertOffset;
				face.c = this->world_->indices[indexOffset + j * 3 + 2] + vertOffset;

				f.indices.push_back(face);
			}
		}
	}

	FaceList& getFaceList(game::native::Material* material)
	{
		auto& faceList = this->faces_[material];

		if (this->facesOrder_.size() < this->faces_.size())
		{
			this->facesOrder_.push_back(material);
		}

		return faceList;
	}

	void performWorldTransformation(const game::native::GfxPackedPlacement& placement, Vertex& v) const
	{
		game::native::MatrixVecMultiply(placement.axis, v.normal, v.normal);
		game::native::Vec3Normalize(v.normal);

		game::native::MatrixVecMultiply(placement.axis, v.coordinate, v.coordinate);
		v.coordinate[0] = v.coordinate[0] * placement.scale + placement.origin[0];
		v.coordinate[1] = v.coordinate[1] * placement.scale + placement.origin[1];
		v.coordinate[2] = v.coordinate[2] * placement.scale + placement.origin[2];
	}

	std::vector<Vertex> parseSurfaceVertices(const game::native::XSurface* surface, const game::native::GfxPackedPlacement& placement)
	{
		std::vector<Vertex> vertices;

		for (unsigned short j = 0; j < surface->vertCount; j++)
		{
			const auto* vertex = &surface->verts0[j];

			Vertex v{};

			v.coordinate[0] = vertex->xyz[0];
			v.coordinate[1] = vertex->xyz[1];
			v.coordinate[2] = vertex->xyz[2];

			// Why...
			game::native::Vec2UnpackTexCoords(vertex->texCoord, &v.texture);
			std::swap(v.texture[0], v.texture[1]);
			v.texture[1] *= -1;

			game::native::Vec3UnpackUnitVec(vertex->normal, v.normal);

			this->performWorldTransformation(placement, v);
			this->transformAxes(v.coordinate);
			this->transformAxes(v.normal);

			vertices.push_back(v);
		}

		return vertices;
	}

	std::vector<Face> parseSurfaceFaces(const game::native::XSurface* surface) const
	{
		std::vector<Face> faces;

		for (unsigned short j = 0; j < surface->triCount; ++j)
		{
			Face face{};
			face.a = surface->triIndices[j * 3 + 0];
			face.b = surface->triIndices[j * 3 + 1];
			face.c = surface->triIndices[j * 3 + 2];

			faces.push_back(face);
		}

		return faces;
	}

	void removeVertex(const int index, std::vector<Face>& faces, std::vector<Vertex>& vertices) const
	{
		vertices.erase(vertices.begin() + index);

		for (auto& face : faces)
		{
			if (face.a > index) --face.a;
			if (face.b > index) --face.b;
			if (face.c > index) --face.c;
		}
	}

	void filterSurfaceVertices(std::vector<Face>& faces, std::vector<Vertex>& vertices) const
	{
		for (auto i = 0; i < int(vertices.size()); ++i)
		{
			auto referenced = false;

			for (const auto& face : faces)
			{
				if (face.a == i || face.b == i || face.c == i)
				{
					referenced = true;
					break;
				}
			}

			if (!referenced)
			{
				this->removeVertex(i--, faces, vertices);
			}
		}
	}

	void parseStaticModel(game::native::GfxStaticModelDrawInst* model)
	{
		for (unsigned char i = 0; i < model->model->numsurfs; ++i)
		{
			this->getFaceList(model->model->materialHandles[i]);
		}

		const auto* lod = &model->model->lodInfo[model->model->numLods - 1];

		const auto baseIndex = this->vertices_.size() + 1;
		const auto surfIndex = lod->surfIndex;

		assert(lod->numsurfs <= model->model->numsurfs);

		for (unsigned short i = 0; i < lod->numsurfs; ++i)
		{
			// TODO: Something is still wrong about the models. Probably baseTriIndex and baseVertIndex might help

			//const auto* surface = &lod->modelSurfs->surfs[i];
			const auto* surface = &model->model->surfs[i];
			auto faces = this->parseSurfaceFaces(surface);
			auto vertices = this->parseSurfaceVertices(surface, model->placement);
			this->filterSurfaceVertices(faces, vertices);

			auto& f = this->getFaceList(model->model->materialHandles[i + surfIndex]);

			for (const auto& vertex : vertices)
			{
				this->vertices_.push_back(vertex);
			}

			for (auto face : faces)
			{
				face.a += baseIndex;
				face.b += baseIndex;
				face.c += baseIndex;
				f.indices.push_back(std::move(face));
			}
		}
	}

	void parseStaticModels()
	{
		console::info("Parsing static models...\n");

		for (unsigned i = 0u; i < this->world_->dpvs.smodelCount; ++i)
		{
			this->parseStaticModel(this->world_->dpvs.smodelDrawInsts + i);
		}
	}

	void write()
	{
		this->object_ = File(utils::string::va("raw/mapdump/%s/%s.obj", this->world_->baseName, this->world_->baseName));
		this->material_ = File(utils::string::va("raw/mapdump/%s/%s.mtl", this->world_->baseName, this->world_->baseName));

		this->object_.append("# Generated by YACC\n");
		this->object_.append("# Credit to SE2Dev for his D3DBSP Tool\n");
		this->object_.append(utils::string::va("o %s\n", this->world_->baseName));
		this->object_.append(utils::string::va("mtllib %s.mtl\n\n", this->world_->baseName));

		this->material_.append("# YACC MTL File\n");
		this->material_.append("# Credit to SE2Dev for his D3DBSP Tool\n");

		this->writeVertices();
		this->writeFaces();

		console::info("Writing files...\n");

		this->object_ = {};
		this->material_ = {};
	}

	void writeVertices()
	{
		console::info("Writing vertices...\n");
		this->object_.append("# Vertices\n");

		for (const auto& vertex : this->vertices_)
		{
			this->object_.append(utils::string::va("v %.6f %.6f %.6f\n", vertex.coordinate[0], vertex.coordinate[1], vertex.coordinate[2]));
		}

		console::info("Writing texture coordinates...\n");
		this->object_.append("\n# Texture coordinates\n");

		for (const auto& vertex : this->vertices_)
		{
			this->object_.append(utils::string::va("vt %.6f %.6f\n", vertex.texture[0], vertex.texture[1]));
		}

		console::info("Writing normals...\n");
		this->object_.append("\n# Normals\n");

		for (const auto& vertex : this->vertices_)
		{
			this->object_.append(utils::string::va("vn %.6f %.6f %.6f\n", vertex.normal[0], vertex.normal[1], vertex.normal[2]));
		}

		this->object_.append("\n");
	}

	game::native::GfxImage* findImage(game::native::Material* material, const std::string& type) const
	{
		game::native::GfxImage* image = nullptr;

		const auto hash = game::native::R_HashString(type.data());

		for (char l = 0; l < material->textureCount; ++l)
		{
			if (material->textureTable[l].nameHash == hash)
			{
				image = material->textureTable[l].u.image; // Hopefully our map
				break;
			}
		}

		return image;
	}

	game::native::GfxImage* extractImage(game::native::Material* material, const std::string& type) const
	{
		auto* image = this->findImage(material, type);

		if (!image)
		{
			return image;
		}

		// TODO: This is still wrong.
		if (image->mapType == 5 && false)
		{
			for (auto i = 0; i < 6; ++i)
			{
				IDirect3DSurface9* surface = nullptr;
				image->texture.cubemap->GetCubeMapSurface(D3DCUBEMAP_FACES(i), 0, &surface);

				if (surface)
				{
					std::string _name = utils::string::va("raw/mapdump/%s/textures/%s_%i.png", this->world_->baseName, image->name, i);
					D3DXSaveSurfaceToFileA(_name.data(), D3DXIFF_PNG, surface, nullptr, nullptr);
					surface->Release();
				}
			}
		}
		else
		{
			if (image && std::string(image->name).length() > 0)
			{
				std::string _name = utils::string::va("raw/mapdump/%s/textures/%s.png", this->world_->baseName, image->name);
				D3DXSaveTextureToFileA(_name.data(), D3DXIFF_PNG, image->texture.map, nullptr);
			}
		}

		return image;
	}

	void writeMaterial(game::native::Material* material)
	{
		std::string name = material->info.name;

		const auto pos = name.find_last_of('/');
		if (pos != std::string::npos)
		{
			name = name.substr(pos + 1);
		}

		this->object_.append(utils::string::va("usemtl %s\n", name.data()));
		this->object_.append("s off\n");

		auto* colorMap = this->extractImage(material, "colorMap");
		auto* normalMap = this->extractImage(material, "normalMap");
		auto* specularMap = this->extractImage(material, "specularMap");

		this->material_.append(utils::string::va("\nnewmtl %s\n", name.data()));
		this->material_.append("Ka 1.0000 1.0000 1.0000\n");
		this->material_.append("Kd 1.0000 1.0000 1.0000\n");
		this->material_.append("illum 1\n");
		this->material_.append(utils::string::va("map_Ka textures/%s.png\n", colorMap->name));
		this->material_.append(utils::string::va("map_Kd textures/%s.png\n", colorMap->name));

		if (specularMap)
		{
			this->material_.append(utils::string::va("map_Ks textures/%s.png\n", specularMap->name));
		}

		if (normalMap)
		{
			this->material_.append(utils::string::va("bump textures/%s.png\n", normalMap->name));
		}
	}

	void writeFaces()
	{
		console::info("Writing faces...\n");
		utils::io::create_directory(utils::string::va("raw/mapdump/%s/textures", this->world_->baseName));

		this->material_.append(utils::string::va("# Material count: %d\n", this->faces_.size()));

		this->object_.append("# Faces\n");

		for (const auto& material : this->facesOrder_)
		{
			this->writeMaterial(material);

			const auto& faces = this->getFaceList(material);
			for (const auto& index : faces.indices)
			{
				const int a = index.a;
				const int b = index.b;
				const int c = index.c;

				this->object_.append(utils::string::va("f %d/%d/%d %d/%d/%d %d/%d/%d\n", a, a, a, b, b, b, c, c, c));
			}

			this->object_.append("\n");
		}
	}
};

class map_dump final : public module
{
public:
	void post_start() override
	{

	}

	void post_load() override
	{
		command::add("dumpmap", [](command::params)
		{
			game::native::GfxWorld* world;
			game::native::DB_EnumXAssets_Internal(game::native::XAssetType::ASSET_TYPE_GFXWORLD, [](game::native::XAssetHeader header, void* world)
				{
					*reinterpret_cast<game::native::GfxWorld**>(world) = header.gfxWorld;
				}, &world, false);

			if (world && *game::native::CL_IsCgameInitialized)
			{
				console::info("Attempting to export Map '%s'\n", world->baseName);
				MapDumper dumper(world);
				dumper.dump();

				console::info("Map '%s' exported!\n", world->baseName);
			}
			else
			{
				console::info("No map loaded, unable to dump anything!\n");
			}
		});
	}
};



REGISTER_MODULE(map_dump)