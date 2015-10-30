#include <iostream>
#include "PlanetWars.h"
#include <fstream>
#include <math.h>
#include <algorithm>
using namespace std;

// The DoTurn function is where your code goes. The PlanetWars object contains
// the state of the game, including information about all planets and fleets
// that currently exist. Inside this function, you issue orders using the
// pw.IssueOrder() function. For example, to send 10 ships from planet 3 to
// planet 8, you would say pw.IssueOrder(3, 8, 10).
//
// There is already a basic strategy in place here. You can use it as a
// starting point, or you can throw it out entirely and replace it with your
// own. Check out the tutorials and articles on the contest website at
// http://www.ai-contest.com/resources.
int num_attacks=0;
class targets{
public:
    int planet_id;
    double score;
    int num_ships;
};

int getLength(vector<targets> target,const PlanetWars& pw){
    int limit=target[0].score/2;
    int needed=0;
    int myShipCount=0;
    vector<Planet> my_planets=pw.MyPlanets();
    for(int i=0;i<my_planets.size();i++){
        myShipCount+=my_planets[i].NumShips();
    }
    for(int i=0;i<target.size();i++){
        if(target[i].score>limit){
            needed+=target[i].num_ships;
            if(needed>myShipCount*0.4){
                return i;
            }
        }
    }
    return target.size();
}

int minimal(int count){
    if(count>1000)return (int)(count/4);
    if(count>500)return (int)((count*2)/9);
    if(count>200){return (int)(count/5);}
    if(count>100){return (int)(count/6);}
    if(count>20){return (int)(count/7);}
}

double distance(double aX, double aY, double bX, double bY){
    double x = aX-bX;
    double y = aY-bY;
    double dist = x*x + y*y;
    return sqrt(dist);
}

bool comparetargets(targets a, targets b){
    return (a.score>b.score);
}

bool compar(Planet a, Planet b){
    return (a.NumShips()>b.NumShips());
}

void Full_on_attack(const PlanetWars &pw){
    /* get the Strongest enemy Planet */
    vector<Planet> my_planets = pw.MyPlanets();
    vector<Planet> enemy=pw.EnemyPlanets();
    vector<Fleet> myfleet=pw.MyFleets();
    vector<Fleet> enemyfleet=pw.EnemyFleets();
    
    int enemy_strongest = -1;
    
    double enemy_score = -999999.0;
    
    for (int i = 0; i < enemy.size(); ++i) {
        const Planet& p = enemy[i];
        double score_current = (double)p.NumShips();

        if (score_current > enemy_score) {
            enemy_score = score_current;
            enemy_strongest = p.PlanetID();
        }
    }
    if(enemy_strongest!=-1){
        for (int i = 0; i < my_planets.size(); ++i)
        {
            /* code */
            if (my_planets[i].NumShips()>10)
            {
                /* code */
                int poi = (int) (my_planets[i].NumShips()/2);
                pw.IssueOrder(my_planets[i].PlanetID(),enemy_strongest, poi); num_attacks++;
            }
        }
        return;
    }
}


