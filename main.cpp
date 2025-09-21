//
//  main.cpp
//  p2-a-new-heap
//
//  Created by Pablo Rodriguez on 5/16/25.
//

// Project Identifier: AD48FB4835AF347EB0CA8009E24C3B13F8519882

#include <cassert>
#include <cstdio>

#include <sstream>

#include "P2random.h"
#include <cstdint> // for uint32_t


#include <iostream>
#include <string>
#include <getopt.h>
#include "xcode_redirect.hpp"

#include <queue>

#include <vector>
using namespace std;



uint32_t next_id = 0;

struct Options {
    bool verbose = false;
    bool median = false;
    bool general_eval = false;
    bool watcher = false;
};




void getOptions(int argc, char **argv, Options &options) {
    // These are used with getopt_long()
    opterr = static_cast<int>(false);  // Let us handle all error output for command line options
    int choice;
    int index = 0;

    // NOLINTBEGIN: getopt is old C code predating C++ style, this usage is from `man getopt`
    option longOptions[] = {
  // Insert two lines for the "mode" ('m') and the "help" ('h') options.
        {"verbose", no_argument, nullptr, 'v' },
        {"median", no_argument, nullptr, 'm' },
        {"general-eval", no_argument, nullptr, 'g' },
        {"watcher", no_argument, nullptr, 'w'},
        {nullptr, 0, nullptr, '\0'},
    };  // longOptions[]
    // NOLINTEND

    // Fill in the double quotes, to match the mode and help options.
    while ((choice = getopt_long(argc, argv, "vmgw", static_cast<option *>(longOptions), &index)) != -1) {
        switch (choice) {
            case 'v':
                options.verbose = true;
                break;
            case 'm':
                options.median = true;
                break;
        
            case 'g':
                options.general_eval = true;
                break;

            case 'w':
                options.watcher = true;
                break;
                        


        default:
            cerr << "Unknown command line option" << '\n';
            exit(1);

        }  // switch ..choice
    }  // while
}  // getOptions()

struct General{
    uint32_t sithtroops = 0;
    uint32_t jeditroops = 0;
    uint32_t troopslost = 0;
};

struct Jedi {
    uint32_t timestamp;
    bool is_jedi;
    uint32_t general;
    uint32_t planet;
    uint32_t force;
    uint32_t troops = 0;
    uint32_t unique_id = 0;
};


struct Sith{
    uint32_t timestamp;
    bool is_jedi;
    uint32_t general;
    uint32_t planet;
    uint32_t force;
    uint32_t troops = 0;
    uint32_t unique_id = 0;
   
};

// Comparator for max-heap by force (strongest Sith first)
//struct CompareSith {
//    bool operator()(const Sith& a, const Sith& b) const {
//        return a.force < b.force;  // higher force = higher priority
//    }
//};
//
//struct CompareJedi {
//    bool operator()(const Jedi& a, const Jedi& b) const {
//        return a.force > b.force;  // less force = higher priority
//    }
//};

struct CompareJedi {
    bool operator()(const Jedi& a, const Jedi& b) const {
        if (a.force != b.force) return a.force > b.force;            // weaker first
        return a.unique_id > b.unique_id;                            // earlier deployment wins
    }
};

struct CompareSith {
    bool operator()(const Sith& a, const Sith& b) const {
        if (a.force != b.force) return a.force < b.force;            // stronger first wins
        return a.unique_id > b.unique_id;                            // earlier deployment wins
    }
};

struct WatcherState {
    Jedi currentjedi;
    Sith currentsith;

    Jedi bestjedi;
    Jedi maybejedi;
    Sith bestsith;

    Jedi abestjedi;
    Sith maybesith;
    Sith abestsith;
};




struct Planets{
    priority_queue<Sith, vector<Sith>, CompareSith> sithQueue;
    
    priority_queue<Jedi, vector<Jedi>, CompareJedi> jediQueue;
    
    // Max-heap for the lower half
    priority_queue<uint32_t> left;
    // Min-heap for the upper half
    priority_queue<uint32_t , vector<uint32_t>, greater<uint32_t>> right;
  
//    Jedi currentjedi;
//    Sith currentsith;
//        
//    Jedi bestjedi;
//    Jedi maybejedi;
//    Sith bestsith;
//        
//        
//    Jedi abestjedi;
//    Sith maybesith;
//    Sith abestsith;
   
    
   // void attackmode(bool True);
    
