#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <vector>
#include <iostream>

#include "mesh.h"
#include "stb_image.h"

namespace kataevic
{

	class Model
	{
	public:
		Model() = default;
		Model(const char* path);
		void CommitGeometry(const std::vector<float>& verts, VertexBitfield mask = 0x0);
		void CommitGeometry(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<Texture>& textures);
		void CommitGeometry(const std::vector<float>& vertices, const std::vector<unsigned int>& indices, const std::vector<Texture>& textures);
		void AssignTexturesToMesh(int idx, const std::vector<Texture>& textures);
		void DrawElements(const Shader& shader) const;
		void DrawElements() const;
		void DrawArrays() const;
		void DrawArrays(const Shader& shader) const;
		Vertex GetVertexOfMeshAt(int meshIdx, int vertIdx);

	private:
		void loadModel(std::string path);
		void processNode(aiNode* node, const aiScene* scene);
		Mesh processMesh(aiMesh* mesh, const aiScene* scene);
		std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);

	private:
		std::vector<Mesh> m_Meshes;
		std::string m_Directory;
		std::vector<Texture> m_TexturesLoaded;
	};

}