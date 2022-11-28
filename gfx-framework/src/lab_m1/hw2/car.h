#pragma once

namespace car {
    class Car {
        public:
            Car();
            ~Car();

            // bool isOnTrack = true;
            glm::vec3 center = glm::vec3(0, 0.5, 0);

            std::unordered_map<std::string, Mesh *> meshes;
            void addMesh(std::string mesh_name, Mesh *mesh);
            void generateMeshes();
	};
}
