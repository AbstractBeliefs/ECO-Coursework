//
//  GA.cpp
//

#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <time.h>

#include "GA.h"
#include "WindScenario.h"
#include "KusiakLayoutEvaluator.h"

GA::GA(KusiakLayoutEvaluator evaluator) {
  

  wfle = evaluator;
 
  // example parameters… might not need
  num_pop = 20;
  nt = 0;
  tour_size = 4;
  mut_rate = 0.05;
  cross_rate = 0.40;
  srand(time(NULL));
}

GA::~GA() {
  delete[] fits;
  delete pops;
  delete grid;
}

void GA::evaluate() {
  double minfit = std::numeric_limits<double>::max();
  for (int p=0; p<num_pop; p++) {
    int nturbines=0;
    for (int i=0; i<nt; i++) {
      if (pops->get(i,p)>0) {
        nturbines++;
      }
    }

    Matrix<double>* layout = new Matrix<double>(nturbines,2);
    int l_i = 0;
    for (int i=0; i<nt; i++) {
      if (pops->get(i,p)>0) {
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
  pops = new Matrix<int>(nt, num_pop);

  for (int p=0; p<num_pop; p++) {
    for (int i=0; i<nt; i++) {
      int turb = 0;
      double randbit = (double) rand()/RAND_MAX;
      if (randbit > 0.5) {
        turb = 1;
      }
      pops->set(i, p, turb);
    }
  }

  // evaluate initial populations (uses num_pop evals)
  evaluate();

  // put your optimisation code here
  for (int i=0; i<(10000); i++) {
     
  }
}
