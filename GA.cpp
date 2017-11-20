//
//  GA.cpp
//

#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <time.h>
#include <vector>
#include <algorithm>
#include <stdexcept>

#include "GA.h"
#include "WindScenario.h"
#include "KusiakLayoutEvaluator.h"
#include "Chromosome.h"

GA::GA(KusiakLayoutEvaluator evaluator) {
  

  wfle = evaluator;
 
  // example parameters… might not need
  num_pop = 20;
  nt = 0;
  tour_size = 4;
  mut_rate = 0.25;
  cross_rate = 0.40;
  srand(time(NULL));
}

GA::~GA() {
  delete[] fits;
  delete grid;
}

double GA::evaluate_single(Chromosome soln) {
  int nturbines=0;
  for (int i=0; i<nt; i++) {
    if (soln.turbines[i]>0) {
      nturbines++;
    }
  }

  Matrix<double>* layout = new Matrix<double>(nturbines,2);
  int l_i = 0;
  for (int i=0; i<nt; i++) {
    if (soln.turbines[i]>0) {
      layout->set(l_i, 0, grid->get(i,0));
      layout->set(l_i, 1, grid->get(i,1));
      l_i++;
    }
  }

  wfle.evaluate(layout);
  double coe = wfle.getEnergyCost();
  delete layout;
  return coe;
}

void GA::evaluate() {
  double minfit = std::numeric_limits<double>::max();
  for (int p=0; p<num_pop; p++) {
    int nturbines=0;
    for (int i=0; i<nt; i++) {
      if (pops[p].turbines[i]>0) {
        nturbines++;
      }
    }

    Matrix<double>* layout = new Matrix<double>(nturbines,2);
    int l_i = 0;
    for (int i=0; i<nt; i++) {
      if (pops[p].turbines[i]>0) {
        layout->set(l_i, 0, grid->get(i,0));
        layout->set(l_i, 1, grid->get(i,1));
        l_i++;
      }
    }

    wfle.evaluate(layout);
    double coe = wfle.getEnergyCost();
    Matrix<double>* fitnesses = wfle.getTurbineFitnesses();

    int n_valid = 0;
    for (int i=0; i<nturbines; i++) {
      if (fitnesses->get(i,0) > 0.80) {
        n_valid++;
      }
    }

    fits[p] = coe; //n_valid;
    if (fits[p] < minfit) {
        minfit = fits[p];
    }
    delete layout;
    delete fitnesses;
  }

  printf("%f\n", minfit);
}

void GA::run() {

  fits = new double[num_pop];

  // set up grid
  // centers must be > 8*R apart
  double interval = 8.001 * wfle.scenario.R;

  int nx = (int) wfle.scenario.width / interval;
  int ny = (int) wfle.scenario.height / interval;

  // get number of valid grid spots
  nt=0;
  for (int x=0; x<nx; x++) {
    for (int y=0; y<ny; y++) {
      double xpos = x*interval;
      double ypos = y*interval;
      bool valid = true;

      for (unsigned int o=0; o<wfle.scenario.obstacles.rows; o++) {
        double xmin = wfle.scenario.obstacles.get(o, 0);
        double ymin = wfle.scenario.obstacles.get(o, 1);
        double xmax = wfle.scenario.obstacles.get(o, 2);
        double ymax = wfle.scenario.obstacles.get(o, 3);
        if (xpos>xmin && ypos>ymin && xpos<xmax && ypos<ymax) {
          valid = false;
	  }
      }
      if (valid) {
        nt++;
      }
    }
  }

  grid = new Matrix<double>(nt, 2);
  int t = 0;
  for (int x=0; x<nx; x++) {
    for (int y=0; y<ny; y++) {
      double xpos = x*interval;
      double ypos = y*interval;
      bool valid = true;

      for (unsigned int o=0; o<wfle.scenario.obstacles.rows; o++) {
        double xmin = wfle.scenario.obstacles.get(o, 0);
        double ymin = wfle.scenario.obstacles.get(o, 1);
        double xmax = wfle.scenario.obstacles.get(o, 2);
        double ymax = wfle.scenario.obstacles.get(o, 3);
        if (xpos>xmin && ypos>ymin && xpos<xmax && ypos<ymax) {
          valid = false;
        }
      }
      if (valid) {
        grid->set(t, 0, x*interval);
        grid->set(t, 1, y*interval);
        t++;
      }
    }
  }

  // initialize populations
  pops = vector<Chromosome>(num_pop, Chromosome(nt));

  for (int chromosome = 0; chromosome < num_pop; chromosome++) {
    for (int turbine = 0; turbine < pops[chromosome].num_turbines; turbine++){
      if ((double)rand()/RAND_MAX > 0.5){
        pops[chromosome].turbines[turbine] = 1;
      } else {
        pops[chromosome].turbines[turbine] = 0;
      }
    }
    pops[chromosome].fitness = evaluate_single(pops[chromosome]);
  }

  // evaluate initial populations (uses num_pop evals)
  puts("Initial population:");
  std::sort(pops.begin(), pops.end());
  for (int i = 0; i < pops.size(); i++){
    printf("%2d: %f\n", i, pops[i].fitness);
  }
  double initial_lowest_cost = pops[0].fitness;
  printf("Lowest cost: %f\n", initial_lowest_cost);

  try {
    int eval_counter = 0;
    puts("Burning evaluations...");
    while (true){
      if (eval_counter % 100 == 0){
        float top = pops[0].fitness;
        float bottom = pops[num_pop-1].fitness;
        float delta = initial_lowest_cost - top;
        printf("Evaluation %5d. Current top: %f, bottom: %f, current delta: %f\n", eval_counter, top, bottom, delta);
      }
      Chromosome child = mutate(crossover(tournament_selection(tour_size), tournament_selection(tour_size)));
      child.fitness = evaluate_single(child);
      pops.push_back(child);
      std::sort(pops.begin(), pops.end());
      pops.pop_back();
      eval_counter++;
    }
  } catch (const std::out_of_range& e) {
    std::sort(pops.begin(), pops.end());
    printf("Ran out of evaluations.\n");
    printf("Initial Lowest cost: %f\n", initial_lowest_cost);
    printf("Current Lowest cost: %f\n", pops[0].fitness);
    printf("delta cost: %f\n", initial_lowest_cost - pops[0].fitness);
  }
}

Chromosome GA::population_pick_one(){
  return pops[rand() % pops.size()];
}

Chromosome GA::tournament_selection(int tournament_size){
  vector<Chromosome> tournament;

  for (int i = 0; i < tournament_size; i++){
    Chromosome selection = population_pick_one();
    while (true){
      if (std::find(tournament.begin(), tournament.end(), selection) != tournament.end()) {
        selection = population_pick_one();
      } else {
        tournament.push_back(selection);
        break;
      }
    }
  }

  std::sort(tournament.begin(), tournament.end());
  return tournament[0];
}

Chromosome GA::crossover(Chromosome a, Chromosome b){
  Chromosome child = Chromosome(a.num_turbines);
  int crossover_point = rand()%child.num_turbines;

  for (int turbine = 0; turbine < child.num_turbines; turbine++){
    if (turbine < crossover_point){
      child.turbines[turbine] = a.turbines[turbine];
    } else {
      child.turbines[turbine] = b.turbines[turbine];
    }
  }

  return child;
}

Chromosome GA::mutate(Chromosome c){
  for (int turbine = 0; turbine < c.num_turbines; turbine++){
    if ((double)rand()/RAND_MAX > this->mut_rate){
      c.turbines[turbine] = c.turbines[turbine] ? 0 : 1;
    }
  }

  return c;
}