void Defence(const PlanetWars& pw){
  //vector
  vector<Planet> my_planets = pw.MyPlanets();
  vector<Planet> neutral_planets = pw.NeutralPlanets();
  vector<Planet> enemy_planets = pw.EnemyPlanets();
  vector<Fleet> my_fleet = pw.MyFleets();
  vector<Fleet> enemy_fleet = pw.EnemyFleets();

  //find the planet that is under attack

  //vector of the my planets that are under attack
  vector<Planet> under_attack;

  for(int i = 0; i < enemy_fleet.size(); ++i){
    bool flag = 0;
    for(int j = 0; j < under_attack.size(); ++j){
      if(enemy_fleet[i].DestinationPlanet() == under_attack[j].PlanetID()) flag = 1;
    }
    for(int j = 0; j < my_planets.size(); ++j){
      if(enemy_fleet[i].DestinationPlanet() == my_planets[j].PlanetID()){
          for(int a=0;a<my_planets.size();a++){
              if(my_planets[a].PlanetID()==enemy_fleet[i].DestinationPlanet()){
                if(!flag)under_attack.push_back(my_planets[a]);
                break;
              }
            }
      }
    }

  }

  if(under_attack.size() <= 1)return;



  
  for(int i = 0; i < under_attack.size(); ++i){
    //assign score to the under_attacked planet
    double safety_score = under_attack[i].NumShips();
    for(int j = 0; j < my_planets.size(); ++j){
      if(under_attack[i].PlanetID() != my_planets[j].PlanetID()){
          // Heuristic
        safety_score += my_planets[j].NumShips() * (10/(1 + distance(under_attack[i].X(), under_attack[i].Y(), my_planets[j].X(), my_planets[j].Y())));
      }
    }


    //total upcoming ships on that planet
    double upcoming = 1.0;
    for(int j = 0; j < enemy_fleet.size(); ++j){
      if(enemy_fleet[j].DestinationPlanet() == under_attack[i].PlanetID()){
        upcoming += enemy_fleet[j].NumShips();
      }
    }

    // to let the planet go or send reinforcement

    //to let planet go and send the ships of that planet to the nearest my planet
    if(safety_score < upcoming){
      //find the planet that has most no of ships
      Planet& to_planet = my_planets[0];
      double total_ships = 0;
      for(int j = 0; j < my_planets.size(); ++j){
        if(my_planets[j].NumShips() >= total_ships){
          total_ships = my_planets[j].NumShips();
          to_planet = my_planets[j];
        }
      }
        /*if(under_attack[i].NumShips()>5){
            if(under_attack[i].PlanetID()!=to_planet.PlanetID()){
            pw.IssueOrder(under_attack[i].PlanetID(), to_planet.PlanetID(), under_attack[i].NumShips()-5);
        }
        }*/
    }

    //to reinforce the planet
    else{
        if((safety_score*0.7) >= upcoming){
          for(int j = 0; j < my_planets.size(); ++j){
            if(my_planets[j].PlanetID() != under_attack[i].PlanetID()){
              if(my_planets[j].NumShips() > 25){
                pw.IssueOrder(my_planets[j].PlanetID(), under_attack[i].PlanetID(),minimal(my_planets[j].NumShips())); num_attacks++;
              }
            }
          }
        }
        else{
            return;
        }
    }
  }

  return;


}

