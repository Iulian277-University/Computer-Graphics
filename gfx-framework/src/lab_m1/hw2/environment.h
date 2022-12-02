#pragma once

namespace environment {
    class Environment {
        public:
            Environment();
            ~Environment();

            std::unordered_map<std::string, Mesh *> meshes;
            std::unordered_map<std::string, Shader *> shaders;

            void addMesh(std::string mesh_name, Mesh *mesh);

            void generateMeshes();

            void generateTrack();
            std::vector<VertexFormat> generateTrackMesh(std::vector<glm::vec3> trackPoints);

            void generateCube(const char *name);

            bool isOnTrack(glm::vec3 center);
            bool isOnCollision(glm::vec3 center);

			void generateObstacles();

            // Vector of track middle points
            std::vector<glm::vec3> trackMiddlePoints;

            // Vector of triangles, where each triangle is a vector of 3 points
            std::vector<std::vector<glm::vec3>> triangles;

            // Vector of trees' positions
            std::vector<glm::vec3> treePositions;
            std::vector<float> treeSizes;

            // Vector of obstacles' positions
            std::vector<int> obstacleIdxs;
            std::vector<glm::vec3> obstaclePositions;
            std::vector<glm::vec3> obstacleOffsets;
            std::vector<glm::vec3> obstacleColors;
            std::vector<float> obstacleScales;

            // Constants
            float trackScale    =  7.0f;
			float max_obstacles = 20.0f;

            // Colors
            glm::vec3 skyColor    = glm::vec3(141, 210, 231) / 255.0f;
            glm::vec3 trackColor  = glm::vec3(50,   50,  50) / 255.0f;
            glm::vec3 groundColor = glm::vec3(130, 214,  84) / 255.0f;

            glm::vec3 trunkColor  = glm::vec3(0.45f, 0.35f, 0.28f);
            glm::vec3 crownColor  = glm::vec3(0.34f, 0.51f, 0.28f);
	};
}
