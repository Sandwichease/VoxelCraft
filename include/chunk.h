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
};

// declaration de la fonction : le code est dans chunk.cpp
bool estSolide(const Chunk &chunk, int x, int y, int z);
