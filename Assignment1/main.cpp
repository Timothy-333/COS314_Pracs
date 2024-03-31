#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <chrono>

using namespace std;

struct CampusVisit
{
  int campusId;
  string campusName;
};
// Function to print a route
void printRoute(const vector<CampusVisit>& route)
{
  for (const auto& visit : route)
  {
    cout << visit.campusName;
    if (&visit != &route.back())
    {
      cout << "->";
    }
  }
  cout << endl;
}
void printDistances(const vector<double>& distances)
{
  double totalDistance = 0;
  for (const auto& distance : distances)
  {
    totalDistance += distance;
    cout << distance;
    if(&distance != &distances.back())
    {
      cout << ", ";
    }
    else
    {
      cout << endl;
    }
  }
  cout << "Average Distance: " << totalDistance / distances.size() << endl;
}
// Function to generate a random initial route
vector<CampusVisit> generateRandomRoute(const vector<CampusVisit>& campuses)
{
  vector<CampusVisit> route = campuses;
  shuffle(route.begin() + 1, route.end() - 1, mt19937(random_device{}()));
  route.push_back(route.front());
  return route;
}

// Function to calculate total distance for a given route
double calculateRouteDistance(const vector<CampusVisit>& route, const double distanceMatrix[][5])
{
  double totalDistance = 0;
  for (size_t i = 0; i < route.size() - 1; ++i)
  {
    totalDistance += distanceMatrix[route[i].campusId][route[i + 1].campusId];
  }
  return totalDistance;
}

vector<CampusVisit> localSearch(vector<CampusVisit> route, const double distanceMatrix[][5])
{
  bool improvement = true;
  while (improvement)
  {
    improvement = false;
    for (size_t i = 1; i < route.size() - 2; ++i) // Exclude first and last elements from swapping
    {
      vector<CampusVisit> newRoute = route;
      swap(newRoute[i], newRoute[i + 1]); // Swap consecutive campuses
      if (calculateRouteDistance(newRoute, distanceMatrix) < calculateRouteDistance(route, distanceMatrix))
      {
        route = newRoute;
        improvement = true;
      }
    }
  }
  return route;
}

// Function to perform perturbation
vector<CampusVisit> perturbation(vector<CampusVisit> route)
{
  swap(route[(rand() % (route.size()-2))+1], route[(rand() % (route.size()-2))+1]);
  return route;
}

// Function to perform simulated annealing
pair<vector<CampusVisit>, vector<double>> simulatedAnnealing(const vector<CampusVisit>& campuses, const double distanceMatrix[][5], double initialTemperature, double coolingRate, int maxIterations)
{
  vector<CampusVisit> currentRoute = generateRandomRoute(campuses);
  vector<CampusVisit> bestRoute = currentRoute;
  vector<double> bestDistances;

  for (int i = 0; i < maxIterations; ++i)
  {
    double temperature = initialTemperature * pow(coolingRate, i);
    vector<CampusVisit> neighbor = currentRoute;
    swap(neighbor[(rand() % (neighbor.size()-2))+1], neighbor[(rand() % (neighbor.size()-2))+1]);
    if (calculateRouteDistance(neighbor, distanceMatrix) < calculateRouteDistance(currentRoute, distanceMatrix) || exp((calculateRouteDistance(currentRoute, distanceMatrix) - calculateRouteDistance(neighbor, distanceMatrix)) / temperature) > static_cast<double>(rand()) / RAND_MAX)
    {
      currentRoute = neighbor;
      if (calculateRouteDistance(currentRoute, distanceMatrix) < calculateRouteDistance(bestRoute, distanceMatrix))
      {
        bestRoute = currentRoute;
      }
    }
    bestDistances.push_back(calculateRouteDistance(bestRoute, distanceMatrix));
  }

  return make_pair(bestRoute, bestDistances);
}

// Function to perform iterated local search
pair<vector<CampusVisit>, vector<double>> iteratedLocalSearch(vector<CampusVisit> currentRoute, const double distanceMatrix[][5], int maxIterations)
{
  vector<CampusVisit> bestRoute = currentRoute;
  vector<double> bestDistances;

  for (int i = 0; i < maxIterations; ++i)
  {
    currentRoute = localSearch(currentRoute, distanceMatrix);
    if (calculateRouteDistance(currentRoute, distanceMatrix) < calculateRouteDistance(bestRoute, distanceMatrix))
    {
      bestRoute = currentRoute;
    }
    bestDistances.push_back(calculateRouteDistance(bestRoute, distanceMatrix));
    currentRoute = perturbation(currentRoute);
  }

  return make_pair(bestRoute, bestDistances);
}

int main()
{
  double distanceMatrix[5][5] =
                            {
                              {0, 15, 20, 22, 30},
                              {15, 0, 10, 12, 25},
                              {20, 10, 0, 8, 22},
                              {22, 12, 8, 0, 18},
                              {30, 25, 22, 18, 0}
                            };

  vector<CampusVisit> campuses =
                              {
                                {0, "Hatfield"},
                                {1, "Hillcrest"},
                                {2, "Groenkloof"},
                                {3, "Prinsof"},
                                {4, "Mamelodi"}
                              };

  int maxIterations = 20;
  double initialTemperature = 1000;
  double coolingRate = 0.99;

  cout << "Iterated Local Search:\n";
  auto start = chrono::high_resolution_clock::now();
  pair<vector<CampusVisit>, vector<double>> ilsResult = iteratedLocalSearch(generateRandomRoute(campuses), distanceMatrix, maxIterations);
  auto end = chrono::high_resolution_clock::now();
  auto elapsed = chrono::duration_cast<chrono::microseconds>(end - start);
  cout << "Time taken by ILS: " << elapsed.count() << " microseconds" << endl;
  cout << "Distances:\n";
  printDistances(ilsResult.second);
  cout << "Best Route (ILS): ";
  printRoute(ilsResult.first);
  cout << "Distance: " << calculateRouteDistance(ilsResult.first, distanceMatrix) << endl << endl;

  cout << "Simulated Annealing:\n";
  start = chrono::high_resolution_clock::now();
  pair<vector<CampusVisit>, vector<double>> saResult = simulatedAnnealing(campuses, distanceMatrix, initialTemperature, coolingRate, maxIterations);
  end = chrono::high_resolution_clock::now();
  elapsed = chrono::duration_cast<chrono::microseconds>(end - start);
  cout << "Time taken by SA: " << elapsed.count() << " microseconds" << endl;
  cout << "Distances:\n";
  printDistances(saResult.second);
  cout << "Best Route (SA): ";
  printRoute(saResult.first);
  cout << "Distance: " << calculateRouteDistance(saResult.first, distanceMatrix) << endl;
  return 0;
}