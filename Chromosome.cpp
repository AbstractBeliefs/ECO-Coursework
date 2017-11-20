#include "Chromosome.h"

#include <limits>
#include <vector>

Chromosome::Chromosome(int num_turbines) {
    this->num_turbines = num_turbines;
    this->fitness = std::numeric_limits<double>::max();
    this->turbines = std::vector<int>(num_turbines);
}

Chromosome::~Chromosome() {
}

bool Chromosome::operator<(const Chromosome &b) const {
  if (this->fitness < b.fitness) {
    return true;
  } else {
    return false;
  }
}

bool Chromosome::operator==(const Chromosome &b) const {
  return this->turbines == b.turbines;
}
