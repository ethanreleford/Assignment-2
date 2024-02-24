#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

class MinotaurLabyrinth
{
private:
    int guests;
    std::mutex mtx;

public:
    MinotaurLabyrinth(int numGuests) : guests(numGuests) {}

    void enterLabyrinth(int guestId)
    {
        std::unique_lock<std::mutex> lock(mtx);
        guests--;
        std::cout << "Guest " << guestId << " entered the labyrinth." << std::endl;
        if (guests == 0)
        {
            std::cout << "All guests have visited the labyrinth." << std::endl;
        }
    }
};

class Guest
{
private:
    int id;
    MinotaurLabyrinth &labyrinth;

public:
    Guest(int guestId, MinotaurLabyrinth &l) : id(guestId), labyrinth(l) {}

    void operator()()
    {
        labyrinth.enterLabyrinth(id);
    }
};

class Showroom
{
private:
    bool available;
    std::mutex mtx;
    std::condition_variable cv;

public:
    Showroom() : available(true) {}

    void enterShowroom(int guestId)
    {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this]()
                { return available; });
        available = false;
        std::cout << "Guest " << guestId << " entered the showroom." << std::endl;
    }

    void exitShowroom(int guestId)
    {
        std::unique_lock<std::mutex> lock(mtx);
        std::cout << "Guest " << guestId << " exited the showroom." << std::endl;
        available = true;
        cv.notify_one();
    }
};

class Guest2
{
private:
    int id;
    Showroom &showroom;

public:
    Guest2(int guestId, Showroom &s) : id(guestId), showroom(s) {}

    void operator()()
    {
        showroom.enterShowroom(id);
        std::this_thread::sleep_for(std::chrono::seconds(2)); // Simulating guest spending some time in the showroom
        showroom.exitShowroom(id);
    }
};

int main()
{
    int numGuests = 5; // Change this to your desired number of guests
    MinotaurLabyrinth labyrinth(numGuests);
    Showroom showroom;

    std::thread guests[numGuests * 2]; // Combined number of guests for both problems

    // Problem 1: Minotaur’s Birthday Party
    for (int i = 0; i < numGuests; ++i)
    {
        guests[i] = std::thread(Guest(i + 1, labyrinth));
    }

    // Problem 2: Minotaur’s Crystal Vase
    for (int i = 0; i < numGuests; ++i)
    {
        guests[i + numGuests] = std::thread(Guest2(i + 1, showroom));
    }

    for (int i = 0; i < numGuests * 2; ++i)
    {
        guests[i].join();
    }

    return 0;
}