    //void ambushmode(bool True);
    void attackmode(WatcherState &w, bool is_sith);
    void ambushmode(WatcherState &w, bool is_sith);
    
    void insert(uint32_t num);
    
    uint32_t getMedian() const;
    
    
    uint32_t battles = 0;
    
    // watcher data, allocated only if needed
    WatcherState* watcher = nullptr;
    
};
void Planets::ambushmode(WatcherState &w,bool n){
    //true == new sith
    //flase == new jedi
    if(n == true){
        if(w.abestsith.troops == 0 && sithQueue.size() == 1){
            w.abestsith = w.currentsith;
        }
        
        else if(w.abestjedi.troops == 0 && w.currentsith.force > w.abestsith.force){
            w.abestsith = w.currentsith;
        }
        
        else if (w.abestsith.troops != 0 && w.currentsith.force > w.abestsith.force){
            w.maybesith = w.currentsith;
        }
        
    }
    
    if(n == false){

        if(w.abestsith.troops == 0){
        }//if best jedi hasn't been found do nothing with new sith
        
        else if(w.abestjedi.troops == 0 &&
                w.currentjedi.force <= w.abestsith.force){
            w.abestjedi = w.currentjedi;
            
            if((w.maybesith.force - w.currentjedi.force) >
                   (w.abestsith.force - w.abestjedi.force) && w.currentjedi.force <= w.abestsith.force && w.maybesith.force != 0){
               w.abestsith = w.maybesith;
               w.abestjedi = w.currentjedi;
           }
        }//if sith will fight and bestjedi isn't empty set sith to best sith
        
         else if(w.abestjedi.troops != 0 &&
             w.currentjedi.force < w.abestjedi.force){
            w.abestjedi = w.currentjedi;
             if((w.maybesith.force - w.currentjedi.force) >
                    (w.abestsith.force - w.abestjedi.force) && w.currentjedi.force <= w.abestsith.force && w.maybesith.force != 0){
                w.abestsith = w.maybesith;
                w.abestjedi = w.currentjedi;
            }//if new sit
         }//if best sith isn't empty and new sith has greater force update best sith
        
          else if((w.maybesith.force - w.currentjedi.force) >
                 (w.abestsith.force - w.abestjedi.force) && w.maybesith.troops != 0){
             w.abestsith = w.maybesith;
             w.abestjedi = w.currentjedi;
         }//if new sith and maybe jedi are better then
    }
    
    
    
}

void Planets::attackmode(WatcherState &w,bool n){
    //true == new sith
    //flase == new jedi
    
    
    if(n == true){
        if(w.bestjedi.troops == 0){
        }//if best jedi hasn't been found do nothing with new sith
        else if(w.bestjedi.troops != 0 &&
                w.currentsith.force >= w.bestjedi.force){
            w.bestsith = w.currentsith;
            if((w.currentsith.force - w.maybejedi.force) >
                    (w.bestsith.force - w.bestjedi.force) && w.maybejedi.troops != 0){
                w.bestsith = w.currentsith;
                w.bestjedi = w.maybejedi;
            }//if new sith and maybe jedi are better then
            
        }//if sith will fight and bestjedi isn't empty set sith to best sith
        
         else if(w.bestsith.troops != 0 &&
             w.currentsith.force > w.bestsith.force){
             w.bestsith = w.currentsith;
             
             if((w.currentsith.force - w.maybejedi.force) >
                     (w.bestsith.force - w.bestjedi.force) && w.maybejedi.troops != 0){
                 w.bestsith = w.currentsith;
                 w.bestjedi = w.maybejedi;
             }//if new s
         }//if best sith isn't empty and new sith has greater force update best sith
        
         else if((w.currentsith.force - w.maybejedi.force) >
                 (w.bestsith.force - w.bestjedi.force )){
             w.bestsith = w.currentsith;
             w.bestjedi = w.maybejedi;
         }//if new sith and maybe jedi are better then
    }
    
    if (n == false){
        if(w.bestjedi.troops == 0 && jediQueue.size() == 1){
            w.bestjedi = w.currentjedi;
        }//first jedi added is best jedi
        
        else if(w.bestsith.troops == 0 && w.currentjedi.force < w.bestjedi.force){
            w.bestjedi = w.currentjedi;
        }//If a new jedi is better when no best sith update best jedi
        
        else if (w.bestjedi.troops != 0 && w.currentjedi.force < w.bestjedi.force){
            w.maybejedi = w.currentjedi;
        }//if new jedi has less force then best jedi add new jedi to maybebest
        
        
        
        
    }
}




 void Planets::insert(uint32_t num){
     // Step 1: Add to one of the heaps
             if (left.empty() || num <= left.top()) {
                 left.push(num);
             } else {
                 right.push(num);
             }

             // Step 2: Balance the heaps
             if (left.size() > right.size() + 1) {
                 right.push(left.top());
                 left.pop();
             } else if (right.size() > left.size()) {
                 left.push(right.top());
                 right.pop();
             }
}

