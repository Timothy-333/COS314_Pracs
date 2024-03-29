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

// Function to generate a random initial route
vector<CampusVisit> generateRandomRoute(const vector<CampusVisit>& campuses)
{
  vector<CampusVisit> route = campuses;
  route.push_back(campuses[0]);
  random_device rd;
  mt19937 g(rd());
  shuffle(route.begin() + 1, route.end() - 1, g);
  return campuses;
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

// Function implementing local search (e.g., swapping consecutive campuses)
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
  double currentDistance = calculateRouteDistance(currentRoute, distanceMatrix);
  // ILS parameters (adjust as needed)
  int maxIterations = 1000;
  int perturbationFrequency = 10;

  for (int i = 0; i < maxIterations; ++i) {
    // Apply local search
    currentRoute = localSearch(currentRoute, distanceMatrix);

    // Perturb the solution occasionally
    if (i % perturbationFrequency == 0) {
      currentRoute = perturbation(currentRoute);
    }

    double newDistance = calculateRouteDistance(currentRoute, distanceMatrix);
    // Update best solution if improvement found
    if (newDistance < currentDistance) {
      currentDistance = newDistance;
    }
  }

  // Print the best route and its distance
  cout << "Best Route (ILS): ";
  for (const auto& visit : currentRoute) {
    cout << visit.campusId << " ";
  }
  cout << endl;
  cout << "Distance (ILS): " << currentDistance << endl;

  return 0;
}