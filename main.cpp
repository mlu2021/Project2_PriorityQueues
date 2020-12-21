// PROJECT IDENTIFIER: 9504853406CBAC39EE89AA3AD238AA12CA198043
//  main.cpp
//  zombies
//
//  Created by Michelle Lu on 10/2/19.
//  Copyright © 2019 Michelle Lu. All rights reserved.
//

#include <stdio.h>
#include <iostream>
#include <getopt.h>
#include <deque>
#include <queue>
#include "P2random.h"
#include "Zombie.h"
#include "xcode_redirect.hpp"
#include <string>

using namespace std;

bool verboseMode = false;
bool showMedian = false;
bool showStats = false;
size_t statsCount = 0;
size_t quiver_capacity = 20;
unsigned int rand_seed = 0;
unsigned int max_rand_distance = 0;
unsigned int max_rand_speed = 0;
unsigned int max_rand_health = 0;
bool randInitialized = false;

// A stack to hold all zombies in the order they are created so far
deque<Zombie> mc;
// A stack to hold all killed zombies for printing stats purpose
deque<Zombie*> killed_zombies;

// functor to compare ETA for zombies
struct CmpZombiePtrs {
    bool operator()(const Zombie* lhs, const Zombie* rhs) const {
        if (lhs->getETA() == rhs->getETA()) {
            if (lhs->getHealth() == rhs->getHealth()) {
                return lhs->getName() > rhs->getName();
            }
            return lhs->getHealth() > rhs->getHealth();
        }
        return lhs->getETA() > rhs->getETA();
    }
};

struct CompRoundsMost {
    bool operator()(const Zombie* lhs, const Zombie* rhs) const {
        if (lhs->getRounds() == rhs->getRounds()) {
            return lhs->getName() > rhs->getName();
        }
        return lhs->getRounds() < rhs->getRounds();
    }
};

struct CompRoundsLeast {
    bool operator()(const Zombie* lhs, const Zombie* rhs) const {
        if (lhs->getRounds() == rhs->getRounds()) {
            return lhs->getName() > rhs->getName();
        }
        return lhs->getRounds() > rhs->getRounds();
    }
};

template<class T>
class CalculateMedian {
private:
    priority_queue<T, vector<T>, greater<T>> upperHalf;
    priority_queue<T, vector<T>, less<T>> lowerHalf;
    
public:
    CalculateMedian() {
        upperHalf.push(numeric_limits<T>::max());
        lowerHalf.push(numeric_limits<T>::min());
    }
    
    void push(T num) {
        if (num >= upperHalf.top()) {
            upperHalf.push(num);
        }
        else {
            lowerHalf.push(num);
        }
        
        if (upperHalf.size()-lowerHalf.size() == 2) {
            lowerHalf.push(upperHalf.top());
            upperHalf.pop();
        }
        else if (lowerHalf.size()-upperHalf.size() == 2) {
            upperHalf.push(lowerHalf.top());
            lowerHalf.pop();
        }
    }
    
    T getMedian() const {
        if (upperHalf.size() == lowerHalf.size()) {
            return ((upperHalf.top() + lowerHalf.top())/static_cast<unsigned int>(2.0));
        }
        else if (upperHalf.size() > lowerHalf.size()) {
            return upperHalf.top();
        }
        else {
            return lowerHalf.top();
        }
    }
};

CalculateMedian<unsigned int> median; // MOVED CLASS DECLARATION HERE

// A priority queue based on the ETA of a zombie to the player.
// Every round, the top() element will be the target to be shot first.
// If a zombie is shot, it will be pop() off the queue.
priority_queue<Zombie*, vector<Zombie*>, CmpZombiePtrs> zombiePQ;

