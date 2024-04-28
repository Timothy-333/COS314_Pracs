#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <map>
#include <vector>
#include <chrono>

using namespace std;
int numItems, maxWeight;
int populationSize = 100, selectionSize = 4, maxGenerations = 5;
double crossoverRate = 0.85, mutationRate = 0.6;
struct Item 
{
    double weight;
    double value;
};
struct Chromosome 
{
    vector<bool> genes;
    double fitness;
};
vector<Item> allItems;
vector<Chromosome> population;
double populationFitness = 0;
void setFitness(Chromosome &chromosome) 
{
    double totalWeight = 0;
    double totalValue = 0;
    for (unsigned int i = 0; i < chromosome.genes.size(); i++)
    {
        totalWeight += allItems[i].weight * chromosome.genes[i];
        totalValue += allItems[i].value * chromosome.genes[i];
    }
    if (totalWeight > maxWeight)
    {
        chromosome.fitness = 0;
    } 
    else 
    {
        chromosome.fitness = totalValue;
    }
}
void setPopulationFitness() 
{
    populationFitness = 0;
    for (unsigned int i = 0; i < population.size(); i++)
    {
        populationFitness += population[i].fitness;
    }
}
vector<Chromosome> generatePopulation(int populationSize) 
{
    vector<Chromosome> population;
    for (int i = 0; i < populationSize; i++) 
    {
        Chromosome chromosome;
        for (int j = 0; j < numItems; j++)
        {
            if (rand() % 2 == 0)
            {
                chromosome.genes.push_back(true);
            } 
            else 
            {
                chromosome.genes.push_back(false);
            }
        }
        setFitness(chromosome);
        population.push_back(chromosome);
    }
    return population;
}
Chromosome getParent() 
{
    vector<Chromosome> selectedParents;
    for (int i = 0; i < selectionSize; i++) 
    {
        int randomIndex = rand() % population.size();
        selectedParents.push_back(population[randomIndex]);
    }
    Chromosome parent = selectedParents[0];
    for (unsigned int i = 1; i < selectedParents.size(); i++) 
    {
        if (selectedParents[i].fitness > parent.fitness) 
        {
            parent = selectedParents[i];
        }
    }
    return parent;
}
pair<Chromosome, Chromosome> selectChromosones() 
{
    Chromosome parent1 = getParent();
    Chromosome parent2 = getParent();
    return make_pair(parent1, parent2);
}
pair<Chromosome, Chromosome> crossover()
{
    pair<Chromosome, Chromosome> parents = selectChromosones();
    Chromosome parent1 = parents.first;
    Chromosome parent2 = parents.second;
    if(rand() % 100 > crossoverRate * 100)
    {
        return make_pair(parent1, parent2);
    }
    Chromosome child1, child2;
    int crossoverPoint = rand() % numItems;
    for (int i = 0; i < crossoverPoint; i++) 
    {
        child1.genes.push_back(parent1.genes[i]);
        child2.genes.push_back(parent2.genes[i]);
    }
    for (unsigned int i = crossoverPoint; i < parent1.genes.size(); i++) 
    {
        child1.genes.push_back(parent2.genes[i]);
        child2.genes.push_back(parent1.genes[i]);
    }
    setFitness(child1);
    setFitness(child2);
    return make_pair(child1, child2);
}
void mutate(Chromosome &chromosome) 
{
    int mutationPoint = rand() % numItems;
    chromosome.genes[mutationPoint] = !chromosome.genes[mutationPoint];
    setFitness(chromosome);
}
Chromosome getBestChromosome() 
{
    Chromosome bestChromosome = population[0];
    for (unsigned int i = 1; i < population.size(); i++)
    {
        if (population[i].fitness > bestChromosome.fitness) 
        {
            bestChromosome = population[i];
        }
    }
    return bestChromosome;
}
int main() 
{
    srand(time(0));
    string fileNames[] = {"f1_l-d_kp_10_269", "f2_l-d_kp_20_878", "f3_l-d_kp_4_20", "f4_l-d_kp_4_11", "f5_l-d_kp_15_375", "f6_l-d_kp_10_60", "f7_l-d_kp_7_50", "f8_l-d_kp_23_10000", "f9_l-d_kp_5_80", "f10_l-d_kp_20_879", "knapPI_1_100_1000_1"};
    cout << "options" << endl;
    for (int i = 0; i < 11; i++)
    {
        cout << i + 1 << ". " << fileNames[i] << endl;
    }

    cout << "Enter the number of the file you want to open: ";
    int chosenFile;
    cin >> chosenFile;
    if (chosenFile < 1 || chosenFile > 11)
    {
        cout << "Invalid file number." << endl;
        return 1;
    }
    string dirName = "Knapsack Instances";
    string fileName = fileNames[chosenFile - 1];

    if (chdir(dirName.c_str()) != 0) 
    {
        perror("Failed to change directory");
        return 1;
    }

    ifstream inputFile(fileName);
    cout << "File name: " << fileName << endl;
    if (!inputFile) 
    {
        cerr << "Failed to open the file." << endl;
        return 1;
    }
    inputFile >> numItems >> maxWeight;

    allItems.resize(numItems);
    maxGenerations *= numItems;
    for (int i = 0; i < numItems; i++) 
    {
        inputFile >> allItems[i].value >> allItems[i].weight;
    }
    inputFile.close();
    auto start = chrono::high_resolution_clock::now();
    population = generatePopulation(populationSize);
    Chromosome bestChromosome = getBestChromosome();
    setPopulationFitness();
    for (int i = 0; i < maxGenerations; i++) 
    {
        vector<Chromosome> newPopulation;
        for (int j = 0; j < populationSize/2; j++) 
        {
            pair<Chromosome, Chromosome> children = crossover();
            Chromosome child1 = children.first;
            Chromosome child2 = children.second;
            if (rand() % 100 < mutationRate * 100)
            {
                mutate(child1);
            }
            if (rand() % 100 < mutationRate * 100) 
            {
                mutate(child2);
            }
            newPopulation.push_back(child1);
            newPopulation.push_back(child2);
        }
        population = newPopulation;
        setPopulationFitness();
        Chromosome newBestChromosome = getBestChromosome();
        if (newBestChromosome.fitness == 0)
        {
            Chromosome newChromosome;
            for (int j = 0; j < numItems; j++)
            {
                newChromosome.genes.push_back(false);
            }
            int index = rand() % population.size();
            population.erase(population.begin() + index);

            population.push_back(newChromosome);
        }
        if (newBestChromosome.fitness > bestChromosome.fitness)
        {
            bestChromosome = newBestChromosome;
        }
        cout << "Generation " << i + 1 << " Best Chromosome Fitness: " << bestChromosome.fitness << endl;
    }
    auto end = chrono::high_resolution_clock::now();
    auto elapsed = chrono::duration<double>(end - start);
    cout << "Time taken: " << elapsed.count() << " seconds" << endl;
    return 0;
}