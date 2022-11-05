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

		// Matrices
		glm::mat3 general_matrix;
		glm::mat3 head_mat();
		glm::mat3 body_mat();
		glm::mat3 wing_left_mat();
		glm::mat3 wing_right_mat();
		glm::mat3 beak_mat();
		glm::mat3 eye_mat();
		glm::mat3 bbox_mat();

		// Colors
		glm::vec3 head_color = glm::vec3(48,  87,  55)  / 255.0f;
		glm::vec3 body_color = glm::vec3(88,  58,  38)  / 255.0f;
		glm::vec3 wing_color = glm::vec3(88,  58,  38)  / 255.0f;
		glm::vec3 beak_color = glm::vec3(231, 204, 57)  / 255.0f;
		glm::vec3 eye_color  = glm::vec3(255, 255, 255) / 255.0f;

		// Sizes
		float head_radius = 30,  head_body_wid_perc = 0.9f;
		float body_wid	  = 200, body_hei			= 70;
		float wing_wid	  = 80,  wing_hei			= 50;
		float beak_wid	  = 30,  beak_hei			= 15;
		float eye_radius  = 4;

		// Bounding box
		float bbox_wid	  = body_wid + head_radius * head_body_wid_perc * 1.1f;
		float bbox_hei    = body_hei + wing_wid;
		float left_x      = 0;
		float left_y	  = 0;
		float angle       = 0;
	};
}
