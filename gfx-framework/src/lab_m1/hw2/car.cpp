#include <iostream>
#include <string>
#include <unordered_map>

#include "core/gpu/mesh.h"
#include "core/managers/resource_path.h"
#include "car.h"

using namespace std;
using namespace car;

// Constructors
Car::Car() {}
Car::~Car() {}

void Car::addMesh(std::string mesh_name, Mesh *mesh) {
    meshes[mesh_name] = mesh;
}

void Car::generateMeshes() {
    Mesh *car = new Mesh("car");
    car->LoadMesh(RESOURCE_PATH::MODELS + "/primitives", "box.obj");
    addMesh("car", car);
}
