#include <future>
#include <iostream>

int X = 0, Y = 0;

int thread_1() {
  X = 1;
  asm volatile("" ::: "memory"); // Prevent compiler reordering.
  // asm volatile("mfence" ::: "memory"); // Prevent both compiler and hardware
  // reordering.
  return Y;
}

int thread_2() {
  Y = 1;
  asm volatile("" ::: "memory"); // Prevent compiler reordering.
  // asm volatile("mfence" ::: "memory"); // Prevent both compiler and hardware
  // reordering.
  return X;
}

bool sb() {
  X = 0;
  Y = 0;

  auto fut_y = std::async(&thread_1);
  auto fut_x = std::async(&thread_2);
  int y = fut_y.get();
  int x = fut_x.get();

  return !x && !y;
}

int main() {
  size_t detected = 0, iteration = 0;
  while (true) {
    iteration++;
    if (sb()) {
      detected++;
      std::cout << "Detected " << detected << " reorders out of " << iteration
                << " iterations" << std::endl;
    }
  }
  return 0;
}
