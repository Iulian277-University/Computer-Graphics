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

		// Sizes
		int body_hei	= 100, body_wid = 200;
		int wing_hei	= 25,  wing_wid = 50;
		int head_radius = 25;
		int beak_hei	= 5,   beak_wid = 10;
	};

}