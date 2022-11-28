#pragma once

namespace environment {
    class Environment {
        public:
            Environment();
            ~Environment();

            std::unordered_map<std::string, Mesh *> meshes;
            std::unordered_map<std::string, Shader *> shaders;

            void addMesh(std::string mesh_name, Mesh *mesh);
            void addShader(std::string shader_name, Shader *shader);

            void generateMeshes();
            void generateShaders();

            void generateTrack();
            void generateTrackMesh(std::vector<glm::vec3> trackPoints, glm::vec3 color);

            // Vector of triangles, where each triangle is a vector of 3 points
            std::vector<std::vector<glm::vec3>> triangles;

            glm::vec3 skyColor    = glm::vec3(141, 210, 231) / 255.0f;
            glm::vec3 trackColor  = glm::vec3(50,   50,  50) / 255.0f;
            glm::vec3 groundColor = glm::vec3(130, 214,  84) / 255.0f;
	};
}
