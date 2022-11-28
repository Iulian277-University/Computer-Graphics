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
	};
}
