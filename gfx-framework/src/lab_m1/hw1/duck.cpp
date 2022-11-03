#include <iostream>
#include <string>
#include <unordered_map>

#include "transform2D.h"
#include "object2D.h"

#include "duck.h"

//using namespace std;
using namespace duck;

// Constructors
Duck::Duck() {
	this->is_alive = true;
}


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


void Duck::generateMeshes() {
	Mesh* duck_body = object2D::CreateTriangle("duck_body", glm::vec3(0, 0, 0), this->body_hei, this->body_wid, glm::vec3(1, 0, 0), true);
	this->addMesh("body", duck_body);
}

