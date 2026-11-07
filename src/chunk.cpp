// chunk.cpp
#include "chunk.h"
#include <cmath>

// le chunk:: devant dit que ce constructeur appartient a la structure chunk
Chunk::Chunk(int cx, int cz) {
  chunkX = cx;
  chunkZ = cz;

  for (int x = 0; x < SIZE; x++) {
    for (int z = 0; z < SIZE; z++) {

      int hauteur = 3 + (int)(2.0f * sin((x + cx * SIZE) * 0.5f) +
                              2.0f * cos((z + cz * SIZE) * 0.5f));

      for (int y = 0; y < hauteur; y++) {
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

int construireMesh(const Chunk &chunk) {}