uint32_t Planets::getMedian() const{
    if (left.size() == right.size()) {
                return (left.top() + right.top()) / 2;
            } else {
                return left.top();
            }
}



//work on verbose something is off
void verbose_output(const Sith &s,const Jedi &j, uint32_t num_troops){
    //General <GENERAL_A_NUM>'s battalion attacked General <GENERAL_B_NUM>'s battalion on planet <PLANET_NUM>. <NUM_TROOPS_LOST> troops were lost.
    cout << "General " << s.general << "'s battalion attacked General "
    << j.general << "'s battalion on planet " << s.planet << ". "
    << num_troops << " troops were lost." << '\n';
    
}


void read_line(vector<Planets> &planet_map, uint32_t timestamp,
               vector<General> &General, Options &options, uint32_t prevtime) {

    
    string line;


    getline(cin, line); // e.g. "0 SITH G1 P2 F100 #10"


    stringstream ss(line);
    //int time;
    string type, general_str, planet_str, force_str, id_str; //orginal

    ss >> type >> general_str >> planet_str >> force_str >> id_str;






    if (type != "JEDI" && type != "SITH") {
        cerr << "Invalid unit type: " << type << '\n';
        exit(1);
    }


    // Extract numeric parts
    int force_val = stoi(force_str.substr(1));     // from "F100"
    int troops_count = stoi(id_str.substr(1));       // from "#10"
    int generalID = stoi(general_str.substr(1)); // from "G1"
    int planet_id = stoi(planet_str.substr(1));     // from "P2"
    if(0 > generalID ){
        cout << "Invalid general ID";
        exit(1);
    }

    if(0 > planet_id){
        cout << "Invalid planet ID";
        exit(1);
    }


    if(force_val <= 0){
        cout << "Invalid force sensitivity level";
        exit(1);
    }

    if(troops_count <= 0){
        cout << "Invalid number of troops";
        exit(1);
    }


    // Convert to uint32_t
    //uint32_t timestamp = static_cast<uint32_t>(time);
    uint32_t general = static_cast<uint32_t>(generalID);
    uint32_t planet = static_cast<uint32_t>(planet_id);
    uint32_t force = static_cast<uint32_t>(force_val);
    uint32_t troops = static_cast<uint32_t>(troops_count);

    if( general >= General.size()){
        cout << "Invalid general ID";
        exit(1);
    }

    if( planet >= planet_map.size()){
        cout << "Invalid planet ID";
        exit(1);
    }

    if(prevtime > timestamp){
        cout << "Invalid decreasing timestamp";
        exit(1);
    }

    
    

    // Add troop to appropriate queue
    if (type == "SITH") {
        Sith s = {timestamp, false, general, planet, force, troops, next_id++};
        planet_map[planet].sithQueue.push(s);

        if (options.general_eval)
            General[s.general].sithtroops += s.troops;

//        if (options.watcher) {
//            bool is_sith = true;
//            planet_map[planet_id].currentsith = s;
//            planet_map[planet_id].attackmode(is_sith);
//            planet_map[planet_id].ambushmode(is_sith);
//        }
        
        if (options.watcher && planet_map[planet].watcher) {
           // bool is_sith = true;
            auto &w = *planet_map[planet].watcher;
            w.currentsith = s;
            planet_map[planet].attackmode(w, true);
            planet_map[planet].ambushmode(w, true);
        }

    } else { // type == "JEDI"
        Jedi j = {timestamp, true, general, planet, force, troops, next_id++};
        planet_map[planet].jediQueue.push(j);

        if (options.general_eval)
            General[j.general].jeditroops += j.troops;

//        if (options.watcher) {
//            bool is_jedi = false;
//            planet_map[planet_id].currentjedi = j;
//            planet_map[planet_id].attackmode(is_jedi);
//            planet_map[planet_id].ambushmode(is_jedi);
//        }
        
        if (options.watcher && planet_map[planet].watcher) {
           // bool is_jedi = false;
            auto &w = *planet_map[planet].watcher;
            w.currentjedi = j;
            planet_map[planet].attackmode(w, false);
            planet_map[planet].ambushmode(w, false);
        }
        
        
        
        
        
        
    }
}






