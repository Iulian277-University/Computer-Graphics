#pragma once

namespace ui {
	class Ui {
	public:
		Ui();
		Ui(int total_lives, int total_bullets);
		~Ui();

		void generateMeshes();

		/* All positions (*_x, *_y) are relative to the center of the object */

		// <"head": head_mesh, "body": body_mesh, ...>
		std::unordered_map<std::string, Mesh *> meshes;
		void addMesh(std::string mesh_name, Mesh *mesh);

		// Lives
		int   total_lives		 =      3;
		int   curr_lives		 =		3;
		float lives_pos_x		 =  50.0f;
		float lives_pos_y		 = 650.0f;
		float life_radius		 =  20.0f;
		float lives_padding		 =  10.0f;
		bool  decremented_lives  = false;
		glm::vec3 life_color	 = glm::vec3(255, 0, 0) / 255.0f;
		glm::mat3 life_mat(int life_idx);

		// Bullets
		int	  total_bullets		=	   3;
		int	  curr_bullets		=	   3;
		float bullets_pos_x		=  lives_pos_x;
		float bullets_pos_y		=  lives_pos_y - 3 * life_radius;
		float bullet_wid		=  20.0f;
		float bullet_hei		=  40.0f;
		float bullets_padding   =  lives_padding;
		glm::mat3 bullets_mat(int bullet_idx);

		// Score
		float score_wireframe_wid =  200.0f;
		float score_wireframe_hei =   20.0f;
		float curr_score_wid	  =    0.0f;
		float curr_score_hei	  =   20.0f;
		float score_pos_x		  = 1100.0f;
		float score_pos_y		  =   20.0f;

		//
	};
}
