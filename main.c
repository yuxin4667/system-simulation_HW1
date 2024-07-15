#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "rngs.h"
#include "sim.h"

#define FALSE 0
#define TRUE 1
#define ARRIVAL 1
#define DEPARTURE 2

double lambda = 1.0 / 4.0;
double mu = 1.0 / 3.0;
node* eventList = NULL;
node* serverQueue = NULL;
int isServerBusy = FALSE;

//phase1:求平均系統人數
//求平均系統人數 = (客人數 * 時間間隔)的總和 / 總時間 = lambda / (mu - lambda)
//(客人數 * 時間間隔)的總和 = 總面積
/*Customer開始被server服務 -> 排departure事件和departureTime*/
double time = 0.0;  // Master clock
int numberOfCustomers = 0;

//生成exp隨機變數，用來設定arrival和departure的時間
double expRnd(int streamIndex, double rate) {
    SelectStream(streamIndex);
    return -log(1.0 - Random()) / rate;
}
//生成一個ArrivalTime
double RandomArrivalTime() { return expRnd(1, lambda); }

//生成一個ServiceTime(departureTime=MC+ServiceTime)
double RandomServiceTime() { return expRnd(2, mu); }

//一個arrival事件發生時要做甚麼
void arrival(void) {

    // Schedule next arrival
    double nextArrivalTime = RandomArrivalTime() + time;

    //把arrival事件加進FEL中
    node* nextArrivalEvent = createNode(ARRIVAL, nextArrivalTime);
    eventList = insertNode(eventList, nextArrivalEvent);

    //Customer多1人
    numberOfCustomers++;


    //server在忙 -> Customer進入serverQueue等待
    if (isServerBusy) {
        // Add customer to queue
        node* customer = createNode(ARRIVAL, time);
        serverQueue = insertNode(serverQueue, customer);
    }
    //server不忙 -> Customer直接被server服務
    else {
        //然後server就變忙了
        isServerBusy = TRUE;
        //當這個Customer開始被服務時可以排departure事件和departureTime
        double departureTime = RandomServiceTime() + time;
        //把departure事件加進FEL中
        node* departureEvent = createNode(DEPARTURE, departureTime);
        eventList = insertNode(eventList, departureEvent);
    }
}

//一個departure事件發生時要做甚麼
void departure(void) {
    //有人離開 -> 客人少1人
    numberOfCustomers--;

    //如果有customer在serverQueue等
    if (serverQueue != NULL) {
        // Remove customer from queue
        node* customer = NULL;
        serverQueue = removeNode(serverQueue, &customer);
        
        //下一個Customer開始被server服務 -> 排departure事件和departureTime
        double departureTime = RandomServiceTime() + time;
        node* departureEvent = createNode(DEPARTURE, departureTime);
        eventList = insertNode(eventList, departureEvent);
    }
    //如果沒有customer在serverQueue等
    else {
        isServerBusy = FALSE;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("need 2 arg\n");
    }
    else {
        lambda = atof(argv[1]);
        mu = atof(argv[2]);
    }

    int n = 10000000;
    int seed = 1;

    PlantSeeds(seed);

    int numberOfDepartures = 0;
    double areaOfCustomersNumber = 0;
    double timeOfLastEvent = 0.0;
    // Schedule first arrival
    double firstArrivalTime = RandomArrivalTime();
    node* firstArrivalEvent = createNode(ARRIVAL, firstArrivalTime);
    eventList = insertNode(eventList, firstArrivalEvent);

    // Start
    while (numberOfDepartures < n) {

        //現在正在執行的事件
        node* event = NULL;
        eventList = removeNode(eventList, &event);
        time = event->clock;

        //求平均系統人數 = (客人數 * 時間間隔)的總和 / 總時間 = lambda / (mu - lambda)
        //(客人數 * 時間間隔)的總和 = 總面積
        areaOfCustomersNumber += numberOfCustomers * (time - timeOfLastEvent);

        if (event->type == ARRIVAL) {
            arrival();
        }
        else if (event->type == DEPARTURE) {
            departure();
            numberOfDepartures++;
        }
        else {
            printf("Unknown event type\n");
        }
        //記錄總時間
        timeOfLastEvent = time;
    }
    double averagenumberOfCustomers = areaOfCustomersNumber / time;
    double theroicalAverageNumberOfCustomers = lambda / (mu - lambda);
    printf("%lf\t%lf\t%lf\t%lf\t\n", lambda, mu,
        theroicalAverageNumberOfCustomers, averagenumberOfCustomers);

    return 0;
}