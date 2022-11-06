#include <iostream>
#include <string>
#include <unordered_map>

#include "transform2D.h"
#include "object2D.h"

#include "duck.h"

using namespace duck;

// Constructors
Duck::Duck() {
	this->alive = true;
}

Duck::~Duck() {
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

	// Iris
	Mesh *iris = object2D::CreateCircle("iris", glm::vec3(0, 0, 0), 50, this->iris_radius, this->iris_color, true);
	this->addMesh("iris", iris);

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
	modelMatrix *= transform2D::Rotate(this->wing_rot_angle); // wing animation
	modelMatrix *= transform2D::Translate(this->wing_hei, 0);
	modelMatrix *= transform2D::Rotate(RADIANS(90));
	return modelMatrix;
}

glm::mat3 Duck::wing_right_mat() {
	glm::mat3 modelMatrix = glm::mat3(1);
	modelMatrix *= transform2D::Translate(this->body_wid / 3, this->body_hei / 2);
	modelMatrix *= transform2D::Rotate(-this->wing_rot_angle); // wing animation
	modelMatrix *= transform2D::Rotate(RADIANS(-90));
	return modelMatrix;
}

glm::mat3 Duck::beak_mat() {
	glm::mat3 modelMatrix = glm::mat3(1);
	modelMatrix *= transform2D::Translate(this->head_radius * 1.1f, -this->head_radius / 3);
	modelMatrix *= transform2D::Translate(this->body_wid * this->head_body_wid_perc, this->body_hei / 2);
	modelMatrix *= transform2D::Rotate(RADIANS(-5));
	modelMatrix *= transform2D::Translate(-this->beak_wid / 2, -this->beak_hei / 2);

	glm::vec3 coord = this->general_matrix * modelMatrix * glm::vec3(this->beak_wid, this->beak_hei / 2, 1);
	this->beak_tip_x = coord.x;
	this->beak_tip_y = coord.y;

	return modelMatrix;
}

glm::mat3 Duck::eye_mat() {
	glm::mat3 modelMatrix = glm::mat3(1);
	modelMatrix *= transform2D::Translate(this->head_radius * 0.2f, this->head_radius * 0.2f);
	modelMatrix *= transform2D::Translate(this->body_wid * this->head_body_wid_perc, this->body_hei / 2);
	return modelMatrix;
}

glm::mat3 Duck::iris_mat() {
	glm::mat3 modelMatrix = glm::mat3(1);
	modelMatrix *= transform2D::Translate(this->head_radius * 0.2f, this->head_radius * 0.2f);
	modelMatrix *= transform2D::Translate(this->body_wid * this->head_body_wid_perc, this->body_hei / 2);
	return modelMatrix;
}

glm::mat3 Duck::bbox_mat() {
	glm::mat3 modelMatrix = glm::mat3(1);
	modelMatrix *= transform2D::Translate(0, -this->bbox_hei / 4);

	/*
	(x4,y4)     (x3,y3)
	-----------------
	|				|
	|    (cx,cy)    |
	|				|
	-----------------
	(x1,y1)      (x2,y2)
	*/

	glm::vec3 coord;
	coord = this->general_matrix * modelMatrix * glm::vec3(0, 0, 1);
	this->x1 = coord.x;
	this->y1 = coord.y;

	// (x2, y2)
	coord = this->general_matrix * modelMatrix * glm::vec3(this->bbox_wid, 0, 1);
	this->x2 = coord.x;
	this->y2 = coord.y;

	// (x3, y3)
	coord = this->general_matrix * modelMatrix * glm::vec3(this->bbox_wid, this->bbox_hei, 1);
	this->x3 = coord.x;
	this->y3 = coord.y;

	// (x4, y4)
	coord = this->general_matrix * modelMatrix * glm::vec3(0, this->bbox_hei, 1);
	this->x4 = coord.x;
	this->y4 = coord.y;

	// (cx, cy)
	coord = this->general_matrix * modelMatrix * glm::vec3(this->bbox_wid / 2, this->bbox_hei / 2, 1);
	this->cx = coord.x;
	this->cy = coord.y;

	return modelMatrix;
}