void fight(Planets &p, Options options, vector<General> &General) {
    const Sith& s = p.sithQueue.top(); // no copy yet
    const Jedi& j = p.jediQueue.top(); // no copy yet

    uint32_t num_troops = 0;

    if (s.troops > j.troops) {
        if (options.general_eval) {
            General[j.general].troopslost += j.troops;
            General[s.general].troopslost += j.troops;
        }

        Sith updated_sith = s; // copy only when modifying
        updated_sith.troops -= j.troops;
        num_troops = j.troops * 2;
        
        if (options.verbose) {
            verbose_output(s, j, num_troops);
        }

        p.sithQueue.pop();
        p.jediQueue.pop();
        p.sithQueue.push(updated_sith);

//        if (options.verbose) {
//            verbose_output(s, j, num_troops);
//        }

        if (options.median) {
            p.insert(num_troops);
            p.battles++;
        }

    } else if (j.troops > s.troops) {
        if (options.general_eval) {
            General[j.general].troopslost += s.troops;
            General[s.general].troopslost += s.troops;
        }

        Jedi updated_jedi = j; // copy only when modifying
        updated_jedi.troops -= s.troops;
        num_troops = s.troops * 2;
        
        if (options.verbose) {
            verbose_output(s, j, num_troops);
        }

        p.jediQueue.pop();
        p.sithQueue.pop();
        p.jediQueue.push(updated_jedi);

//        if (options.verbose) {
//            verbose_output(s, j, num_troops);
//        }

        if (options.median) {
            p.insert(num_troops);
            p.battles++;
        }

    } else {
        // Tie: both lose all troops
        if (options.general_eval) {
            General[j.general].troopslost += j.troops;
            General[s.general].troopslost += j.troops;
        }

        num_troops = j.troops * 2;
        if (options.verbose) {
            verbose_output(s, j, num_troops);
        }
        
        p.jediQueue.pop();
        p.sithQueue.pop();

//        if (options.verbose) {
//            verbose_output(s, j, num_troops);
//        }

        if (options.median) {
            p.insert(num_troops);
            p.battles++;
        }
    }
}





void check_fight(vector<Planets> &planet_map, uint32_t &battles, Options &options, vector<General> &General){
    for(size_t i = 0; i < planet_map.size(); i++){
        Planets &p = planet_map[i];
        while(!p.jediQueue.empty() && !p.sithQueue.empty()
              && p.sithQueue.top().force >= p.jediQueue.top().force){//checking if planet has any troops
              fight(p, options, General);
              battles++;
            }
        }
    }
    


void median(const vector<Planets> &planet_map, uint32_t timestamp, uint32_t &battles) {
    if(battles > 0){
        const uint32_t display_time = timestamp;
        for (size_t i = 0; i < planet_map.size(); ++i) {
            const Planets &p = planet_map[i];
            if (p.battles > 0) {
                cout << "Median troops lost on planet " << i
                << " at time " << display_time
                << " is " << p.getMedian() << ".\n";
            }
        }
    }
}