// Get the options from command line
void get_options(int argc, char** argv) {
    int option_index = 0, option = 0;
    
    struct option longOpts[] = {
        {"help", no_argument, nullptr, 'h'},
        {"verbose", no_argument, nullptr, 'v'},
        {"statistics", required_argument, nullptr, 's'},
        {"median", no_argument, nullptr, 'm'},
        { nullptr, 0, nullptr, '\0' }
    };
    
    while ((option = getopt_long(argc, argv, "hvs:m", longOpts,
                                 &option_index)) != -1) {
        switch (option) {
            case 'h':
                cout << "This program simulates a zombie apocalypse and prints\n"
                <<  "out the results of the game.\n"
                <<  "Usage: \'./zombbb\n\t[--help | -h]\n"
                <<                      "\t[--verbose | -v]\n"
                <<                      "\t[--statistics | -s] <num>\n"
                <<                      "\t[--median | -m]\n"
                <<                      "\t< inputfile>\n";
                exit(0);
            case 's':
                if (optarg) {
                    statsCount = static_cast<size_t>(stoi(optarg));
                    showStats = true;
                }
                else {
                    exit(1);
                }
                break;
            case 'v':
                verboseMode = true;
                break;
            case 'm':
                showMedian = true;
                break;
            default:
                exit(1);
        }
    }
}

void print_stats() {
    size_t count = 0;
    priority_queue<Zombie*, vector<Zombie*>, CompRoundsMost> roundsMostPQ;
    priority_queue<Zombie*, vector<Zombie*>, CompRoundsLeast> roundsLeastPQ;
    
    // The number of zombies still active
    cout << "Zombies still active: " << zombiePQ.size() << "\n";
    
    // Make sure that we are not printing more then the killed zombies
    size_t num_to_print;
    if (statsCount > killed_zombies.size()) {
        num_to_print = killed_zombies.size();
    }
    else {
        num_to_print = statsCount;
    }
    
    // The names of the first N zombies killed by the order they were killed
    cout << "First zombies killed:\n";
    if (!killed_zombies.empty()) {
        for (size_t i = 0; i < num_to_print; i++) {
            cout << killed_zombies[i]->getName() << " " << i + 1 << "\n";
        }
    }
    // The names of the last N zombied killed by the reverse order they were killed
    cout << "Last zombies killed:\n";
    if (!killed_zombies.empty()) {
        for (size_t i = killed_zombies.size() - 1; i >= killed_zombies.size() - num_to_print; i--) {
            cout << killed_zombies[i]->getName() << " " << num_to_print - count << "\n";
            count++;
            if (i == 0) {
                break;
            }
        }
    }
    
    // reset count
    count = 0;
    
    // Build the priority queues for printing
    for (size_t i = 0; i < mc.size(); i++) {
        roundsMostPQ.push(&mc[i]);
        roundsLeastPQ.push(&mc[i]);
    }
    
    cout << "Most active zombies:\n";
    while (!roundsMostPQ.empty() && count < statsCount) {
        cout << roundsMostPQ.top()->getName() << " " << roundsMostPQ.top()->getRounds() << "\n";
        ++count;
        roundsMostPQ.pop();
    }
    
    count = 0;
    cout << "Least active zombies:\n";
    while (!roundsLeastPQ.empty() && count < statsCount) {
        cout << roundsLeastPQ.top()->getName() << " " << roundsLeastPQ.top()->getRounds() << "\n";
        roundsLeastPQ.pop();
        ++count;
    }
}

void create_rand_zombies(size_t rand_zombie_num) {
    // If P2random has not been initialized, do it first.
    if (!randInitialized) {
        P2random::initialize(rand_seed, max_rand_distance, max_rand_speed, max_rand_health);
        randInitialized = true;
    }
    
    // Create random zombies by the given number
    for (size_t i = 0; i < rand_zombie_num; i++) {
        string name = P2random::getNextZombieName();
        unsigned int distance = P2random::getNextZombieDistance();
        unsigned int speed = P2random::getNextZombieSpeed();
        unsigned int health = P2random::getNextZombieHealth();
        
        Zombie z = Zombie(name, distance, speed, health);
        
        // Add them to the master queues
        mc.push_back(z);
        
        // Add zombie to priority quque
        zombiePQ.push(&mc.back());
        
        if (verboseMode) {
            cout << "Created: " << name << " (distance: " << distance << ", speed: " << speed << ", health: " << health << ")\n";
        }
    }
}

