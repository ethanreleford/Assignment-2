#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>

std::mutex mtx;
int guests_visited = 0; // Number of guests visited
const int N = 5;       // Number of guests
int cupcakes_left = 1; // Initial cupcakes left at the exit

void visitLabyrinth(int guest_id)
{
    // Simulate guest visiting labyrinth
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Each guest announces their visit
    std::lock_guard<std::mutex> lock(mtx);
    std::cout << "Guest " << guest_id << " visited the labyrinth." << std::endl;

    // Check if cupcake is available
    if (cupcakes_left > 0)
    {
        std::cout << "Guest " << guest_id << " found a cupcake and ate it." << std::endl;
        cupcakes_left--;
    }
    else
    {
        std::cout << "Guest " << guest_id << " found no cupcake." << std::endl;
    }

    guests_visited++;
}

class Showroom
{
private:
    bool available;
    std::mutex mtx;

public:
    Showroom() : available(true) {}

    void enterShowroom(int guestID)
    {
        std::unique_lock<std::mutex> lock(mtx);
        while (!available)
        {
            std::cout << "Guest " << guestID << " is waiting. Showroom is busy." << std::endl;
            // Guest has to come back and check again later
            lock.unlock();
            std::this_thread::sleep_for(std::chrono::seconds(1));
            lock.lock();
        }
        std::cout << "Guest " << guestID << " is entering the showroom." << std::endl;
        available = false;
    }

    void exitShowroom(int guestID)
    {
        std::unique_lock<std::mutex> lock(mtx);
        std::cout << "Guest " << guestID << " is exiting the showroom." << std::endl;
        available = true;
    }
};

void guest(Showroom &showroom, int guestID)
{
    showroom.enterShowroom(guestID);
    // Guest is in the showroom
    std::this_thread::sleep_for(std::chrono::seconds(3)); // Viewing the vase for 3 seconds
    showroom.exitShowroom(guestID);
}

int main()
{
    std::vector<std::thread> threads;

    // Create threads for each guest
    for (int i = 1; i <= N; ++i)
    {
        threads.emplace_back([i]
                             { visitLabyrinth(i); });
    }

    // Join threads
    for (auto &t : threads)
    {
        t.join();
    }

    // Check if all guests have visited the labyrinth
    if (guests_visited == N)
    {
        std::cout << "All guests have visited the labyrinth at least once." << std::endl;
    }
    else
    {
        std::cout << "Not all guests have visited the labyrinth." << std::endl;
    }


    const int numGuests = 10; // Specify the number of guests
    Showroom showroom;
    std::thread guests[numGuests];

    for (int i = 0; i < numGuests; ++i)
    {
        guests[i] = std::thread(guest, std::ref(showroom), i);
    }

    for (int i = 0; i < numGuests; ++i)
    {
        guests[i].join();
    }

    return 0;
}
