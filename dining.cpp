#include <semaphore>
#include <iostream>
#include <thread>
#include <Windows.h>
#include <array>
#include <vector>

#define N 5
#define THINKING 2
#define HUNGRY 1
#define EATING 0
#define LEFT (phnum + 4) % N
#define RIGHT (phnum + 1) % N

const int eating_time = 100;
const int ITER = 10;
int state[N];
int phil[N] = { 0, 1, 2, 3, 4 };

std::counting_semaphore<2> mutex(1);
std::array<std::counting_semaphore<2>, N> S = {
		std::counting_semaphore<2>(0),
		std::counting_semaphore<2>(0),
		std::counting_semaphore<2>(0),
		std::counting_semaphore<2>(0),
		std::counting_semaphore<2>(0),
};
std::vector<std::thread> threads(N);

void test(int phnum) {
	//check the philosopher is hungry, and neither left and right is currently eating
	if (state[phnum] == HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING) { 
		state[phnum] = EATING; //change state of philosopher to EATING
		Sleep(eating_time); //Eating for 5 seconds
		std::cout << "Philosopher " << phnum  << " has taken fork " << LEFT << " and " << phnum << std::endl;
		std::cout << "Philosopher " << phnum << " is eating\n";
		S[phnum].release(); //signal semaphore of philosopher
	}
}
void take_fork(int phnum) {
	mutex.acquire(); //Semaphore wait()
	state[phnum] = HUNGRY; //change state of philosopher to HUNGRY
	std::cout << "Philosopher " << phnum << " is hungry\n";
	test(phnum); //Test if adjacent philosophers are eating
	mutex.release(); //Semaphore signal()
	S[phnum].acquire(); //If cannot currently eat, wait
	Sleep(1);
}

void put_fork(int phnum) {
	mutex.acquire(); //Semaphore wait()
	state[phnum] = THINKING; // change state of philosopher to THINKING
	std::cout << "Philosopher " << phnum << " has put fork " << LEFT << " and " << phnum << " down\n";
	std::cout << "Philosopher " << phnum << " is thinking\n";
	test(LEFT);
	test(RIGHT);
	mutex.release();
}
void* philosophers(int* num) {
	//function for the philosopher's itself
	for (int i = 0; i < ITER; i++) {
		int* n = num;
		Sleep(1);
		take_fork(*n); //take fork and eat
		Sleep(1);
		put_fork(*n); //put fork then think
	}
	return 0;
}

int main() {
	for (int i = 0; i < N; i++) {
		threads[i] = std::thread(philosophers, &phil[i]);
	}
	for (int i = 0; i < N; i++) {
		threads[i].join();
	}
	return 0;
}