void print_generaleval(vector<General> &G){
    cout << "---General Evaluation---" << '\n';
    //General <GENERAL_ID> deployed <NUM_JEDI> Jedi troops and <NUM_SITH> Sith troops, and <NUM_SURVIVORS>/<NUM_DEPLOYED>
    for(size_t i = 0; i < G.size(); i++){
        cout << "General " << i << " deployed " << G[i].jeditroops <<
        " Jedi troops and " << G[i].sithtroops << " Sith troops, and "
        << (G[i].jeditroops + G[i].sithtroops) - G[i].troopslost << "/"
        << G[i].jeditroops + G[i].sithtroops << " troops survived." << '\n';
    }
}





void print_movie(const vector<Planets> &pmap) {
    cout << "---Movie Watcher---\n";

    for (size_t i = 0; i < pmap.size(); ++i) {
        const Planets &p = pmap[i];

        // Only run if this planet has watcher data
        if (p.watcher) {
            const WatcherState &w = *p.watcher;

            bool has_ambush = w.abestjedi.troops > 0 && w.abestsith.troops > 0;
            bool has_attack = w.bestjedi.troops > 0 && w.bestsith.troops > 0;

            if (has_ambush) {
                cout << "On planet " << i
                     << ", a movie watcher would enjoy an ambush with Sith at time "
                     << w.abestsith.timestamp << " and Jedi at time "
                     << w.abestjedi.timestamp << " with a force difference of "
                     << w.abestsith.force - w.abestjedi.force << ".\n";
            } else {
                cout << "On planet " << i
                     << ", a movie watcher would not see an interesting ambush.\n";
            }

            if (has_attack) {
                cout << "On planet " << i
                     << ", a movie watcher would enjoy an attack with Jedi at time "
                     << w.bestjedi.timestamp << " and Sith at time "
                     << w.bestsith.timestamp << " with a force difference of "
                     << w.bestsith.force - w.bestjedi.force << ".\n";
            } else {
                cout << "On planet " << i
                     << ", a movie watcher would not see an interesting attack.\n";
            }
        } else {
            // If watcher data wasn't enabled for this planet
            cout << "On planet " << i << ", a movie watcher would not see an interesting ambush.\n";
            cout << "On planet " << i << ", a movie watcher would not see an interesting attack.\n";
        }
    }
}


    
    


int main(int argc, char *argv[]) {
    // insert code here...
    ios_base::sync_with_stdio(false);
    xcode_redirect(argc, argv);
    Options options;
    getOptions(argc, argv, options);
    

    
        string line, mode;
        size_t num_generals = 0, num_planets = 0;

    
    
    while (getline(cin, line)) {
        if (line.compare(0, 8, "COMMENT:") == 0) {
            continue; // Skip comment lines
        } else if (line.compare(0, 5, "MODE:") == 0) {
            mode = line.substr(5);
            mode.erase(0, mode.find_first_not_of(" \t"));
        } else if (line.compare(0, 13, "NUM_GENERALS:") == 0) {
            string val = line.substr(13);
            val.erase(0, val.find_first_not_of(" \t"));
            num_generals = stoul(val);
        } else if (line.compare(0, 12, "NUM_PLANETS:") == 0) {
            string val = line.substr(12);
            val.erase(0, val.find_first_not_of(" \t"));
            num_planets = stoul(val);
            break; // Done reading headers
        }
    }

    

    
    vector<Planets> planet_map(num_planets);
    
    if (options.watcher) {
        for (auto &p : planet_map) {
            p.watcher = new WatcherState();
        }
    }

    
    vector<General> General(num_generals);
    
    uint32_t timestamp = 0;
    uint32_t prevtime = 0;

    uint32_t battles = 0;
    cout << "Deploying troops..." << '\n';
    
    while(cin >> timestamp){
        if(timestamp != prevtime && options.median){
            median(planet_map,prevtime, battles);
        }
        
        read_line(planet_map, timestamp, General, options, prevtime);
        check_fight(planet_map, battles, options, General);
        
        prevtime = timestamp;
    }
    if(options.median){
        median(planet_map,timestamp, battles);
    }
    
    cout << "---End of Day---" << '\n';
    cout << "Battles: " << battles << '\n';
    
    if(options.general_eval){
        print_generaleval(General);
    }
    if(options.watcher){
        print_movie(planet_map);
    }
    
    if (options.watcher) {
        for (auto &p : planet_map) {
            delete p.watcher;
            p.watcher = nullptr;
        }
    }
    
   
    
    return 0;
}
