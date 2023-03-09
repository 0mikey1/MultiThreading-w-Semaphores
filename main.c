#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#define THREAD_NUM 3
#define COUNT 2000000

sem_t semaphore; // declare semaphore

struct shared_data 
{
  int value;     /* shared variable to store result*/
};

struct shared_data *counter; // used to store shared counter

void *bonus_thread(void *arg)
{
  
  int bonus_count = 0;
  for (int i = 0; i < COUNT; i++)
  {
    // entry Section
    if (sem_trywait(&semaphore) == 0) // tries to lock the semaphore, if 0, successful
    {
    // critical Section
      counter -> value++;
      // checks for bonus
      if(counter -> value % 100 == 0)
      {
        counter -> value += 100;
        bonus_count++;
      }
      // exit Section
      sem_post(&semaphore);
    }
    else
    {
      // remainder Section
      ; // decrements to retry iteration if failed
    }
  }
  
 
  
  printf("Im thread1, I did %d updates and I got the bonus for %d times, counter = %d\n", COUNT, bonus_count, counter -> value);
  
}

void *second_thread(void *arg)
{
  
  for (int i = 0; i < COUNT; i++)
  {
  //entry Section
  if(counter -> value > COUNT * THREAD_NUM)
    {
      break;
    }
    
    if(sem_trywait(&semaphore) == 0) // tries to lock the semaphore, if successful enter the critical section
    {
    // critical Section
      counter -> value++;
    // exit Section
      sem_post(&semaphore);
    }
    else
    {
    // remainder Section
      i--; // decrements to retry iteration if failed
    }
    if(i == COUNT - 1) //if the last iteration, checks the counter and increments until it reaches the bonus count
    {
      while(counter -> value < COUNT * THREAD_NUM)
      {
        counter -> value++;
      }
    }
  }
  
  
  printf("Im thread2, I did %d updates, counter = %d\n", COUNT, counter->value);
  
}

void *third_thread(void *arg)
{
  
  for (int i = 0; i < COUNT; i++)
  {
    //entry Section
    if(counter -> value >= COUNT * THREAD_NUM)
    {
      break;
    }
    
    if(sem_trywait(&semaphore) == 0) // tries to lock the semaphore, if successful enter the critical section
    {
      // critical Section
      counter -> value++;
      // exit Section
      sem_post(&semaphore);
    }
    else
    {
      // remainder Section
      i--; // decrements to retry iteration if failed
    }
  }
  
  printf("Im thread3, I did %d updates, counter = %d\n", COUNT, counter->value);
  
}

int main(int argc, char* argv[]) {
    
    pthread_t th1, th2, th3;
    

    counter = (struct shared_data *) malloc(sizeof(struct shared_data)); // dyn. allocate memory using malloc
    counter->value = 0; // initializes shared data to 0
    
    sem_init(&semaphore, 0, 1); // init. semaphore 
  // spawn first thread
    pthread_attr_t attr1; 
    pthread_attr_init(&attr1);
    pthread_attr_setscope(&attr1, PTHREAD_SCOPE_SYSTEM);
    if(pthread_create(&th1, &attr1, bonus_thread, NULL) != 0)
    {
      printf("Error creating thread1\n");
      return EXIT_FAILURE;
    }
    
    // spawn second thread
    pthread_attr_t attr2;
    pthread_attr_init(&attr2);
    pthread_attr_setscope(&attr2, PTHREAD_SCOPE_SYSTEM);
    if(pthread_create(&th2, &attr2, second_thread, NULL) != 0)
    {
      printf("Error creating thread2\n");
      return EXIT_FAILURE;
    }
    
    // spawn third thread
    pthread_attr_t attr3;
    pthread_attr_init(&attr3);
    pthread_attr_setscope(&attr3, PTHREAD_SCOPE_SYSTEM);
    if(pthread_create(&th3, &attr3, third_thread, NULL) != 0)
    {
      printf("Error creating thread3\n");
      return EXIT_FAILURE;
    }
    
    pthread_join(th1, NULL); // waits for th1 to terminate 
    pthread_join(th2, NULL); // waits for th2 to terminate 
    pthread_join(th3, NULL); // waits for th3 to terminate 
    
    sem_destroy(&semaphore); // kill semaphore when t1, t2, and t3 are terminated 
    
    printf("from parent counter = %d\n", counter->value);

    free(counter); // deallocate memory that was used for counter 
    
    return 0;
}