void create_named_zombies(size_t named_zombie_num) {
    // Read number of random zombies
    string label;
    string value;
    string name;
    
    for (size_t i = 0; i < named_zombie_num; i++) {
        unsigned int distance;
        unsigned int speed;
        unsigned int health;
        
        cin >> name;
        cin >> label >> value;
        distance = static_cast<unsigned int>(stoi(value));
        
        cin >> label >> value;
        speed = static_cast<unsigned int>(stoi(value));
        
        cin >> label >> value;
        health = static_cast<unsigned int>(stoi(value));
        
        Zombie z = Zombie(name, distance, speed, health);
        
        // Add them to the master queues
        mc.push_back(z);
        
        // Add zombie to priority queue
        zombiePQ.push(&mc.back());

        if (verboseMode) {
            cout << "Created: " << name << " (distance: " << distance << ", speed: " << speed << ", health: " << health << ")\n";
        }
    }
}

// VOID RUN_ROUND
// start with the arrow count with quiver_capacity
void run_round(size_t round_num, size_t rand_zombie_num, size_t named_zombie_num) {
    size_t arrow_num = quiver_capacity;
    bool player_dead = false;
    string killer_name;
    
    if (verboseMode) {
        cout << "Round: " << round_num << "\n";
    }
    
    // Move all active zombies (from the previous round)
    for (size_t i = 0; i < mc.size(); i++) {
        
        if (mc[i].getHealth() > 0) {
            // zombie move
            mc[i].move();
            
            if (verboseMode) {
                cout << "Moved: " << mc[i].getName() << " (distance: " << mc[i].getDistance() << ", speed: " << mc[i].getSpeed() << ", health: " << mc[i].getHealth() << ")\n";
            }
            
            // If zombie has reached player, player dies and game over
            // but keep looping through to update all active zombies so continue the loop
            if (mc[i].getDistance() == 0) {
                player_dead = true;
                
                // When multiple zombies can kill the player, only print the first zombie name
                if (killer_name.length() == 0) {
                    killer_name = mc[i].getName();
                }
            }
            // increase the round by 1
            mc[i].increaseRound();
        }
    }
    
    // If player is dead, please stats
    // Otherwise new zombies are created
    if (player_dead) {
        if (showMedian && !killed_zombies.empty()) {
            cout << "At the end of round " << round_num << ", the median zombie lifetime is " << median.getMedian() << "\n";
        }
        cout << "DEFEAT IN ROUND " << round_num << "! " << killer_name << " ate your brains!\n";
        // ADDED SHOW STATS HERE
        if (showStats) {
            print_stats();
        }
        exit(0);
    }
    else {
        // Create new zombies
        // ramdom zombies first
        create_rand_zombies(rand_zombie_num);
        
        // create name zombies
        create_named_zombies(named_zombie_num);
        
        // Battle starts here:
        // Player shoots zombies with the least ETA, by poping the priority queue
        // Player would try to kill as many zombies as possible until running out the arrows or no more zombies
        // TODO: need to handle the tie situation!
        while (!zombiePQ.empty() && arrow_num > 0) {
            // Player would use available arrows to kill the closest zombie
            // If a zombie is dead, remove from the priority queue
            if ((zombiePQ.top()->getHealth()) <= arrow_num) {
                // Adjust the arrows used to kill the zombie
                arrow_num -= (zombiePQ.top()->getHealth());
                
                // The top zombie is killed
                zombiePQ.top()->setHealth(0);
                
                // Print the kill
                if (verboseMode) {
                    cout << "Destroyed: " << zombiePQ.top()->getName() << " (distance: " << zombiePQ.top()->getDistance() << ", speed: " << zombiePQ.top()->getSpeed() << ", health: 0)\n";
                }
                
                // Remove the zombie from the PQ
                // push # rounds active for zombie into median class
                if (showMedian) {
                    median.push(zombiePQ.top()->getRounds());
                }
                // If need to show stats, add the killed zombie to the stack
                if (showStats) {
                    killed_zombies.push_back(zombiePQ.top());
                }
                zombiePQ.pop();
            }
            else {
                // player used all available arrows to redue the health of the top zombie
                //TODO: check to see how to better cast to unsigned int??
                zombiePQ.top()->setHealth((unsigned int)((zombiePQ.top()->getHealth()) - arrow_num));
                
                // No arrow left for this round
                arrow_num = 0;
            }
        }
    }
    
    //is this most efficient way of doing it bc we're creating a new median object each time?
    if (showMedian && !killed_zombies.empty()) {
        cout << "At the end of round " << round_num << ", the median zombie lifetime is " << median.getMedian() << "\n";
    }
    
}

