#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
using namespace std;

struct CampusVisit
{
    int campusId;
    string campusName;
};
void printRoute(vector<CampusVisit> route)
{
  for (const auto& visit : route) 
  {
    cout << visit.campusName;
    if (&visit != &route.back()) 
    {
      cout << "->";
    }
  }
}
vector<CampusVisit> simulatedAnnealing(const vector<CampusVisit>& campuses, const double distanceMatrix[][5], double initialTemperature, double coolingRate, int maxIterations) 
{
  // Generate initial random route
  vector<CampusVisit> currentRoute = generateRandomRoute(campuses);
  double currentDistance = calculateRouteDistance(currentRoute, distanceMatrix);
  vector<CampusVisit> bestRoute = currentRoute;
  double bestDistance = currentDistance;

  for (int i = 0; i < maxIterations; ++i) {
    double temperature = initialTemperature * pow(coolingRate, i);

    // Generate a neighbor solution (swap two random campuses)
    vector<CampusVisit> neighbor = currentRoute;
    int swapIndex1 = rand() % neighbor.size();
    int swapIndex2 = (swapIndex1 + rand() % (neighbor.size() - 1) + 1) % neighbor.size();
    swap(neighbor[swapIndex1], neighbor[swapIndex2]);

    double newDistance = calculateRouteDistance(neighbor, distanceMatrix);
    double delta = newDistance - currentDistance;

    // Metropolis acceptance criteria
    if (delta < 0 || exp(-delta / temperature) > static_cast<double>(rand()) / RAND_MAX) {
      currentRoute = neighbor;
      if (newDistance < bestDistance) {
        bestRoute = currentRoute;
        bestDistance = newDistance;
      }
    }
  }

  return bestRoute;
}
// Function to generate a random initial route
vector<CampusVisit> generateRandomRoute(const vector<CampusVisit>& campuses)
{
  static mt19937 g(random_device{}());
  
  vector<CampusVisit> route = campuses;
  route.push_back(campuses[0]);
  shuffle(route.begin() + 1, route.end() - 1, g);
  cout << "Start Route: ";
  printRoute(route);
  cout << endl;
  return route;
}

// Function to calculate total distance for a given route
double calculateRouteDistance(const vector<CampusVisit>& route, const double distanceMatrix[][5]) 
{
  double totalDistance = 0;
  for (size_t i = 0; i < route.size() - 1; ++i)
  {
    const CampusVisit& current = route[i];
    const CampusVisit& next = route[i + 1];
    totalDistance += distanceMatrix[current.campusId][next.campusId];
  }
  // Add distance back to starting point (Hatfield)
  totalDistance += distanceMatrix[route[route.size() - 1].campusId][0];
  return totalDistance;
}

vector<CampusVisit> localSearch(vector<CampusVisit> route, const double distanceMatrix[][5]) 
{
  double currentDistance = calculateRouteDistance(route, distanceMatrix);
  bool improvement = true;
  while (improvement) 
  {
    improvement = false;
    for (size_t i = 1; i < route.size() - 2; ++i) 
    {
      for (size_t j = i + 1; j < route.size() - 1; ++j) 
      {
        vector<CampusVisit> newRoute = route;
        reverse(newRoute.begin() + i, newRoute.begin() + j + 1);
        double newDistance = calculateRouteDistance(newRoute, distanceMatrix);
        if (newDistance < currentDistance) 
        {
          route = newRoute;
          currentDistance = newDistance;
          improvement = true;
        }
      }
    }
  }
  return route;
}

// Function for perturbation (e.g., random swap of non-consecutive campuses)
vector<CampusVisit> perturbation(vector<CampusVisit> route)
{
  random_device rd;
  mt19937 g(rd());
  uniform_int_distribution<int> dist(1, route.size() - 2);
  int i = dist(g);
  int j = dist(g);
  while (j == i) 
  {
    j = dist(g);
  }
  vector<CampusVisit> newRoute = route;
  swap(newRoute[i], newRoute[j]);
  return newRoute;
}
int main()
{
    double distanceMatrix[5][5] = {{0, 15, 20, 22, 30},
                                 {15, 0, 10, 12, 25},
                                 {20, 10, 0, 8, 22},
                                 {22, 12, 8, 0, 18},
                                 {30, 25, 22, 18, 0}};

    vector<CampusVisit> campuses;
    // Hatfield Hillcrest Groenkloof Prinsof Mamelodi
    CampusVisit campus;
    campus.campusName = "Hatfield";
    campus.campusId = 0;
    campuses.push_back(campus);
    campus.campusName = "Hillcrest";
    campus.campusId = 1;
    campuses.push_back(campus);
    campus.campusName = "Groenkloof";
    campus.campusId = 2;
    campuses.push_back(campus);
    campus.campusName = "Prinsof";
    campus.campusId = 3;
    campuses.push_back(campus);
    campus.campusName = "Mamelodi";
    campus.campusId = 4;
    campuses.push_back(campus);

  // Generate initial random route
  vector<CampusVisit> currentRoute = generateRandomRoute(campuses);
  vector<CampusVisit> bestRoute = currentRoute;
  double currentDistance = calculateRouteDistance(currentRoute, distanceMatrix);
  double bestDistance = currentDistance;
  // ILS parameters (adjust as needed)
  int maxIterations = 1000;
  int perturbationFrequency = 10;

  for (int i = 0; i < maxIterations; ++i) 
  {
    // Apply local search
    currentRoute = localSearch(currentRoute, distanceMatrix);
    currentDistance = calculateRouteDistance(currentRoute, distanceMatrix);
    // Perturb the solution occasionally
    if (i % perturbationFrequency == 0)
    {
      currentRoute = perturbation(currentRoute);
      currentDistance = calculateRouteDistance(currentRoute, distanceMatrix);
    }
    if (currentDistance < bestDistance)
    {
      bestRoute = currentRoute;
      bestDistance = currentDistance;
      cout << "Current Best Route: ";
      printRoute(bestRoute);
      cout << endl;
      cout << "Distance: " << currentDistance << endl;
    }
  }

  cout << "Best Route: ";
  printRoute(bestRoute);
  cout << endl;
  cout << "Distance: " << bestDistance << endl;

  return 0;
}