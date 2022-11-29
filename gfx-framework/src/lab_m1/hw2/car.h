#pragma once

namespace car {
    class Car {
        public:
            Car();
            ~Car();

            // bool isOnTrack = true;
            glm::vec3 center = glm::vec3(1, 1, 1);

            std::unordered_map<std::string, Mesh *> meshes;
            void addMesh(std::string mesh_name, Mesh *mesh);
            void generateMeshes();
	
            glm::vec3 color = glm::vec3(21, 255, 251) / 255.0f;
    };
}
