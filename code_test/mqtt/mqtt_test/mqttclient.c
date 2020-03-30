/*************************************************************************
	> File Name: mqttclient.c
	> Author: rock
	> Mail: rock_telp@163.com 
	> Created Time: Friday, November 15, 2019 AM11:00:58 HKT
 ************************************************************************/
//编译gcc -o mqttclient mqttclient.c  -lpaho-mqtt3c -I  ./../paho.mqtt.c/src/ -L  ../paho.mqtt.c/build -pthread -Imqtt


#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MQTTClient.h"
#include <unistd.h>
#include <sys/stat.h>


#define NUM_THREADS 2
#define ADDRESS "tcp://192.168.123.172:1883"
#define CLIENTID "ExampleClient_pub"
#define SUB_CLIENTID    "ExampleClient_sub" //更改此处客户端ID
#define TOPICPUB    "Question"  //更改发送的话题
#define TOPICSUB    "temperature"
#define QOS         1
#define TIMEOUT     100000L
#define DISCONNECT  "out"

int CONNECT = 1;
volatile MQTTClient_deliveryToken deliverytoken;
long PAYLOADLEN;
char* PAYLOAD;
char data[10];

void delivered(void *context, MQTTClient_deliveryToken dt)
{
  printf("消息 token  %d 确认发送\n", dt);
  deliverytoken = dt;
}

int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
  int i;
  char* payloadptr;

  printf("消息到达\n");
  printf("    主题: %s\n", topicName);
  printf("  消息: \n");

  payloadptr = message->payload;
  if (strcmp(payloadptr, DISCONNECT) == 0) {
    printf("\n out!!  ");
    CONNECT = 0;
  }

  for (i = 0; i < message->payloadlen; i++) {
    putchar(*payloadptr++);
  }
  printf("\n\n");
	
  printf("接受的消息：\n id=%s version=%d len=%d load=%s qos=%d dup=%d gid=%d \n",
	message->struct_id,message->struct_version,message->payloadlen,(char *)message->payload,
	message->qos,message->retained,message->dup,message->msgid);
  printf("payloadptr=%s \n\n",payloadptr);
	
	//strncpy(dest, src, 10);
	strncpy(data,message->payload,message->payloadlen);
	printf("消息: %s\n\n",data);


  MQTTClient_freeMessage(&message);
  MQTTClient_free(topicName);
  return 1;
}

void connlost(void *context, char *cause)
{
  printf("\n连接中\n");
  printf("     原因: %s\n", cause);
}


//发布消息
void *pubClient(void *threadid) {
  long tid;
  tid = (long)threadid;
  int count = 0;
  printf("pub线程  #%ld!\n", tid);
  //声明一个MQTTClient
  MQTTClient client;
  //初始化MQTT Client选项
  MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
  //#define MQTTClient_message_initializer { {'M', 'Q', 'T', 'M'}, 0, 0, NULL, 0, 0, 0, 0 }
  MQTTClient_message pubmsg = MQTTClient_message_initializer;
  //声明消息token
  MQTTClient_deliveryToken token;
  int rc;
  //使用参数创建一个client，并将其赋值给之前声明的client
  MQTTClient_create(&client, ADDRESS, CLIENTID,
                    MQTTCLIENT_PERSISTENCE_NONE, NULL);
  conn_opts.keepAliveInterval = 20;
  conn_opts.cleansession = 1;
  //使用MQTTClient_connect将client连接到服务器，使用指定的连接选项。成功则返回MQTTCLIENT_SUCCESS
  if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
  {
    printf("连接失败 返回值 %d\n", rc);
    exit(EXIT_FAILURE);
  }
  PAYLOAD = "What's the temperature";
  // printf("%s\n", PAYLOAD);
  pubmsg.payload = PAYLOAD;
  pubmsg.payloadlen = (int)strlen(PAYLOAD);
  pubmsg.qos = QOS;
  pubmsg.retained = 0;
  //循环发布
  while (CONNECT) {
    MQTTClient_publishMessage(client, TOPICPUB, &pubmsg, &token);
    printf("等待 %d 秒 发布:%s\n"
             "发布:%s 客户端ID: %s\n",
             (int)(TIMEOUT/1000), PAYLOAD, TOPICPUB, CLIENTID);
    rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
    printf("消息token %d 传递\n", token);
    // thread sleep
    usleep(2000000L);//接受延时时间
  }

  MQTTClient_disconnect(client, 10000);
  MQTTClient_destroy(&client);
}

//订阅消息
void *subClient(void *threadid) {
  long tid;
  tid = (long)threadid;
  printf("sub线程 #%ld!\n", tid);

  MQTTClient client;
  MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
  int rc;
  int ch;

  MQTTClient_create(&client, ADDRESS, SUB_CLIENTID,
                    MQTTCLIENT_PERSISTENCE_NONE, NULL);
  conn_opts.keepAliveInterval = 20;
  conn_opts.cleansession = 1;
  //设置回调函数
  MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered);

  if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
  {
    printf("连接失败 返回值 %d\n", rc);
    exit(EXIT_FAILURE);
  }
  printf("订阅是主题 %s\n 客户端 %s  QoS=%d\n\n"
         "按 《回车》确认\n\n", TOPICSUB, SUB_CLIENTID, QOS);
  MQTTClient_subscribe(client, TOPICSUB, QOS);

  do
  {
    ch = getchar();
  } while (ch != 'Q' && ch != 'q');
  //quit
  MQTTClient_unsubscribe(client, TOPICSUB);
  MQTTClient_disconnect(client, 10000);
  MQTTClient_destroy(&client);

  pthread_exit(NULL);
}

int main(int argc, char* argv[])
{
  pthread_t threads[NUM_THREADS];
  pthread_create(&threads[0], NULL, subClient, (void *)0);
  pthread_create(&threads[1], NULL, pubClient, (void *)1);
  pthread_exit(NULL);
}
