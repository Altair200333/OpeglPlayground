#pragma once
#include <iostream>
#include <random>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Material.h"
#include "Mesh.h"

class MeshLoader final
{
    std::string directory;
public:
    struct LoadedModel
    {
        std::shared_ptr<Mesh> mesh;
        std::shared_ptr<Material> material;
    };
    std::vector<LoadedModel> models;
    std::vector<Texture> cachedTextures;
	
    static std::string getDir(const std::string& path)
    {
        std::string directory;
        const size_t last_slash_idx = path.rfind('/');
        if (std::string::npos != last_slash_idx)
        {
            directory = path.substr(0, last_slash_idx);
        }
        return directory;
    }
    std::string modelPath;
    std::vector<LoadedModel> loadModel(const std::string& path)
    {
        modelPath = path;
        Assimp::Importer import;
        const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
            return {};
        }
        directory = path.substr(0, path.find_last_of('/'));

        processNode(scene->mRootNode, scene);
        return models;
    }
    void processNode(aiNode* node, const aiScene* scene)
    {
        // process each mesh located at the current node
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            // the node object only contains indices to index the actual objects in the scene. 
            // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            models.push_back(processMesh(mesh, scene));
        }
        // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene);
        }

    }
    LoadedModel processMesh(aiMesh* mesh, const aiScene* scene)
    {
        // data to fill
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;

        // walk through each of the mesh's vertices
        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
        	
            QVector3D vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
            // positions
            vector.setX(mesh->mVertices[i].x);
            vector.setY(mesh->mVertices[i].y);
            vector.setZ(mesh->mVertices[i].z);
            vertex.vertex = { vector.x(), vector.y(), vector.z() };
            // normals
            if (mesh->HasNormals())
            {
                vector.setX(mesh->mNormals[i].x);
                vector.setY(mesh->mNormals[i].y);
                vector.setZ(mesh->mNormals[i].z);
                vertex.normal = { vector.x(), vector.y(), vector.z() };
            }
          
            // texture coordinates
            if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
            {
                QVector2D vec;
                // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
                // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
                vec.setX(mesh->mTextureCoords[0][i].x);
                vec.setY(mesh->mTextureCoords[0][i].y);
                vertex.TexCoords = { vec.x(), vec.y() };
                // tangent
                vertex.Tangent = { mesh->mTangents[i].x ,mesh->mTangents[i].y ,mesh->mTangents[i].z};
                // bitangent
                vertex.Bitangent = { mesh->mBitangents[i].x , mesh->mBitangents[i].y , mesh->mBitangents[i].z };
            }
            if (mesh->HasVertexColors(0))
            {
                vertex.color = { mesh->mColors[0][i].r, mesh->mColors[0][i].g, mesh->mColors[0][i].b };
            }
            else
            {
                std::uniform_real_distribution<float> dist(0.0, 1.0);
                std::default_random_engine  rd(mesh->mVertices[i].x*10+ mesh->mVertices[i].y * 10 + mesh->mVertices[i].z * 10);
                vertex.color = {dist(rd), dist(rd), dist(rd) };
            }
            vertices.push_back(vertex);
        }
        // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            // retrieve all indices of the face and store them in the indices vector
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }
        // process materials
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        aiColor3D color(0.f, 0.f, 0.f);
        material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
        const QColor albedo = { static_cast<int>(color.r * 255), static_cast<int>(color.g * 255), static_cast<int>(color.b * 255)};

        // 1. textures maps
        std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        // 2. specular maps
        std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        // 3. normal maps
        std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
        // 4. height maps
        std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
    	
        //std::vector<Texture> roughnessMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE_ROUGHNESS, "texture_roughness");

        std::shared_ptr<Material> mat =std::make_shared<Material>(albedo);
    	
        mat->textures = diffuseMaps;
        mat->normal = normalMaps;
        mat->specular = specularMaps;

        return { std::make_shared<Mesh>(vertices, indices), mat };
    }
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName)
    {
        std::vector<Texture> textures;
        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);
            // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
            bool skip = false;
            for (unsigned int j = 0; j < cachedTextures.size(); j++)
            {
                if (std::strcmp(cachedTextures[j].path.data(), str.C_Str()) == 0)
                {
                    textures.push_back(cachedTextures[j]);
                    skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                    break;
                }
            }
            if (!skip)
            {   // if texture hasn't been loaded already, load it
                Texture texture;
                std::string path = getDir(modelPath)+'/' + std::string(str.C_Str());
                texture.texture = new QOpenGLTexture(QImage(QString(path.c_str())));
                texture.type = typeName;
                texture.path = str.C_Str();
                texture.setFilterMode(Texture::FilterMode::Anisotropic);
                textures.push_back(texture);
                cachedTextures.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
            }
        }
        return textures;
    }
};