void Attack_and_neutral_Attack(const PlanetWars& pw,double f1){
    ofstream m("ex.txt",ios::app);
    vector<targets> target;
    vector<Planet> enemy_planets=pw.EnemyPlanets();
    vector<Planet> my_planets = pw.MyPlanets();
    vector<Planet> neutral_planets = pw.NeutralPlanets();
    vector<Planet> not_my_planets=pw.NotMyPlanets();
    vector<Fleet> enemy_fleets=pw.EnemyFleets();
    vector<Fleet> my_fleets=pw.MyFleets();
    //find the target enemy planet
    double target_score = -999999.0;
    Planet& target_planet = not_my_planets[0];
    // enemy
    if(num_attacks > 0){
        for(int i = 0; i < enemy_planets.size(); ++i){
        const Planet& p = enemy_planets[i];
        double surrounding_score = 1;
        double enemy_score=1;
        targets t;
        t.planet_id=p.PlanetID();
        t.num_ships=p.NumShips();
        //assign the score on the basis of reinforcement and planet itself

        //planet_score due to surroundings surroundings score

        for(int j = 0; j < enemy_planets.size(); ++j){
        const Planet& p_neigh = enemy_planets[j];
        if(p_neigh.PlanetID() != p.PlanetID()){
          // Heuristic
          enemy_score += (1/(1 + distance(p.X(), p.Y(), p_neigh.X(), p_neigh.Y()) * distance(p.X(), p.Y(), p_neigh.X(), p_neigh.Y()) )) * p_neigh.NumShips();
        }
        }
        for(int j = 0; j < my_planets.size(); ++j){
        const Planet& p_neigh = my_planets[j];

        if(p_neigh.PlanetID() != p.PlanetID()){
          // Heuristic
          surrounding_score += (1/(1 + distance(p.X(), p.Y(), p_neigh.X(), p_neigh.Y()) * distance(p.X(), p.Y(), p_neigh.X(), p_neigh.Y()) )) * p_neigh.NumShips();
        }
        }
        //finding the target planet by following score heuristic

        //can further optimized by putting target planets in a priority queue sorted based on the planet_score
        int factor = 2;
        // Heuristic
        int ships=(int)(p.NumShips() * 3);
        for (int i = 0; i < my_fleets.size(); ++i)
        {
            if (my_fleets[i].DestinationPlanet()==p.PlanetID())
            {
                ships-=my_fleets[i].NumShips();
            }
        }
        for (int i = 0; i < enemy_fleets.size(); ++i)
        {
            if (enemy_fleets[i].DestinationPlanet()==p.PlanetID())
            {
                ships+=enemy_fleets[i].NumShips();
            }
        }
        if (ships<=0)
        {
        continue;
        }
        double planet_score =p.GrowthRate()*surrounding_score/(ships*ships*enemy_score);  //growth rate, suurounding score, ships
        //m<<p.PlanetID()<<", "<<planet_score<<", "<<surrounding_score<<", "<<ships<<", "<<enemy_score<<endl;
        t.score=planet_score;
        target.push_back(t);
        }
    }


// for neutrals
  for(int i = 0; i < neutral_planets.size(); ++i){
    const Planet& p = neutral_planets[i];
    double surrounding_score = 0;
    double enemy_score=0;
    targets t;
    t.planet_id=p.PlanetID();
    t.num_ships=p.NumShips();
    //assign the score on the basis of reinforcement and planet itself

    //planet_score due to surroundings surroundings score

    for(int j = 0; j < enemy_planets.size(); ++j){
      const Planet& p_neigh = enemy_planets[j];

      if(p_neigh.PlanetID() != p.PlanetID()){
          // Heuristic
        enemy_score += (1/(1 + distance(p.X(), p.Y(), p_neigh.X(), p_neigh.Y()) * distance(p.X(), p.Y(), p_neigh.X(), p_neigh.Y()) )) * p_neigh.NumShips();
      }
    }
    for(int j = 0; j < my_planets.size(); ++j){
      const Planet& p_neigh = my_planets[j];

      if(p_neigh.PlanetID() != p.PlanetID()){
          // Heuristic
        surrounding_score += (1/(1 + distance(p.X(), p.Y(), p_neigh.X(), p_neigh.Y()) * distance(p.X(), p.Y(), p_neigh.X(), p_neigh.Y()) )) * p_neigh.NumShips();
      }
    }
    //finding the target planet by following score heuristic

    //can further optimized by putting target planets in a priority queue sorted based on the planet_score
    int factor = 2;
    // Heuristic
    int ships=(int)(p.NumShips() * 1.5);
    for (int i = 0; i < my_fleets.size(); ++i)
    {
        if (my_fleets[i].DestinationPlanet()==p.PlanetID())
        {
            ships-=my_fleets[i].NumShips();
        }
    }
    for (int i = 0; i < enemy_fleets.size(); ++i)
    {
        if (enemy_fleets[i].DestinationPlanet()==p.PlanetID())
        {
            ships+=enemy_fleets[i].NumShips();
        }
    }
    if (ships<=0)
    {
        continue;
    }
    double planet_score =f1*p.GrowthRate()*surrounding_score/(ships*enemy_score);  //growth rate, suurounding score, ships
    //double planet_score =f1*p.GrowthRate()*surrounding_score/(p.NumShips()*enemy_score);  //growth rate, suurounding score, ships
    t.score=planet_score;
    target.push_back(t);
  }
  //find source planets(my planets) from where the fleet should go
  //send 5 ships from each of my planets
    if(!(target.size()>0))return;
    sort(target.begin(),target.end(),comparetargets);
    int target_planet_count =getLength(target,pw);
    for(int it=0;it<target_planet_count;it++){
        for(int i = 0; i < my_planets.size(); ++i){
            int inbound=0;
            for(int j=0;j<enemy_fleets.size();j++){
                if(enemy_fleets[j].DestinationPlanet()==my_planets[i].PlanetID()){
                    inbound+=enemy_fleets[j].NumShips();
                }
            }
            int Nships=minimal(my_planets[i].NumShips()-inbound);
            if(my_planets[i].NumShips()-inbound > (it+1)*Nships+10){
                pw.IssueOrder(my_planets[i].PlanetID(), target[it].planet_id,Nships );
            }
            else return;
        }
    }
  return;
}


