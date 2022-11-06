#include <iostream>
#include <string>
#include <unordered_map>

#include "transform2D.h"
#include "object2D.h"

#include "ui.h"

using namespace ui;

// Constructors
Ui::Ui() {
}

Ui::Ui(int total_lives, int total_bullets) {
	this->total_lives   = total_lives;
	this->curr_lives    = total_lives;
	this->total_bullets = total_bullets;
	this->curr_bullets  = total_bullets;
}

Ui::~Ui() {
}

glm::mat3 Ui::life_mat(int life_idx) {
	glm::mat3 modelMatrix = glm::mat3(1);
	modelMatrix *= transform2D::Translate(life_idx * (2 * this->life_radius + this->lives_padding), 0);
	modelMatrix *= transform2D::Translate(this->lives_pos_x, this->lives_pos_y);
	return modelMatrix;
}

glm::mat3 Ui::bullets_mat(int bullet_idx) {
	glm::mat3 modelMatrix = glm::mat3(1);
	modelMatrix *= transform2D::Translate(bullet_idx * (2 * this->bullet_wid + this->bullets_padding), 0);
	modelMatrix *= transform2D::Translate(this->bullets_pos_x, this->bullets_pos_y);
	modelMatrix *= transform2D::Translate(-this->bullet_wid / 2, -this->bullet_hei / 2);
	return modelMatrix;
}

void Ui::addMesh(std::string mesh_name, Mesh *mesh) {
	this->meshes[mesh_name] = mesh;
}

void Ui::generateMeshes() {
	// Life
	Mesh* life = object2D::CreateCircle("life", glm::vec3(0, 0, 0), 25, this->life_radius, this->life_color, true);
	this->addMesh("life", life);

	// Bullet
	Mesh *bullet = object2D::CreateRectangle("bullet", glm::vec3(0, 0, 0), this->bullet_wid, this->bullet_hei, glm::vec3(0, 0, 1), false);
	this->addMesh("bullet", bullet);
}
