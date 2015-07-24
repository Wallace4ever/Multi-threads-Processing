#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include<semaphore.h>

#define NUM 1000000
#define N 2  //开始的时候想用for循环控制两个reader进程 但是还有问题 暂时保留
#define top (0)
long long int sum =0,sumread1=-1,sumread2=-1;
int number01=0,number02=0;
//int flag=0;  //原计划作为数组置底的标记  暂时没有实现

int buffer[NUM+2];  // 创建共享缓冲区 数组 在最后设置两个位置是留给标记的
int read_i,write_i; // 数组中标记读写的位置
sem_t sem_read;     // 同步信号量， 当满了时阻止生产者放产品
sem_t sem_write;    // 同步信号量， 当没产品时阻止消费者消费
pthread_mutex_t mutex;  // 互斥信号量， 一次只有一个线程访问缓冲

/* 打印缓冲情况 */
void print()
{

int i;
for(i = 0; i < NUM; i++)
   printf("%d ", buffer[i]);
printf("\n");

}//但是目前来看没什么效果

/*生产者写入数据*/
void * writer()
{

  int n;
  for (n = 1; n <NUM+3; n++)
{

  sem_wait(&sem_write);
  pthread_mutex_lock(&mutex);
  if(n>NUM) buffer[write_i] = 0;
  else {
buffer[write_i] = n;
  printf("writer %d\n", buffer[write_i]);
  sum += 1;

}write_i++;
  pthread_mutex_unlock(&mutex);
  sem_post(&sem_read);


}
  return NULL;

}

/*消费者读取数据 （线程1）*/
void *reader1()
{


  while (1)
{

  sem_wait(&sem_read);
  pthread_mutex_lock(&mutex);
  number01 = buffer[read_i];
  read_i++;
  pthread_mutex_unlock(&mutex);
  sem_post(&sem_write);
  sumread1 += 1;
  if (number01 == 0) break;    //判断数组是否已置底
  printf("reader1 %d\n", number01);

}
return NULL;

}

/*消费者读取数据 （线程2）*/
void *reader2()
{

  while (1)
{

  sem_wait(&sem_read);
  pthread_mutex_lock(&mutex);
  number02 = buffer[read_i];
  read_i++;
  pthread_mutex_unlock(&mutex);
  sem_post(&sem_write);
  sumread2 += 1;
  if (number02 == 0) break;    //判断数组是否已置底

  printf("reader2 %d\n", number02);

}
return NULL;

}


int main(void)
{

  pthread_t th_write, th_reader1,th_reader2;
  void * retval;
  int i,readers;
  sem_init(&sem_write, 0, NUM );      //初始化同步信号量
  sem_init(&sem_read, 0, 0);          //初始化同步信号量
  pthread_mutex_init(&mutex, NULL);   //初始化互斥信号量
  read_i = 0;                         //初始化消费者数组指针（其实就是数组中的标记）
  write_i = 0;                        //初始化生产者数组指针（同上）


//创建1个生产者和2个消费者线程
  pthread_create(&th_write, NULL, writer, NULL);
  pthread_create(&th_reader1, NULL, reader1, NULL);
  pthread_create(&th_reader2, NULL, reader2, NULL);


//销毁所有线程
  (void)pthread_join(th_write, &retval);
  (void)pthread_join(th_reader1, &retval);
  (void)pthread_join(th_reader2, &retval);

//输出写入总量 与 取出总量 （因为设为了全局变量 不受线程销毁的影响）
  printf(" writer sum is %lld \n", sum);
  printf("reader1 sum is %lld \n", sumread1);
  printf("reader2 sum is %lld \n", sumread2);


return 0;

}