void Attack_and_neutral_Defense(const PlanetWars& pw,double f1){
    vector<targets> target;
    vector<Planet> enemy_planets=pw.EnemyPlanets();
    vector<Planet> my_planets = pw.MyPlanets();
    vector<Planet> neutral_planets = pw.NeutralPlanets();
    vector<Planet> not_my_planets=pw.NotMyPlanets();
    vector<Fleet> enemy_fleets=pw.EnemyFleets();
    vector<Fleet> my_fleets=pw.MyFleets();
    //find the target enemy planet
    double target_score = -999999.0;
    Planet& target_planet = not_my_planets[0];
    // enemy
    for(int i = 0; i < enemy_planets.size(); ++i){
    const Planet& p = enemy_planets[i];
    double surrounding_score = 1;
    double enemy_score=1;
    targets t;
    t.planet_id=p.PlanetID();

    //assign the score on the basis of reinforcement and planet itself

    //planet_score due to surroundings surroundings score

    for(int j = 0; j < enemy_planets.size(); ++j){
    const Planet& p_neigh = enemy_planets[j];
    if(p_neigh.PlanetID() != p.PlanetID()){
      // Heuristic
      enemy_score += (1/(1 + distance(p.X(), p.Y(), p_neigh.X(), p_neigh.Y()))) * p_neigh.NumShips();
    }
    }
    for(int j = 0; j < my_planets.size(); ++j){
    const Planet& p_neigh = my_planets[j];

    if(p_neigh.PlanetID() != p.PlanetID()){
      // Heuristic
      surrounding_score += (1/(1 + distance(p.X(),p.Y(), p_neigh.X(),p_neigh.Y()))) * p_neigh.NumShips();
    }
    }
    //finding the target planet by following score heuristic

    //can further optimized by putting target planets in a priority queue sorted based on the planet_score
    int factor = 2;
    // Heuristic
    int ships=(int)(p.NumShips() * 3);
    for (int i = 0; i < my_fleets.size(); ++i)
    {
      if (my_fleets[i].DestinationPlanet()==p.PlanetID())
      {
        ships-=my_fleets[i].NumShips();
      }
    }
    for (int i = 0; i < enemy_fleets.size(); ++i)
    {
      if (enemy_fleets[i].DestinationPlanet()==p.PlanetID())
      {
        ships+=enemy_fleets[i].NumShips();
      }
    }
    if (ships<0)
    {
    continue;
    }
    double planet_score =p.GrowthRate()*surrounding_score/(ships*ships*enemy_score);  //growth rate, suurounding score, ships
    t.score=planet_score;
    target.push_back(t);
    if(planet_score >= target_score){
        target_score = planet_score;
        target_planet = p;
    }
    }


// for neutrals
  for(int i = 0; i < neutral_planets.size(); ++i){
    const Planet& p = neutral_planets[i];
    double surrounding_score = 1;
    double enemy_score=1;
    targets t;
    t.planet_id=p.PlanetID();
    //assign the score on the basis of reinforcement and planet itself

    //planet_score due to surroundings surroundings score

    for(int j = 0; j < enemy_planets.size(); ++j){
      const Planet& p_neigh = enemy_planets[j];

      if(p_neigh.PlanetID() != p.PlanetID()){
      // Heuristic
        enemy_score += (1/(1 + distance(p.X(), p.Y(), p_neigh.X(), p_neigh.Y()))) * p_neigh.NumShips();
      }
    }
    for(int j = 0; j < my_planets.size(); ++j){
      const Planet& p_neigh = my_planets[j];

      if(p_neigh.PlanetID() != p.PlanetID()){
      // Heuristic
        surrounding_score += (17/(1 + distance(p.X(),p.Y(), p_neigh.X(),p_neigh.Y()))) * p_neigh.NumShips();
      }
    }
    //finding the target planet by following score heuristic

    //can further optimized by putting target planets in a priority queue sorted based on the planet_score
    int factor = 2;
    // Heuristic
    int ships=(int)(p.NumShips() * 1.5);
    for (int i = 0; i < my_fleets.size(); ++i)
    {
      if (my_fleets[i].DestinationPlanet()==p.PlanetID())
      {
        ships-=my_fleets[i].NumShips();
      }
    }
    for (int i = 0; i < enemy_fleets.size(); ++i)
    {
      if (enemy_fleets[i].DestinationPlanet()==p.PlanetID())
      {
        ships+=enemy_fleets[i].NumShips();
      }
    }
  if (ships<0)
  {
    continue;
  }
    double planet_score =f1*p.GrowthRate()*surrounding_score/(ships*enemy_score);  //growth rate, suurounding score, ships
    //double planet_score =f1*p.GrowthRate()*surrounding_score/(p.NumShips()*enemy_score);  //growth rate, suurounding score, ships
    t.score=planet_score;
    if(planet_score >= target_score){
      target_score = planet_score;
      target_planet = p;
    }
  }
  //find source planets(my planets) from where the fleet should go
  //send 5 ships from each of my planets

  for(int i = 0; i < my_planets.size(); ++i){
    int inbound=0;
    for(int j=0;j<enemy_fleets.size();j++){
      if(enemy_fleets[j].DestinationPlanet()==my_planets[i].PlanetID()){
        inbound+=enemy_fleets[j].NumShips();
      }
    }
    if(my_planets[i].NumShips()-inbound > 15){
      pw.IssueOrder(my_planets[i].PlanetID(), target_planet.PlanetID(), minimal(my_planets[i].NumShips()-inbound)); num_attacks++;
    }
  }
  return;
}

