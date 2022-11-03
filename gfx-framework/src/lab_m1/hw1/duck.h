#pragma once

#include "components/simple_scene.h"

class Duck {
// private:

protected:
	int id;
	bool is_alive;
	std::unordered_map<std::string, Mesh *> meshes; // <"head": head_mesh, "body": body_mesh, ...>

public:
	Duck(int id);
	~Duck();

	int getId();
	bool isAlive();
	std::unordered_map<std::string, Mesh *> getMeshes();

	void setId(int id);
	void setIsAlive(bool is_alive);
	void addMesh(std::string mesh_name, Mesh *mesh);

	void render();
};

