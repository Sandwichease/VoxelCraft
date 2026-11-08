// chunk.cpp
#include "chunk.h"
#include <algorithm>
#include <cmath>
#include <glad/glad.h>
#include <vector>
// le bruit de perlin vient de glm pas
#include <glm/gtc/noise.hpp>

// le chunk:: devant dit que ce constructeur appartient a la structure chunk
Chunk::Chunk(int cx, int cz) {
  chunkX = cx;
  chunkZ = cz;

  for (int x = 0; x < SIZE; x++) {
    for (int z = 0; z < SIZE; z++) {

      int worldX = chunkX * SIZE + x;
      int worldZ = chunkZ * SIZE + z;

      for (int y = 0; y < hauteurTerrain(worldX, worldZ); y++) {
        blocks[x][z][y] = 1;
      }
    }
  }
}

// renvoie true s il y a un bloc a cette position dans le chunk
bool estSolide(const Chunk &chunk, int x, int y, int z) {
  // sous le monde on dit que c est plein car on ne le verra jamais
  if (y < 0)
    return true;
  // sur les cotes du chunk ou au dessus on dit que c est de l air
  if (x < 0 || x >= Chunk::SIZE)
    return false;
  if (z < 0 || z >= Chunk::SIZE)
    return false;
  if (y >= 64)
    return false;
  return chunk.blocks[x][z][y] != 0;
}

int compterFacesDessus(const Chunk &chunk) {
  int compteur = 0;

  for (int x = 0; x < 16; x++) {
    for (int z = 0; z < 16; z++) {
      for (int y = 0; y < 64; y++) {
        if (estSolide(chunk, x, y, z) && !estSolide(chunk, x, y + 1, z)) {
          compteur++;
        }
      }
    }
  }

  return compteur;
}

// met dans un tableaux la position de chaques face visible au dessus d un
// cube.Je reprend la formule chunk.chunkX * 16 + x, y,chunk.chunkZ * 16 + z));
// qui permet de calculer au dela des chunks locals
std::vector<float> positionsFacesDessus(const Chunk &chunk) {
  std::vector<float> position;
  for (int x = 0; x < 16; x++) {
    for (int z = 0; z < 16; z++) {
      for (int y = 0; y < 64; y++) {
        if (estSolide(chunk, x, y, z) && !estSolide(chunk, x, y + 1, z)) {
          position.push_back(chunk.chunkX * 16 + x);
          position.push_back(y);
          position.push_back(chunk.chunkZ * 16 + z);
        }
      }
    }
  }
  return position;
}

void ajouterFace(std::vector<float> &vecteur, const std::vector<float> &face,
                 int x, int y, int z) {
  for (size_t i = 0; i < face.size(); i += 6) {

    vecteur.push_back(x + face[i]);
    vecteur.push_back(y + face[i + 1]);
    vecteur.push_back(z + face[i + 2]);

    vecteur.push_back(face[i + 3]);
    vecteur.push_back(face[i + 4]);
    vecteur.push_back(face[i + 5]);
  }
}