void DoTurn(const PlanetWars& pw) {
    vector<Planet> my_planets = pw.MyPlanets();
    vector<Planet> neutral_planets=pw.NeutralPlanets();
    vector<Planet> enemy_planets=pw.EnemyPlanets();
    vector<Fleet> my_fleets=pw.MyFleets();
    vector<Fleet> enemy_fleets=pw.EnemyFleets();
    int enemyShipCount=0;
    int myShipCount=0;
    
/* get count of both */
    for(int i=0;i<my_planets.size();i++){
        myShipCount+=my_planets[i].NumShips();
    }
    for(int i=0;i<enemy_planets.size();i++){
        enemyShipCount+=enemy_planets[i].NumShips();
    }
    for (int i = 0; i < my_fleets.size(); ++i)
    {
        myShipCount+=my_fleets[i].NumShips();
    }
    for (int i = 0; i < enemy_fleets.size(); ++i)
    {
        enemyShipCount+=enemy_fleets[i].NumShips();
    }
    //if we're good attack
    if(myShipCount>=1.2*enemyShipCount){
        // an all out attack
        Full_on_attack(pw);
        return;
    } else{
        if(myShipCount>1.1*enemyShipCount){
            Attack_and_neutral_Attack(pw,0.01);
            Defence(pw);
        }
        else{
            if(myShipCount>enemyShipCount){ 
                Attack_and_neutral_Defense(pw,0.8);
                Defence(pw);
            }
            else{
                if(num_attacks>5){
                    Attack_and_neutral_Defense(pw,2);
                    Defence(pw);
                }
                else{
                    Attack_and_neutral_Defense(pw,1);
                    Defence(pw);
                }
            }
        }
        
    }
    return;
}

// This is just the main game loop that takes care of communicating with the
// game engine for you. You don't have to understand or change the code below.
int main(int argc, char *argv[]) {
    string current_line;
    string map_data;
    while (true) {
        int c = cin.get();
        current_line += (char)c;
        if (c == '\n') {
            if (current_line.length() >= 2 && current_line.substr(0, 2) == "go") {
                PlanetWars pw(map_data);
                map_data = "";
                DoTurn(pw);
                pw.FinishTurn();
            } else {
                map_data += current_line;
            }
            current_line = "";
        }
    }
    
    return 0;
}
