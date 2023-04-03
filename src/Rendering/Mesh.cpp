#include <Rendering/Mesh.hpp>
//#include <assimp/Importer.hpp>
//#include <assimp/scene.h>
//#include <assimp/postprocess.h>

namespace Stardust_Celeste::Rendering {

    Mesh<Rendering::Vertex>* LoadMeshFromFile(std::string filename) {
        /*
        Assimp::Importer importer;

        const aiScene *scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_FlipUVs);

        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            auto estring = importer.GetErrorString();
            SC_CORE_ERROR("Assimp Failed to Load: {}, with error {}", filename, estring);
            return nullptr;
        }

        auto base_mesh = scene->mMeshes[0];

        auto final_mesh = new Mesh<Rendering::Vertex>();

        for(size_t i = 0; i < base_mesh->mNumVertices; i++) {
            Rendering::Vertex v;

            v.x = base_mesh->mVertices[i].x;
            v.y = base_mesh->mVertices[i].y;
            v.z = base_mesh->mVertices[i].z;

            v.color = Rendering::Color{{255, 255, 255, 255}};

            v.u = base_mesh->mTextureCoords[0][i].x;
            v.v = base_mesh->mTextureCoords[0][i].y;

            final_mesh->vertices.push_back(v);
        }

        for(size_t i = 0; i < base_mesh->mNumFaces; i++)
        {
            aiFace face = base_mesh->mFaces[i];
            for(unsigned int j = 0; j < face.mNumIndices; j++)
                final_mesh->indices.push_back(face.mIndices[j]);
        }

        final_mesh->setup_buffer();

        return final_mesh;
         */
    }
}