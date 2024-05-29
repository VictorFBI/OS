#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <signal.h>

int sem_id;
int pipe_fd[2];

// Функция для управления семафором
void sem_op(int sem_id, int sem_num, int sem_op) {
    struct sembuf sem_b;
    sem_b.sem_num = sem_num;
    sem_b.sem_op = sem_op;
    sem_b.sem_flg = 0;
    if (semop(sem_id, &sem_b, 1) == -1) {
        perror("semop");
        exit(EXIT_FAILURE);
    }
}

// Обработчик сигнала прерывания
void handle_sigint(int sig) {
    printf("\nSignal received, terminating processes...\n");
    semctl(sem_id, 0, IPC_RMID);
    semctl(sem_id, 1, IPC_RMID);
    semctl(sem_id, 2, IPC_RMID);
    close(pipe_fd[0]);
    close(pipe_fd[1]);	
    exit(EXIT_SUCCESS);
}

int main() {
    pid_t pid;
    key_t key = ftok("semfile", 65);
    
    // Создаем семафор
    if ((sem_id = semget(key, 3, 0666 | IPC_CREAT)) == -1) {
        perror("semget");
        exit(EXIT_FAILURE);
    }

    // Инициализация семафоров
    semctl(sem_id, 0, SETVAL, 1);  // Семафор для родительского процесса (первый созданный в semget)
    semctl(sem_id, 1, SETVAL, 0);  // Семафор для дочернего процесса (второй созданный в semget)
    semctl(sem_id, 2, SETVAL, 0);  // Вспомогательный семафор для родительского процесса (третий созданный в semget)

    // Создаем pipe
    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Устанавливаем обработчик сигнала прерывания
    signal(SIGINT, handle_sigint);

    // Создаем дочерний процесс
    if ((pid = fork()) == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid > 0) { // Родительский процесс
	int i = 0;
        for (;;) {
	    sleep(1);	
            sem_op(sem_id, 0, -1); // Ждем возможности отправить сообщение
            char message[100];
            snprintf(message, sizeof(message), "Message %d from parent", ++i);
            write(pipe_fd[1], message, sizeof(message));
            printf("Parent sent: %s\n", message);
            sem_op(sem_id, 1, 1); // Разрешаем дочернему процессу отправить сообщение
            sem_op(sem_id, 2, -1); // Ждем возможности прочитать сообщение
            char buffer[256];
            int n = read(pipe_fd[0], buffer, sizeof(buffer));
            if (n > 0) {
                buffer[n] = '\0';
                printf("Parent received: %s\n", buffer);
            }
        }

    } else { // Дочерний процесс
        int i = 0;
        while (1) {
            sem_op(sem_id, 1, -1); // Ждем возможности отправить сообщение
            char buffer[256];
            int n = read(pipe_fd[0], buffer, sizeof(buffer));
            if (n > 0) {
                buffer[n] = '\0';
                printf("Child received: %s\n", buffer);
            }
            char message[100];
            snprintf(message, sizeof(message), "Message %d from child", ++i);
            write(pipe_fd[1], message, sizeof(message));
            printf("Child sent: %s\n", message);
            sem_op(sem_id, 0, 1); // Разрешаем родительскому процессу отправить сообщение
            sem_op(sem_id, 2, 1); // Разрешаем родительскому процеесу прочитать сообщение
        }
    }

    return 0;
}
