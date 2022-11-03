#include <iostream>
#include <string>
#include <unordered_map>
#include "core/gpu/mesh.h"
#include "duck.h"

// Constructors
Duck::Duck(int id) {
    this->id = id;
    this->is_alive = true;
}

Duck::~Duck() {
}


// Getters
int Duck::getId() {
    return this->id;
}

bool Duck::isAlive() {
    return this->is_alive;
}

std::unordered_map<std::string, Mesh *> Duck::getMeshes() {
	return this->meshes;
}


// Setters
void Duck::setId(int id) {
    this->id = id;
}

void Duck::setIsAlive(bool is_alive) {
    this->is_alive = is_alive;
}


void Duck::addMesh(std::string mesh_name, Mesh *mesh) {
	this->meshes[mesh_name] = mesh;
}


//
void Duck::render() {
	for (const auto& pair : this->getMeshes()) {
		auto mesh = pair.second;
		// SimpleScene::RenderMesh2D(mesh, shaders["VertexColor"], glm::mat3(1));
	}
}
