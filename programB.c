#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

struct ThreadInfo
{
  int id;
  int active;
  pthread_t t_id;
  const char *type;
};

struct ShareData
{
  int buffer[100];
  int active;
};

int thread_count = 0;
struct ThreadInfo ThreadArray[100];

void *count_to_1000(void *arg)
{
  printf("Task is doing........\n");
  for (int i = 0; i < 1000; i++)
  {
    sleep(1);
  }
  return NULL;
}

void *worker_task(void *arg)
{
  // Tạo một thread mới cho task count_to_1000
  pthread_create(&ThreadArray[thread_count].t_id, NULL, count_to_1000, NULL);
  ThreadArray[thread_count].id = thread_count;
  ThreadArray[thread_count].active = 1;
  ThreadArray[thread_count].type = "Task_Thread";
  thread_count++;
  return NULL;
}

void create_worker()
{
  pthread_create(&ThreadArray[thread_count].t_id, NULL, worker_task, NULL);
  ThreadArray[thread_count].id = thread_count;
  printf("Worker thread %d is created\n", ThreadArray[thread_count].id);
  ThreadArray[thread_count].active = 1;
  ThreadArray[thread_count].type = "Worker_Thread";
  thread_count++;
}

void kill_thread(int id)
{
  if (id >= 0 && id < thread_count)
  {
    if (ThreadArray[id].active)
    {
      pthread_cancel(ThreadArray[id].t_id);
      ThreadArray[id].active = 0;
    }
    else
    {
      printf("This thread is already inactive.\n");
    }
  }
  else
  {
    printf("Invalid thread ID: %d\n", id);
  }
}

void list_thread()
{
  printf("Thread list:\n");
  for (int i = 0; i < thread_count; i++)
  {
    if (ThreadArray[i].active != 0)
    {
      printf("Thread ID: %d, ", ThreadArray[i].id);
      printf("Status: %s, ", ThreadArray[i].active ? "Active" : "Inactive");
      printf("Type: %s\n", ThreadArray[i].type);
    }
  }
}

void *thread_manager_task(void *arg)
{
  int fd = *(int *)arg;
  char cmd[200];

  while (1)
  {
    memset(cmd, 0, sizeof(cmd));
    if (read(fd, cmd, sizeof(cmd)) > 0)
    {
      if (strncmp(cmd, "CREATE_THREAD", strlen("CREATE_THREAD")) == 0)
      {
        create_worker();
      }
      else if (strncmp(cmd, "LIST_THREADS", strlen("LIST_THREADS")) == 0)
      {
        list_thread();
      }
      else if (strncmp(cmd, "KILL_THREAD", strlen("KILL_THREAD")) == 0)
      {
        int id;
        if (sscanf(cmd + strlen("KILL_THREAD"), "%d", &id) == 1)
        {
          kill_thread(id);
        }
        else
        {
          fprintf(stderr, "Invalid thread id provided for kill.\n");
        }
      }
    }
  }
  return NULL;
}

int main()
{
  if (mkfifo("/tmp/thread_mgmt_pipe", 0666) < 0)
  {
    if (errno != EEXIST)
    {
      perror("mkfifo");
      exit(EXIT_FAILURE);
    }
  }
  int fd = open("/tmp/thread_mgmt_pipe", O_RDONLY);
  if (fd < 0)
  {
    perror("open FIFO");
    exit(EXIT_FAILURE);
  }

  // Tạo thread quản lý
  struct ThreadInfo *manager = &ThreadArray[thread_count];
  manager->id = thread_count;
  manager->active = 1;
  manager->type = "Manager";

  if (pthread_create(&manager->t_id, NULL, thread_manager_task, &fd) != 0)
  {
    perror("pthread_create failed");
    exit(EXIT_FAILURE);
  }
  printf("Manager Thread Created Successfully\n");
  thread_count++;

  pthread_join(manager->t_id, NULL);
  close(fd);
  return 0;
}
