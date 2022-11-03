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
	// Body
	Mesh* body = object2D::CreateTriangle("body", glm::vec3(0, 0, 0), this->body_wid, this->body_hei, glm::vec3(1, 0, 0), true);
	this->addMesh("body", body);

	// Wings
	Mesh* wing_left = object2D::CreateTriangle("wing_left", glm::vec3(0, 0, 0), this->wing_wid, this->wing_hei, glm::vec3(0, 1, 0), true);
	this->addMesh("wing_left", wing_left);
	Mesh* wing_right = object2D::CreateTriangle("wing_right", glm::vec3(0, 0, 0), this->wing_wid, this->wing_hei, glm::vec3(0, 1, 0), true);
	this->addMesh("wing_right", wing_right);

	// Head
	Mesh *head = object2D::CreateCircle("head", glm::vec3(0, 0, 0), this->head_radius, glm::vec3(0, 0, 1), true);
	this->addMesh("head", head);
	
	// Beak
}

glm::mat3 Duck::wing_left_mat() {
	glm::mat3 modelMatrix = glm::mat3(1);
	modelMatrix *= transform2D::Translate(this->body_wid / 2, this->body_hei / 2);
	modelMatrix *= transform2D::Translate(this->wing_hei / 2, 0);
	modelMatrix *= transform2D::Rotate(90.0f * M_PI / 180.0f);
	return modelMatrix;
}

glm::mat3 Duck::wing_right_mat() {
	glm::mat3 modelMatrix = glm::mat3(1);
	modelMatrix *= transform2D::Translate(this->body_wid / 2, this->body_hei / 2);
	modelMatrix *= transform2D::Translate(-this->wing_hei / 2, 0);
	modelMatrix *= transform2D::Rotate(-90.0f * M_PI / 180.0f);
	return modelMatrix;
}

