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
	Mesh* body = object2D::CreateTriangle("body", glm::vec3(0, 0, 0), this->body_wid, this->body_hei, this->body_color, true);
	this->addMesh("body", body);

	// Wings
	Mesh* wing_left = object2D::CreateTriangle("wing_left", glm::vec3(0, 0, 0), this->wing_wid, this->wing_hei, this->wing_color, true);
	this->addMesh("wing_left", wing_left);
	Mesh* wing_right = object2D::CreateTriangle("wing_right", glm::vec3(0, 0, 0), this->wing_wid, this->wing_hei, this->wing_color, true);
	this->addMesh("wing_right", wing_right);

	// Head
	Mesh *head = object2D::CreateCircle("head", glm::vec3(0, 0, 0), 50, this->head_radius, this->head_color, true);
	this->addMesh("head", head);
	
	// Beak
	Mesh *beak = object2D::CreateTriangle("beak", glm::vec3(0, 0, 0), this->beak_wid, this->beak_hei, this->beak_color, true);
	this->addMesh("beak", beak);

	// Eye
	Mesh *eye = object2D::CreateCircle("head", glm::vec3(0, 0, 0), 50, this->eye_radius, this->eye_color, true);
	this->addMesh("eye", eye);

	// Bounding box
	Mesh *bbox = object2D::CreateRectangle("bbox", glm::vec3(0, 0, 0), this->bbox_wid, this->bbox_hei, glm::vec3(1, 1, 1), false);
	this->addMesh("bbox", bbox);
}


glm::mat3 Duck::head_mat() {
	glm::mat3 modelMatrix = glm::mat3(1);
	modelMatrix *= transform2D::Translate(this->body_wid * this->head_body_wid_perc, this->body_hei / 2);
	return modelMatrix;
}

glm::mat3 Duck::body_mat() {
	glm::mat3 modelMatrix = glm::mat3(1);
	return modelMatrix;
}

glm::mat3 Duck::wing_left_mat() {
	glm::mat3 modelMatrix = glm::mat3(1);
	modelMatrix *= transform2D::Translate(this->body_wid / 3, this->body_hei / 2);
	modelMatrix *= transform2D::Translate(this->wing_hei, 0);
	modelMatrix *= transform2D::Rotate(90.0f * M_PI / 180.0f);
	return modelMatrix;
}

glm::mat3 Duck::wing_right_mat() {
	glm::mat3 modelMatrix = glm::mat3(1);
	modelMatrix *= transform2D::Translate(this->body_wid / 3, this->body_hei / 2);
	modelMatrix *= transform2D::Rotate(-90.0f * M_PI / 180.0f);
	return modelMatrix;
}

glm::mat3 Duck::beak_mat() {
	glm::mat3 modelMatrix = glm::mat3(1);
	modelMatrix *= transform2D::Translate(this->head_radius * 1.1f, -this->head_radius / 3);
	modelMatrix *= transform2D::Translate(this->body_wid * this->head_body_wid_perc, this->body_hei / 2);
	modelMatrix *= transform2D::Translate(-this->beak_wid / 2, -this->beak_hei / 2);
	return modelMatrix;
}

glm::mat3 Duck::eye_mat() {
	glm::mat3 modelMatrix = glm::mat3(1);
	modelMatrix *= transform2D::Translate(this->head_radius * 0.2f, this->head_radius * 0.2f);
	modelMatrix *= transform2D::Translate(this->body_wid * this->head_body_wid_perc, this->body_hei / 2);
	return modelMatrix;
}

glm::mat3 Duck::bbox_mat() {
	glm::mat3 modelMatrix = glm::mat3(1);
	modelMatrix *= transform2D::Translate(0, -this->bbox_hei / 4);

	glm::vec3 coord = glm::vec3(0, 0, 1);
	coord = this->general_matrix * modelMatrix * coord;
	this->left_x = coord.x;
	this->left_y = coord.y;

	return modelMatrix;
}
