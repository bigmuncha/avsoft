#include <unistd.h>
#include <pthread.h>

#include <string>
#include <iostream>
#include <random>

#define READERS_NUMBER 5
#define WRITERS_NUMBER 5
#define READ_TIME 1
#define WRITE_TIME 2

#define READ_COUNT 40
#define WRITE_COUNT 50

pthread_mutex_t g_lock;
pthread_mutex_t read_lock;
static unsigned int readers = 0;

//Эту строку мы шарим
std::string shared_str = "Give me my drink, burtender";

//эта структура просто для того, чтобы удобно было логировать
//работу читателей  и писателей
//у читателей в поле change пустая строка, ибо они ничего не изменяют
//Приоритет у читателей
struct user{
  std::string name;
  std::string change;
};

void init()
{
  pthread_mutex_init(&g_lock, NULL);
  pthread_mutex_init(&read_lock, NULL);
}

void* read_fn(void* arg)
{
  user* current = (user *) arg;  
  //незвестно кто начнет первым читать
  std::random_device rd;
  std::mt19937 gen(rd());
  sleep(gen()%20);
  
  for(int i=0;i<READ_COUNT;++i){
    pthread_mutex_lock(&read_lock);
    readers++;
    if(readers == 1){
      std::cout <<current->name << " try lock mutex\n";
      pthread_mutex_lock(&g_lock);
    }
    pthread_mutex_unlock(&read_lock);
    
    std::cout << current->name << " read: " << shared_str << '\n';
    //делаю вид, что это продолжительная операция
    sleep(READ_TIME);
    
    pthread_mutex_lock(&read_lock);
    readers--;
    if(readers == 0){
      std::cout <<current->name << " unlock mutex\n";
      pthread_mutex_unlock(&g_lock);
    }
    pthread_mutex_unlock(&read_lock);
    //делаю вид, что захочу читать через какое то время
    sleep(READ_TIME);
  }
  //очищаю память 
  delete current;
  return NULL;
}

void* write_fn(void* arg)
{
  user* current = (user *) arg;
  //неизвестно кто начнет первым писать
  std::random_device rd;
  std::mt19937 gen(rd());
  sleep(gen()%20);
  
  for(int i=0;i<WRITE_COUNT;++i){
    
    std::cout <<current->name << " try lock mutex\n";
    pthread_mutex_lock(&g_lock);

    shared_str = current->change;
    std::cout <<current->name << " take shared str and change to"<< shared_str <<'\n';
    //делаю вид, что это продолжительная операция
    sleep(WRITE_TIME);
    std::cout <<current->name << " unlock mutex\n";
    pthread_mutex_unlock(&g_lock);
    //делаю вид, что захочу писать через какое то время
    sleep(WRITE_TIME);
  }
  //очищаю память
  delete current;
  return NULL;
}


int main()
{
  std::string reader_name= "READER_";
  std::string writer_name= "WRITER_";
  std::string change_str= "CHANGE_";
  pthread_t m_thread;
  pthread_t read_thread_pool[READERS_NUMBER];
  pthread_t write_thread_pool[WRITERS_NUMBER];
  
  init();  

  //неизвестно, кто первым захватит g_lock, так как в воркерах
  //идет засыпание на случайное количество секунд
  
  for(int i = 0; i < READERS_NUMBER; ++i){

    user* cur_user = new user;
    cur_user->name = reader_name + std::to_string(i);
    pthread_create(&read_thread_pool[i],NULL,read_fn,cur_user);
  }

  for(int i = 0; i < WRITERS_NUMBER; ++i){
    
    user* cur_user = new user;
    cur_user->name = writer_name + std::to_string(i);
    cur_user->change = change_str + std::to_string(i);
    pthread_create(&write_thread_pool[i],NULL,write_fn,cur_user);
    
  }

  for(int i=0; i<WRITERS_NUMBER; ++i)
    pthread_join(write_thread_pool[i], NULL);

  for(int i=0; i<READERS_NUMBER; ++i)
    pthread_join(read_thread_pool[i], NULL);

  return 0;
}