// HOW TO READ INPUT

void read_rounds() {
    string separator;
    string label;
    string value;
    size_t current_round = 0;
    
    // Read the separator first
    cin >> separator;
    
    // use --- as the separator for a new round
    while (separator.compare("---") == 0) {
        // Read round number
        cin >> label >> value;
        
        size_t round_num = static_cast<size_t>(stoi(value));
        
        // If the round in the input file is not the next consective round of the current round,
        // keep running the skipped rounds with new zombies
        while (round_num > current_round + 1) {
            // Advanced the current round count
            ++current_round;
            run_round(current_round, 0, 0);
        }
        
        // Read number of random zombies
        cin >> label >> value;
        
        size_t rand_zombie_num = static_cast<size_t>(stoi(value));
        
        // Read number of named zombies
        cin >> label >> value;
        
        size_t named_zombie_num = static_cast<size_t>(stoi(value));
        
        run_round(round_num, rand_zombie_num, named_zombie_num);
        
        // record the current round
        current_round = round_num;
        
        // reset the separator to handle the last round edge case
        separator = "";
        
        cin >> separator;
        
        // Check whether all zombies have been killed.
        // if so, check whether there is more rounds defined in the input file.
        // if not, you won the game! Otherwise, keep going with the while loop.
        if (zombiePQ.empty() && separator.compare("---") != 0) {
            cout << "VICTORY IN ROUND " << round_num << "! " << killed_zombies.back()->getName() << " was the last zombie.\n";
            // Print stats if needed
            if (showStats) {
                print_stats();
            }
            exit(0);
        }
    }
    
    // If there are still active zombies, continue to run rounds without
    // creating any new zombies
    while (zombiePQ.size() > 0) {
        run_round(++current_round, 0, 0);
        
        // Win after running a round and no more active zombies
        if (zombiePQ.empty()) {
            cout << "VICTORY IN ROUND " << current_round << "! " << killed_zombies.back()->getName() << " was the last zombie.\n";
            
            // Print stats if needed
            if (showStats) {
                print_stats();
            }
            exit(0);
        }
    }
}

void read_header() {
    string inputline;
    
    // Skip comment lines
    // while (inputline.front() == '#' || inputline.empty()) {
    //    cout << " got inputline: " << inputline << endl;
    //    getline(cin, inputline);
    // }
    getline(cin, inputline);
    
    string label;
    string value;
    
    cin >> label >> value;
    quiver_capacity = static_cast<size_t>(stoi(value));
    
    cin >> label >> value;
    rand_seed = static_cast<unsigned int>(stoi(value));
    
    cin >> label >> value;
    max_rand_distance = static_cast<unsigned int>(stoi(value));
    
    cin >> label >> value;
    max_rand_speed = static_cast<unsigned int>(stoi(value));
    
    cin >> label >> value;
    max_rand_health = static_cast<unsigned int>(stoi(value));
    
}

int main(int argc, char** argv) {
    ios_base::sync_with_stdio(false);
    xcode_redirect(argc, argv);
    get_options(argc, argv);
    
    // Read the header info
    read_header();
    
    // Read rounds
    read_rounds();
    
    return 0;
}
