// chunk.h
#pragma once
#include <vector>

struct Chunk {
  int chunkX;
  int chunkZ;

  static const int SIZE = 16;

  // le = {} remplit tout le tableau de 0 au depart (0 = air)
  int blocks[SIZE][SIZE][64] = {};

  // declaration du constructeur : le code est dans chunk.cpp
  Chunk(int cx, int cz);

  // la notice et la boite de ce chunk sur le gpu
  unsigned int vao = 0;
  unsigned int vbo = 0;

  // combien de sommets a dessiner
  int nombreSommets = 0;
};

// declaration de la fonction : le code est dans chunk.cpp
bool estSolide(const Chunk &chunk, int x, int y, int z);
void envoyerMeshAuGPU(Chunk &chunk);
std::vector<float> construireMesh(const Chunk &chunk);
int compterFacesDessus(const Chunk &chunk);
void ajouterFace(std::vector<float> &vecteur, const std::vector<float> &face);
