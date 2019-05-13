#include <algorithm>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <typeindex>

using namespace std;

struct GameObject;
void collide(GameObject& first, GameObject& second);

struct GameObject {
  virtual ~GameObject() = default;
  virtual type_index type() const = 0;
  virtual void collide(GameObject& other) { ::collide(*this, other); }
};

template <typename T>
struct GameObjectImpl : GameObject {
  type_index type() const override { return typeid(T); }
};

struct Planet : GameObjectImpl<Planet> {};
struct Asteroid : GameObjectImpl<Asteroid> {};
struct Spaceship : GameObjectImpl<Spaceship> {};
struct ArmedSpaceship : Spaceship {
  type_index type() const override { return typeid(ArmedSpaceship); }
};

void spaceship_planet() { cout << "spaceship lands on planet\n"; }
void asteroid_planet() { cout << "asteroid burns up in atmosphere\n"; }
void asteroid_spaceship() { cout << "asteroid hits and destroys spaceship\n"; }
void asteroid_armed_spaceship() { cout << "spaceship shoots asteroid\n"; }

// Multi-dispatch implemented with a map from pair of types to function.
map<pair<type_index, type_index>, void (*)(void)> outcomes{
    {{typeid(Spaceship), typeid(Planet)}, spaceship_planet},
    {{typeid(Asteroid), typeid(Planet)}, asteroid_planet},
    {{typeid(Asteroid), typeid(Spaceship)}, asteroid_spaceship},
    {{typeid(Asteroid), typeid(ArmedSpaceship)}, asteroid_armed_spaceship},
};

void collide(GameObject& first, GameObject& second) {
  auto it = outcomes.find({first.type(), second.type()});
  if (it == outcomes.end()) {
    it = outcomes.find({second.type(), first.type()});
    if (it == outcomes.end()) {
      cout << "objects pass each other harmlessly\n";
      return;
    }
  }
  it->second();
}

int main() {
  ArmedSpaceship spaceship;
  Asteroid asteroid;
  Planet planet;

  collide(planet, spaceship);
  collide(planet, asteroid);
  collide(spaceship, asteroid);
  // collide(planet, planet);
  planet.collide(planet);

  return 0;
}
