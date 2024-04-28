#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <map>
#include <vector>
#include <chrono>
//These are only used for Z testing
#include <numeric>
#include <cmath>
#include <algorithm>
using namespace std;
double calculateMean(const vector<double>& data) 
{
    return accumulate(data.begin(), data.end(), 0.0) / data.size();
}

double calculateStdDev(const vector<double>& data, double mean) 
{
    double sq_sum = inner_product(data.begin(), data.end(), data.begin(), 0.0);
    double variance = sq_sum / data.size() - mean * mean;
    if (variance <= 0) {
        return 0;
    }
    return sqrt(variance);
}
int numRuns = 30;
vector<double> fitness_with_ls(numRuns);
vector<double> fitness_without_ls(numRuns);
int numItems, maxWeight;
int populationSize = 100, selectionSize = 4, maxGenerations = 5;
double crossoverRate = 0.85, mutationRate = 0.6;
double baseMutationRate = 0.03;
double maxMutationRate = 0.6;
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
void localSearch(Chromosome &chromosome) 
{
    for (unsigned int i = 0; i < chromosome.genes.size(); i++)
    {
        chromosome.genes[i] = !chromosome.genes[i];
        double oldFitness = chromosome.fitness;
        setFitness(chromosome);

        if (chromosome.fitness <= oldFitness)
        {
            chromosome.genes[i] = !chromosome.genes[i];
            chromosome.fitness = oldFitness;
        }
    }
}
double runProblem(string fileName, bool localSearchEnabled, time_t seed, bool outputEnabled)
{
    if (localSearchEnabled)
        maxGenerations = numItems;
    else
        maxGenerations = 5 * numItems;
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
            if (localSearchEnabled)
            {
                localSearch(child1);
                localSearch(child2);
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
        if(!outputEnabled)
            cout << "Generation " << i + 1 << " Best Chromosome Fitness: " << bestChromosome.fitness << endl;
    }
    auto end = chrono::high_resolution_clock::now();
    auto elapsed = chrono::duration<double>(end - start);
    cout << endl << "Problem: " << fileName << endl;
    cout << "Algorithm: " << (localSearchEnabled ? "GA-LS" : "GA") << endl;
    cout << "Best Solution: ";
    for (unsigned int i = 0; i < bestChromosome.genes.size(); i++)
    {
        if (bestChromosome.genes[i])
        {
            cout << i + 1 << " ";
        }
    }
    cout << endl << "Known Optimum: " << bestChromosome.fitness << endl;
    cout << "Runtime: " << elapsed.count() << " seconds" << endl;
    cout << "Seed: " << seed << endl;
    return bestChromosome.fitness;
}
int main() 
{
    time_t seed;
    cout << "Z-Test [y/n]: ";
    char zTestOption;
    cin >> zTestOption;
    bool zTestEnabled = zTestOption == 'y' || zTestOption == 'Y';
    bool localSearchEnabled = false;
    if (!zTestEnabled)
    {
        cout << "Enter seed: ";
        cin >> seed;
        // seed = time(0);
        srand(seed);
        cout << "Knapsack Problem" << endl;
        cout << "Local Search [y/n]: ";
        char localSearchOption;
        cin >> localSearchOption;
        localSearchEnabled = localSearchOption == 'y' || localSearchOption == 'Y';
    }
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
    for (int i = 0; i < numItems; i++)
    {
        inputFile >> allItems[i].value >> allItems[i].weight;
    }
    inputFile.close();
    mutationRate = min(baseMutationRate * numItems, maxMutationRate);
    if (!zTestEnabled)
        runProblem(fileName, localSearchEnabled, seed, false);
    // Used for Z testing
    else
    {
        for (int i = 0; i < numRuns; i++)
        {
            seed = time(0) + i;
            srand(seed);
            fitness_with_ls[i] = runProblem(fileName, true, seed, true);
            fitness_without_ls[i] = runProblem(fileName, false, seed, true);
        }
        double mean_with_ls = calculateMean(fitness_with_ls);
        double mean_without_ls = calculateMean(fitness_without_ls);
        double sd_with_ls = calculateStdDev(fitness_with_ls, mean_with_ls);
        double sd_without_ls = calculateStdDev(fitness_without_ls, mean_without_ls);

        double se_diff = sqrt((sd_with_ls * sd_with_ls / numRuns) + (sd_without_ls * sd_without_ls / numRuns));

        double z_score = se_diff == 0 ? 0 : (mean_with_ls - mean_without_ls) / se_diff;

        cout << "Z-Score: " << z_score << endl;

        double critical_value = 1.645;
        if (z_score > critical_value) 
        {
            cout << "Reject null hypothesis: the means are not equivalent." << endl;
        } 
        else 
        {
            cout << "Fail to reject null hypothesis: the means are equivalent." << endl;
        }
    }

    return 0;
}