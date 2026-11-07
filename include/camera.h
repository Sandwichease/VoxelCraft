// camera.h
#pragma once
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
// position de la camera dans le monde

extern glm::vec3 cameraPos;
extern glm::vec3 cameraFront;
extern glm::vec3 cameraUp;

extern float yaw;
extern float pitch;

extern float dernierX;
extern float dernierY;

extern bool premiereSouris;

void quandSourisBouge(GLFWwindow *fenetre, double xpos, double ypos);
