#include "camera.h"

// DEFINITIONS DES VARIABLES

glm::vec3 cameraPos(0.0f, 12.0f, 3.0f);
glm::vec3 cameraFront(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);

float yaw = -90.0f;
float pitch = 0.0f;

float dernierX = 400.0f;
float dernierY = 300.0f;

bool premiereSouris = true;

// CALLBACK SOURIS

void quandSourisBouge(GLFWwindow *fenetre, double xpos, double ypos) {
  // au premier appel on memorise juste la position
  if (premiereSouris) {
    dernierX = (float)xpos;
    dernierY = (float)ypos;
    premiereSouris = false;
  }
  // de combien la souris a bouge depuis la derniere fois
  // y est inverse car l ecran compte vers le bas

  float decalageX = (float)xpos - dernierX;
  float decalageY = dernierY - (float)ypos;

  dernierX = (float)xpos;
  dernierY = (float)ypos;

  // la sensibilite regle la vitesse de rotation

  float sensibilite = 0.1f;

  yaw += decalageX * sensibilite;
  pitch += decalageY * sensibilite;

  // on limite le pitch pour ne pas faire de tour complet vers le haut
  if (pitch > 89.0f)
    pitch = 89.0f;

  if (pitch < -89.0f)
    pitch = -89.0f;

  // on transforme les 2 angles en une direction 3d
  glm::vec3 direction;

  direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
  direction.y = sin(glm::radians(pitch));
  direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
  // normalize fait une fleche de longueur 1

  cameraFront = glm::normalize(direction);
}
