#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <vector>
#include <algorithm>

bool toBreak = false;
std::vector<int> bits;

void my_build_number_handler(int nsig) {
  if (nsig == SIGUSR2) {
    bits.push_back(1);
  } else {
    bits.push_back(0);
  }
}

void my_terminate_handler(int nsig) {
  toBreak = true;
}

int main() {
  bits.reserve(32);
  signal(SIGUSR1, my_build_number_handler);
  signal(SIGUSR2, my_build_number_handler);
  signal(SIGTERM, my_terminate_handler);
  pid_t pid = getpid();
  std::cout << "RECEIVER\n";
  std::cout << "My pid is " << pid;
  pid_t sender_pid;
  std::cout << "\nEnter sender id: ";
  std::cin >> sender_pid;
  while (!toBreak) {
    kill(sender_pid, SIGUSR1);
    pause();
  }

  std::reverse(bits.begin(), bits.end());
  std::cout << "Number has been received. ";
  std::cout << "Bits of the number are ";
  for (auto i = 0; i < 32; ++i) {
    std::cout << bits[i];
  }
  std::cout << '\n';
  int multiplier = 1;
  int ans = 0;
  int mask = 0;
  if (bits[0] == 1) mask = 1;

  for (int i = 31; i > 0; --i) {
    ans += (bits[i] ^ mask) * multiplier;
    multiplier *= 2;
  }

  if (mask) ans = -ans - 1;
  std::cout << "Value of the number is " << ans << '\n';

  return 0;
}