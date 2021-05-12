#include "model.h"

static unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma = false)
{
	std::string filename = std::string(path);
	filename = directory + filename;
	std::cout << "texture loaded: " << filename << std::endl;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

Model::Model(const char* path)
	: m_Directory("resources/textures/")
{
	loadModel(path);
}

void Model::CommitGeometry(const std::vector<float>& verts, VertexBitfield mask)
{
	m_Meshes.emplace_back(verts, mask);
}

void Model::CommitGeometry(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<Texture>& textures)
{
	m_Meshes.emplace_back(vertices, indices, textures);
}

void Model::CommitGeometry(const std::vector<float>& vertices, const std::vector<unsigned int>& indices, const std::vector<Texture>& textures)
{
	m_Meshes.emplace_back(vertices, indices, textures);
}

void Model::AssignTexturesToMesh(int idx, const std::vector<Texture>& textures)
{
	m_Meshes[idx].SetTextures(textures);
}

void Model::DrawElements(const Shader& shader) const
{
	for (unsigned int i = 0; i < m_Meshes.size(); i++)
		m_Meshes[i].DrawElements(shader);
}

void Model::DrawElements() const
{
	for (unsigned int i = 0; i < m_Meshes.size(); i++)
		m_Meshes[i].DrawElements();
}

void Model::DrawArrays() const
{
	for (unsigned int i = 0; i < m_Meshes.size(); i++)
		m_Meshes[i].DrawArrays();
}

void Model::DrawArrays(const Shader& shader) const
{
	for (unsigned int i = 0; i < m_Meshes.size(); i++)
		m_Meshes[i].DrawArrays(shader);
}

Vertex Model::GetVertexOfMeshAt(int meshIdx, int vertIdx)
{
	return m_Meshes[meshIdx].GetVertexAt(vertIdx);
}

void Model::loadModel(std::string path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || !scene->mRootNode || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
	{
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}

	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		m_Meshes.push_back(processMesh(mesh, scene));
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	//VERTICES LOAD
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		vertex.position.x = mesh->mVertices[i].x;
		vertex.position.y = mesh->mVertices[i].y;
		vertex.position.z = mesh->mVertices[i].z;

		vertex.normal.x = mesh->mNormals[i].x;
		vertex.normal.y = mesh->mNormals[i].y;
		vertex.normal.z = mesh->mNormals[i].z;

		if (mesh->mTextureCoords[0])
		{
			vertex.texCoords.x = mesh->mTextureCoords[0][i].x;
			vertex.texCoords.y = mesh->mTextureCoords[0][i].y;

		}
		else
			vertex.texCoords = glm::vec2(0.0f);

		vertices.push_back(vertex);
	}

	//INDICES LOAD
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	//TEXTURES LOAD
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

	}
	return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
	std::vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString path;
		mat->GetTexture(type, i, &path);
		//std::cout << path.C_Str() << std::endl;
		bool skip = false;
		for (unsigned int j = 0; j < m_TexturesLoaded.size(); j++)
		{
			if (std::strcmp(m_TexturesLoaded[j].path.data(), path.C_Str()) == 0)
			{
				textures.push_back(m_TexturesLoaded[j]);
				skip = true;
				break;
			}
		}
		if (!skip)
		{
			Texture texture;
			texture.id = TextureFromFile(path.C_Str(), m_Directory);
			texture.type = typeName;
			texture.path = path.C_Str();
			textures.push_back(texture);
			m_TexturesLoaded.push_back(texture);
		}
	}
	return textures;
}
