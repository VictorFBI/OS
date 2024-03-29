#include <iostream>
#include <unistd.h>

int main() {
  pid_t pid, ppid, chpid;
  size_t number;
  std::cout << "Enter non-negative number: ";
  std::cin >> number;

  chpid = fork();
  pid = getpid();
  ppid = getppid();

  if (!chpid) { // if child then computing factorial of n
    size_t ans = 1;
    bool isBreak = false;

    for (size_t i = 1; i <= number; ++i) {
      if (UINT64_MAX / i < ans) {
        isBreak = true;
        break;
      }
      ans *= i;
    }

    if (isBreak) {
      printf("I am CHILD, my_id=%d, parent_id=%d, child_id=%d, result (of factorial)=(sorry, you are out of range)) \n",
             pid,
             ppid,
             chpid);
    } else {
      printf("I am CHILD, my_id=%d, parent_id=%d, child_id=%d, result (of factorial)=%zu \n", pid, ppid, chpid, ans);
    }

  } else { // else parent then computing nth Fibonacci number
    size_t a = 0; // zero fib number
    size_t b = 1; // first fib number
    bool isBreak = false;

    for (size_t i = 2; i <= number; ++i) {
      if (UINT64_MAX - b < a) {
        isBreak = true;
        break;
      }
      size_t c = a + b;
      a = b;
      b = c;
    }

    if (!number) b = 0;

    if (isBreak) {
      printf(
          "I am PARENT, my_id=%d, parent_id=%d, child_id=%d, result (of nth fib number)=(sorry, you are out of range)) \n",
          pid,
          ppid,
          chpid);
    } else {
      printf("I am PARENT, my_id=%d, parent_id=%d, child_id=%d, result (of nth fib number)=%zu \n",
             pid,
             ppid,
             chpid,
             b);
    }
  }

  sleep(1); // to format output

  if (chpid) { // if process is not child then output the information about content of the working directory
    std::cout << "Content of the current working directory: \n";
    system("ls");
  }

  return 0;
}
