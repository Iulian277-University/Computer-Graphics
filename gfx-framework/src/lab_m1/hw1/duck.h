#pragma once

namespace duck {
	class Duck {
		// private:

	protected:
		int id;
		bool is_alive;
	
		std::unordered_map<std::string, Mesh *> meshes; // <"head": head_mesh, "body": body_mesh, ...>

	public:
		Duck();
		Duck(int id);
		~Duck();

		int getId();
		bool isAlive();
		std::unordered_map<std::string, Mesh *> getMeshes();

		void setId(int id);
		void setIsAlive(bool is_alive);
		void addMesh(std::string mesh_name, Mesh *mesh);

		void generateMeshes();

		glm::mat3 wing_left_mat();
		glm::mat3 wing_right_mat();

		// Sizes
		float body_hei	  = 100, body_wid = 200;
		float wing_hei	  = 50,  wing_wid = 100;
		float head_radius = 50;
		float beak_hei	  = 5,   beak_wid = 10;

		// Positions

	};

}