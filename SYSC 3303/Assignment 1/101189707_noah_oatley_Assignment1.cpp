//Created by Noah Oatley 101189707

// Includes
#include <chrono>
#include <thread>
#include <iostream>
#include <random>
#include <mutex>
#include <condition_variable>
#include <stdlib.h>
#include <string>
#include <time.h>
#include <set>
#include <atomic>

const int sandwichCount = 20;
std::string foods[4]={"Nothing","Bread", "PeanutButter", "Jam"};

// Box Class creation
template <typename Type> class Box {
private:
    int foodNum1;
    int foodNum2;
    bool isEmpty = true; // Whether box is empty or not
    std::mutex boxlock; // Lock on box
    std::condition_variable_any cond; // Condition
    int sandwichNum = 0; // Count of sandwich

public:
    Box() : isEmpty(true), boxlock(), cond(), foodNum1(int()), foodNum2(int()) {} // Constructor, default values

    int put(int item1, int item2) {
        std::unique_lock<std::mutex> lock(boxlock); // Lock the mutex
        if(!isEmpty) cond.wait(boxlock); // Wait for box to be empty
        foodNum1 = item1; foodNum2 = item2; // Put items into box
        isEmpty = false; 
        cond.notify_all(); //Notify that box is full now
        sandwichNum++; // Sandwich count increases when ingredients are put into box
        return sandwichNum;
    }

    int get() {
        std::unique_lock<std::mutex> lock(boxlock); // Lock mutex
        if(isEmpty) cond.wait(boxlock); // Wait for box to be full        
        int item1 = foodNum1; int item2 = foodNum2;
        isEmpty = true;
        cond.notify_all();
        return sandwichNum;
    }

    void boxInquiry(int& returnfood1, int& returnfood2) { // Allow Chef to check which 2 ingredients are on table
        returnfood1 = foodNum1;
        returnfood2 = foodNum2;
    }
};

template <typename Type> class Agent {
private: 
    std::string name;
    Box<Type>& box;
    int chosenFood1;
    int chosenFood2;
    std::default_random_engine generator;

public:
    Agent(Box<Type>& foodbox) : name(), box(foodbox), generator(){}

    void operator() (const std::string& name) {
        for(int i=0; i<sandwichCount; i++) {
            std::set<int> foodset = chooseFoods();
            chosenFood1 = *next(foodset.begin(),0); // Getting indexes from choose function
            chosenFood2 = *next(foodset.begin(), 1);
            std::cout << name << " produced " << foods[chosenFood1]  << "(" << chosenFood1 << ")"<< " and " << foods[chosenFood2] << "(" << chosenFood2 << ")" << std::endl;
            int sandwich = box.put(chosenFood1, chosenFood2);
            std::cout << name << " put items in box. Number of sandwiches: " << sandwich << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
        std::cout << "Thread: " << name << " ending. " << std::endl;
        return;
    }

    std::set<int> chooseFoods() {
        std::uniform_int_distribution<int> gen(1,3); // Random number between 1 and 3
        std::set<int> foodgens;

        int foodIndex1 = gen(generator);
        foodgens.insert(foodIndex1);

        int foodIndex2 = gen(generator);
        auto result = foodgens.insert(foodIndex2); // Using set to ensure no dupes

        while(!result.second) { // Ensure there are two ingredients
            foodIndex2 = gen(generator);
            result = foodgens.insert(foodIndex2); // Redo choose if dupes
        }
        
        return foodgens;
    }
};

template <typename Type> class Chef {
private:
    std::string name;
    Box<Type>& box;
    int foodNum;
    std::string foodType = foods[foodNum];

public:
    Chef(Box<Type>& foodbox, int foodnumber) : name(), box(foodbox), foodNum(foodnumber){}

    void operator() (const std::string& name){
        for(int i=0; i<sandwichCount; i++) {
            std::this_thread::sleep_for(std::chrono::seconds(1)); // Give time for agent to insert ingredients
            int foodCheck1; int foodCheck2;
            box.boxInquiry(foodCheck1, foodCheck2); // Checking the two ingredients
            bool canMake = false;
            canMake = checkFood(foodCheck1, foodCheck2);

            if(canMake) { // If neither ingredients in box are the one they have, make sandwich
                int sandwich = box.get();
                std::cout << name << ": producer of " << foodType << " can make and eat the sandwich " << std::endl;
                std::cout << name << " ate sandwich number: " << sandwich << std::endl;
                if (sandwich == sandwichCount) return; // Terminate when ate final sandwich
            }
            canMake = false; // Ensure they don't accidentally create next sandwich
            std::this_thread::sleep_for(std::chrono::seconds(1)); 
        }
        return;
    }

    bool checkFood(int food1, int food2) {
        if(foodNum != food1 && foodNum != food2) return true; // If both ingredients are not the one the chef has, returns that they can make it
        return false;
    }
};


int main(int argc, char ** argv)
{    
    Box<int> box;
    Agent<int> agent(box);
    Chef<int> Chef1(box, 1);
    Chef<int> Chef2(box, 2);
    Chef<int> Chef3(box, 3);

    std::thread producer (agent, "AgentFoodMaker");
    std::thread consumer1 (Chef1, "BreadChef");
    std::thread consumer2 (Chef2, "PBChef");
    std::thread consumer3 (Chef3, "JamChef");

    producer.join();
    consumer1.join();
    consumer2.join();
    consumer3.join();

    return 0;
}