#include "GeneticAlgorithm.h"

GeneticAlgorithm::GeneticAlgorithm(const int pop, const int rM, const double rS, const Automaton &m) {
   nowGene = 0;
   population = pop;
   rateOfMutation = rM;
   survivePopulation = (int) population * rS; //nextGenerationで生き残る人口
   automaton = m;
   const vector<Automaton>::size_type size = pow(m.getNumberOfStates() + 1, m.getNumberOfActions() - 1);
   existedAutomatons.resize(size);
}

bool GeneticAlgorithm::newGene(const Gene &gene) {
   const Automaton m = gene.getAutomaton();
   const int numStates = m.getNumberOfStates();
   const int numActions = m.getNumberOfSignals();
   vector<int> numActionStates(numActions, 0);
   for (int st = 0; st < numStates; ++st)
      ++numActionStates[m.getActionChoice(st)];
   int place = 0;
   int radix = 1;
   for (int ac = 0; ac < numActions - 1; ++ac) {
      place += numActionStates[ac] * radix;
      radix *= m.getNumberOfStates() + 1;
   }
   if (find(existedAutomatons[place].begin(), existedAutomatons[place].end(), m) == existedAutomatons[place].end()) {
      existedAutomatons[place].push_back(m);
      return true;
   }
   return false;
}

void GeneticAlgorithm::createGenes() {
   int i=nowGene;
   while (i<population) {
      Gene gene(automaton);
      if (!newGene(gene)) continue;
      genes.push_back(gene);
      ++i;
   }
}

void GeneticAlgorithm::nextGeneration() {
   sort(genes.begin(),genes.end());
   int i=survivePopulation;
   int count=0;
   while (i<population) {
      if (++count > 20000)
         break;
      int mother,father;
      mother = randomWithBias();
      father = randomWithBias();
      if (mother==father) continue;
      Gene gene(genes[mother],genes[father],rateOfMutation);
      if (!newGene(gene)) continue;
      genes[i] = gene;
      i++;
   }
   if (count >= 20000) {
      cout << "last generation" << endl;
   }
   nowGene=survivePopulation;
}

// select mother or father
int GeneticAlgorithm::randomWithBias() const {
   int random = rand() % ((survivePopulation)*(survivePopulation+1)/2);
   int cut=0;
   int x = survivePopulation;
   int count = 0;
   do {
      cut += x;
      x--;
      count++;
   } while (random >= cut);
   return count-1;
}

bool GeneticAlgorithm::nextGene() {
   int i;
   for (i=nowGene; i<population; i++) {
      if (!genes[i].doesSetScore())
         break;
   }
   if (i==population) {
      return false;
   } else {
      nowGene = i;
      return true;
   }
}

Automaton GeneticAlgorithm::getNowGeneAutomaton() const{
   return genes[nowGene].getAutomaton();
}

void GeneticAlgorithm::nowGeneSetScore(const PreciseNumber &value) {
   genes[nowGene].setScore(value);
}

void GeneticAlgorithm::view(const int index) const {
   if (index > population)
      return;
   cout << genes[index] << endl;
}
   
