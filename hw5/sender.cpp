#include <iostream>
#include <signal.h>
#include <unistd.h>

void custom_handler(int nsig) {
}

int main() {
  signal(SIGUSR1, custom_handler);
  pid_t pid = getpid();
  std::cout << "SENDER\n";
  std::cout << "My pid is " << pid << '\n';
  pid_t receiver_pid;
  std::cout << "Enter receiver id: ";
  std::cin >> receiver_pid;
  std::cout << "Enter number: ";
  int num;
  std::cin >> num;
  std::cout << "\n";

  for (int i = 0; i < 32; ++i) {
    int bit = num & 1;
    if (!bit) {
      kill(receiver_pid, SIGUSR1);
    } else {
      kill(receiver_pid, SIGUSR2);
    }
    num >>= 1;
    pause(); // waiting for signal (method from unistd.h)
  }

  kill(receiver_pid, SIGTERM);
  return 0;
}