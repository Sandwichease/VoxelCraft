
// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on
// glm = la bibliotheque de maths pour la 3d
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>
#include <vector>
// glm a deja un bruit de perlin
#include <glm/gtc/noise.hpp>

// include les cpp et h
#include "camera.h"
#include "chunk.h"
#include "shader.h"
// code du vertex shader en glsl
// il recoit une position et la place telle quelle a l'ecran
const char *vertexSource = R"(
#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 couleur;
out vec3 couleurVersFragment;

// les 3 matrices envoyees par le c++
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    // on applique les matrices de droite a gauche
    gl_Position = projection * view * model * vec4(position, 1.0);
    couleurVersFragment = couleur;
}
)";

// code du fragment shader en glsl
// il decide de la couleur de chaque pixel
const char *fragmentSource = R"(
#version 330 core
in vec3 couleurVersFragment;
out vec4 sortie;

void main() {
    sortie = vec4(couleurVersFragment, 1.0);
}
)";

// fonction appelee quand la fenetre change de taille
void quandFenetreRedimensionnee(GLFWwindow *fenetre, int largeur, int hauteur) {
  glViewport(0, 0, largeur, hauteur);
}

int main() {

  // on demarre glfw et on demande opengl 3.3 core
  if (!glfwInit()) {
    std::cout << "impossible d'initialiser GLFW" << std::endl;
    return -1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // on cree la fenetre
  GLFWwindow *fenetre =
      glfwCreateWindow(800, 600, "mon premier opengl", nullptr, nullptr);

  if (fenetre == nullptr) {
    std::cout << "impossible de creer la fenetre" << std::endl;
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(fenetre);

  // les fps
  glfwSwapInterval(0);

  // on charge les fonctions opengl avec glad
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "impossible de charger GLAD" << std::endl;
    glfwDestroyWindow(fenetre);
    glfwTerminate();
    return -1;
  }

  // callback de redimensionnement
  glfwSetFramebufferSizeCallback(fenetre, quandFenetreRedimensionnee);
  // on cache le curseur et on le garde dans la fenetre
  glfwSetInputMode(fenetre, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  // on dit a glfw d appeler notre fonction quand la souris bouge
  glfwSetCursorPosCallback(fenetre, quandSourisBouge);
  int largeurReelle, hauteurReelle;
  glfwGetFramebufferSize(fenetre, &largeurReelle, &hauteurReelle);
  glViewport(0, 0, largeurReelle, hauteurReelle);

  // compilation des shaders
  unsigned int vertexShader = compilerShader(GL_VERTEX_SHADER, vertexSource);

  unsigned int fragmentShader =
      compilerShader(GL_FRAGMENT_SHADER, fragmentSource);

  // creation du programme
  unsigned int programme = glCreateProgram();

  glAttachShader(programme, vertexShader);
  glAttachShader(programme, fragmentShader);
  glLinkProgram(programme);

  int succes;
  glGetProgramiv(programme, GL_LINK_STATUS, &succes);

  if (!succes) {
    char message[512];
    glGetProgramInfoLog(programme, 512, nullptr, message);
    std::cout << "erreur linking : " << message << std::endl;
  }

  // les shaders sont copies dans le programme
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  unsigned int vao, vbo;
  // clang-format on

  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);

  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  // attribut 0 = position
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);

  glEnableVertexAttribArray(0);

  // attribut 1 = couleur
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                        (void *)(3 * sizeof(float)));

  glEnableVertexAttribArray(1);

  // emplacement de la variable decalage dans le shader
  int emplacementDecalage = glGetUniformLocation(programme, "decalage");

  bool Collision = false;
  // active le test de profondeur
  // le gpu retient la distance de chaque pixel et garde le plus proche
  glEnable(GL_DEPTH_TEST);

  // emplacements des 3 matrices dans le shader
  int emplacementModel = glGetUniformLocation(programme, "model");
  int emplacementView = glGetUniformLocation(programme, "view");
  int emplacementProjection = glGetUniformLocation(programme, "projection");

  // pour mesurer le temps entre deux images
  float dernierTemps = 0.0f;

  // pour compter les images par seconde
  double dernierAffichage = glfwGetTime();
  int compteurImages = 0;
  // le gpu ignore les faces qui tournent le dos a la camera
  glEnable(GL_CULL_FACE);

  GLuint triangleQuery;
  glGenQueries(1, &triangleQuery);
  // on cree le monde une seule fois avant la boucle
  std::vector<Chunk> chunks;
  chunks.reserve(361);

  for (int cx = -16; cx <= 16; cx++) {
    for (int cz = -16; cz <= 16; cz++) {
      chunks.emplace_back(cx, cz);
    }
  }
  // pour chaque chunk on envoie son mesh au gpu une fois
  for (Chunk &chunk : chunks) {
    envoyerMeshAuGPU(chunk);
  }
  // on affiche la taille du mesh du premier chunk pour verifier le defi
  std::cout << "taille mesh : " << construireMesh(chunks[0]).size()
            << std::endl;
  std::cout << "faces dessus : " << compterFacesDessus(chunks[0]) << std::endl;

  // on affiche le bruit le long d une ligne
  for (int i = 0; i < 20; i++) {
    float a = i * 0.1f;
    std::cout << glm::perlin(glm::vec2(a, 0.9f)) << std::endl;
  }
  while (!glfwWindowShouldClose(fenetre)) {

    // temps ecoule depuis la derniere image
    // permet une vitesse identique quel que soit le nombre d images par seconde
    float tempsActuel = (float)glfwGetTime();
    float dt = tempsActuel - dernierTemps;
    dernierTemps = tempsActuel;

    // echap pour quitter car le curseur est cache
    if (glfwGetKey(fenetre, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
      glfwSetWindowShouldClose(fenetre, true);
    }

    // deplacement de la camera : 20.5 unites par seconde
    float vitesseCamera = 20.5f * dt;

    // avancer et reculer dans la direction du regard
    if (glfwGetKey(fenetre, GLFW_KEY_W) == GLFW_PRESS)
      cameraPos += vitesseCamera * cameraFront;
    if (glfwGetKey(fenetre, GLFW_KEY_S) == GLFW_PRESS)
      cameraPos -= vitesseCamera * cameraFront;

    // gauche et droite : cross donne la direction perpendiculaire au regard
    if (glfwGetKey(fenetre, GLFW_KEY_A) == GLFW_PRESS)
      cameraPos -=
          glm::normalize(glm::cross(cameraFront, cameraUp)) * vitesseCamera;
    if (glfwGetKey(fenetre, GLFW_KEY_D) == GLFW_PRESS)
      cameraPos +=
          glm::normalize(glm::cross(cameraFront, cameraUp)) * vitesseCamera;
    // lecture des evenements
    glfwPollEvents();

    // effacement de l'ecran
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    // on efface la couleur et la profondeur
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // dessin du triangle
    // on active le programme avant d envoyer des valeurs
    glUseProgram(programme);

    // temps ecoule en secondes depuis le lancement
    float temps = (float)glfwGetTime();

    // view : la camera est en cameraPos et regarde vers cameraPos + cameraFront
    glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

    // projection : perspective avec le ratio de la fenetre
    int largeur, hauteur;
    glfwGetFramebufferSize(fenetre, &largeur, &hauteur);
    float ratio = (float)largeur / (float)hauteur;
    float renderDistance = 10000.0f;
    glm::mat4 projection =
        glm::perspective(glm::radians(45.0f), ratio, 0.1f, renderDistance);

    // view et projection sont les memes pour tous les cubes
    // donc on les envoie une seule fois avant de dessiner
    glUniformMatrix4fv(emplacementView, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(emplacementProjection, 1, GL_FALSE,
                       glm::value_ptr(projection));

    glm::mat4 model = glm::mat4(1.0f);

    glUniformMatrix4fv(emplacementModel, 1, GL_FALSE, glm::value_ptr(model));
    glBeginQuery(GL_PRIMITIVES_GENERATED, triangleQuery);

    // on dessine tous les chunks du monde
    for (const Chunk &chunk : chunks) {

      // on sélectionne le VAO de ce chunk
      glBindVertexArray(chunk.vao);

      // on dessine tout son mesh
      glDrawArrays(GL_TRIANGLES, 0, chunk.nombreSommets);
    }

    glEndQuery(GL_PRIMITIVES_GENERATED);

    // on compte une image de plus
    compteurImages++;

    // une fois par seconde on affiche les mesures dans le titre
    if (tempsActuel - dernierAffichage >= 1.0) {
      // on lit le resultat seulement ici donc une fois par seconde
      GLuint triangles = 0;
      glGetQueryObjectuiv(triangleQuery, GL_QUERY_RESULT, &triangles);

      std::string titre = "fps : " + std::to_string(compteurImages) +
                          "      triangles : " + std::to_string(triangles) +
                          "position : " + std::to_string(cameraPos.x) + ", " +
                          std::to_string(cameraPos.y) + ", " +
                          std::to_string(cameraPos.z);
      glfwSetWindowTitle(fenetre, titre.c_str());
      compteurImages = 0;
      dernierAffichage = tempsActuel;
    }
    // affichage de l'image
    glfwSwapBuffers(fenetre);
  }

  // NETTOYAGE DES RESSOURCES
  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(1, &vbo);
  glDeleteProgram(programme);

  glfwDestroyWindow(fenetre);
  glfwTerminate();

  return 0;
}
