#ifndef GENETICALGORITHM_H_
#define GENETICALGORITHM_H_

#ifndef COMMON_H_
#include "Common.h"
#endif

#ifndef GENE_H_
#include "Gene.h"
#endif

#ifndef WRITER_H_
#include "Writer.h"
#endif

class GeneticAlgorithm {
 private:
    int nowGene;
    int population;
    int rateOfMutation;
    int survivePopulation;
    vector <Gene> genes;
    vector<vector<Automaton> > existedAutomatons;
    Automaton automaton;
    
    bool newGene(const Gene& gene);
    int randomWithBias() const;
    
 public:
    GeneticAlgorithm(const int pop,const int rM,const double rS,const Automaton &m);

    void createGenes();

    bool nextGene();
    void nextGeneration();
    
    void nowGeneSetScore(const PreciseNumber &value);

    Automaton getNowGeneAutomaton() const;

    void view(const int index) const;

};

#endif

    