std::vector<float> construireMesh(const Chunk &chunk) {

  std::vector<float> vecteur;

  const std::vector<float> FaceHaut = {
      -0.5f, 0.5f, -0.5f, 1,    0,    1,     0.5f,  0.5f, 0.5f,
      1,     0,    1,     0.5f, 0.5f, -0.5f, 1,     0,    1,

      -0.5f, 0.5f, -0.5f, 1,    0,    1,     -0.5f, 0.5f, 0.5f,
      1,     0,    1,     0.5f, 0.5f, 0.5f,  1,     0,    1};

  const std::vector<float> FaceBas = {
      -0.5f, -0.5f, -0.5f, 1,     0,     0,    0.5f, -0.5f, -0.5f,
      1,     0,     0,     0.5f,  -0.5f, 0.5f, 1,    0,     0,

      -0.5f, -0.5f, -0.5f, 1,     0,     0,    0.5f, -0.5f, 0.5f,
      1,     0,     0,     -0.5f, -0.5f, 0.5f, 1,    0,     0};

  const std::vector<float> FaceAvant = {
      -0.5f, -0.5f, 0.5f, 0,     1,    0,    0.5f, -0.5f, 0.5f,
      0,     1,     0,    0.5f,  0.5f, 0.5f, 0,    1,     0,

      -0.5f, -0.5f, 0.5f, 0,     1,    0,    0.5f, 0.5f,  0.5f,
      0,     1,     0,    -0.5f, 0.5f, 0.5f, 0,    1,     0};

  const std::vector<float> FaceArriere = {
      -0.5f, -0.5f, -0.5f, 0,    0,     1,     0.5f,  0.5f, -0.5f,
      0,     0,     1,     0.5f, -0.5f, -0.5f, 0,     0,    1,

      -0.5f, -0.5f, -0.5f, 0,    0,     1,     -0.5f, 0.5f, -0.5f,
      0,     0,     1,     0.5f, 0.5f,  -0.5f, 0,     0,    1};

  const std::vector<float> FaceDroite = {
      0.5f, -0.5f, -0.5f, 1,    1,     0,    0.5f, 0.5f, 0.5f,
      1,    1,     0,     0.5f, -0.5f, 0.5f, 1,    1,    0,

      0.5f, -0.5f, -0.5f, 1,    1,     0,    0.5f, 0.5f, -0.5f,
      1,    1,     0,     0.5f, 0.5f,  0.5f, 1,    1,    0};

  const std::vector<float> FaceGauche = {
      -0.5f, -0.5f, -0.5f, 0,     1,    1,     -0.5f, -0.5f, 0.5f,
      0,     1,     1,     -0.5f, 0.5f, 0.5f,  0,     1,     1,

      -0.5f, -0.5f, -0.5f, 0,     1,    1,     -0.5f, 0.5f,  0.5f,
      0,     1,     1,     -0.5f, 0.5f, -0.5f, 0,     1,     1};

  for (int x = 0; x < 16; x++) {
    for (int z = 0; z < 16; z++) {
      for (int y = 0; y < 64; y++) {

        if (!estSolide(chunk, x, y, z))
          continue;

        int worldX = chunk.chunkX * 16 + x;
        int worldZ = chunk.chunkZ * 16 + z;

        if (!estSolide(chunk, x, y + 1, z))
          ajouterFace(vecteur, FaceHaut, worldX, y, worldZ);

        if (!estSolide(chunk, x, y - 1, z))
          ajouterFace(vecteur, FaceBas, worldX, y, worldZ);

        if (!estSolide(chunk, x, y, z + 1))
          ajouterFace(vecteur, FaceAvant, worldX, y, worldZ);

        if (!estSolide(chunk, x, y, z - 1))
          ajouterFace(vecteur, FaceArriere, worldX, y, worldZ);

        if (!estSolide(chunk, x + 1, y, z))
          ajouterFace(vecteur, FaceDroite, worldX, y, worldZ);

        if (!estSolide(chunk, x - 1, y, z))
          ajouterFace(vecteur, FaceGauche, worldX, y, worldZ);
      }
    }
  }

  return vecteur;
}

void envoyerMeshAuGPU(Chunk &chunk) {
  // on construit les sommets de ce chunk
  std::vector<float> mesh = construireMesh(chunk);

  // 6 nombres par sommet donc on divise par 6
  chunk.nombreSommets = mesh.size() / 6;

  // on crée la notice et la boîte
  glGenVertexArrays(1, &chunk.vao);
  glGenBuffers(1, &chunk.vbo);

  // on les sélectionne
  glBindVertexArray(chunk.vao);
  glBindBuffer(GL_ARRAY_BUFFER, chunk.vbo);

  // on remplit la boîte avec le vecteur
  glBufferData(GL_ARRAY_BUFFER, mesh.size() * sizeof(float), mesh.data(),
               GL_STATIC_DRAW);

  // la notice : position puis couleur
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
}

float hauteurTerrain(int worldX, int worldZ) {
  float amplitude = 40.0f;
  float frequence = 0.01f;
  float centre = 30.0f;

  float bruit = glm::perlin(glm::vec2(worldX * frequence, worldZ * frequence));

  int hauteur = centre + bruit * amplitude;
  return std::clamp(hauteur, 1, 63);
}
