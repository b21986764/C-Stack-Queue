//
//  main.cpp
//  Ass3
//
//  Created by Sertaç Güler on 29.11.2022.
//

#include <iostream>
#include <fstream>
#include <vector>

using namespace std;
typedef enum {Customer, Cashier, Barista} Peo;
//Peoople struct parent class
struct People{
    Peo peo;
    People();
    People(Peo peo){
        this->peo = peo;
    }
};
//Customer struct inherits from people
struct Cust:People{
    Cust* next;
    double arrivetime;
    double ordertime;
    double maketime;
    double price;
    double done;
    Cust(double arr, double ord, double mak, double prc){
        arrivetime = arr;
        ordertime = ord;
        maketime = mak;
        price = prc;
        next = NULL;
    }
    void setdone(double newdone){
        done = newdone;
    }
    double turnaround(){
        return done - arrivetime;
    }
};
//Cashier is a people
struct Cashier:People{
    Cust* cust;
    
    int casid;
    double timespent;
    Cashier(int id){
        
        casid = id;
        timespent = 0;
    }
    void newCust(Cust* c){
        cust = c;
        timespent = timespent + c->ordertime;
    }
};
//Barista is another people
struct Barista{
    Cust* cust;
    
    int barid;
    double timespent;
    Barista(int id){
        
        barid = id;
        timespent = 0;
    }
    void newCust(Cust* c){
        cust = c;
        timespent = timespent + c->maketime;
    }
};
//Simulation to store events
struct Simulation{
    Simulation* next;
    People* people;
    double timeoccur;
    Simulation(People* people, double time1){
        this->people = people;
        timeoccur = time1;
    }
};
//Simulation List to create a priority queue
struct SimulationList{
    Simulation* front;
    Simulation* rear;
    double istime;
    SimulationList(){
        front = rear = NULL;
        istime = 0;
    }
    void enqueue(double time1, People* people){
        Simulation* temp;
        Simulation* prev;
        temp = new Simulation(people, time1+istime);
        
        if (front == NULL || temp->timeoccur < front->timeoccur)
        {
            temp->next = front;
            front = rear = temp;
            
        }
        else
        {
            prev = front;
            while (prev->next != NULL && prev->next->timeoccur < temp->timeoccur)
                prev = prev->next;
            
            temp->next = prev->next;
            prev->next = temp;
        }
    }
    
    Simulation* dequeue(){
        
        if(front == NULL){
            return NULL;
        }
        istime = front->timeoccur;
        Simulation* temp = front;
        front = front->next;
        if(front == NULL){
            rear = NULL;
        }
        
        return temp;
    }
    bool isEmpty(){
        return (front == NULL) && (rear == NULL);
    }
};
//Cashier queue determines the queue of cashier waiting customers
struct CashierQueue{
    
    int size;
    Cust* front;
    Cust* rear;
    int grelen;
    CashierQueue(){
        front = rear = NULL;
        size = 0;
        grelen = 0;
    }
    
    void enqueue(Cust* c){
        Cust* temp = new Cust(c->arrivetime, c->ordertime, c->maketime, c->price);
        
        if(rear == NULL){
            front = rear = temp;
            return;
        }
        rear->next = temp;
        rear = temp;
        this->size += 1;
    }
    Cust* dequeue(){
        if(front == NULL){
            return NULL;
        }
        Cust* temp = front;
        front = front->next;
        this->size -=1;
        if(front == NULL){
            rear = NULL;
        }
        
        return temp;
    }
    bool isEmpty(){
        return (front == NULL) && (rear == NULL);
    }
    void makegrelen(){
        int temp = this->size;
        this->grelen = max(temp,grelen);
    }
    
};
//BaristaQueue determines orders waiting for a barista to execute
struct BaristaQueue{
    Cust* front;
    Cust* rear;
    
    int size;
    int grelen;
    BaristaQueue(){
        front = rear = NULL;
        size = 0;
        grelen = 0;
    }
    void enqueue(Cust* c){
        Cust* temp;
        Cust* prev;
        temp = new Cust(c->arrivetime, c->ordertime, c->maketime, c->price);
        
        if (front == NULL || c->price > front->price)
        {
            temp->next = front;
            front = rear = temp;
            
        }
        else
        {
            prev = front;
            while (prev->next != NULL && prev->next->price > c->price)
                prev = prev->next;
            
            temp->next = prev->next;
            prev->next = temp;
        }
    }
    Cust* dequeue(){
        if(front == NULL){
            return NULL;
        }
        Cust* temp = front;
        front = front->next;
        this->size -=1;
        if(front == NULL){
            rear = NULL;
        }
        
        return temp;
    }
    bool isEmpty(){
        return (front == NULL) && (rear == NULL);
    }
    void makegrelen(){
        int temp = this->size;
        this->grelen = max(temp,grelen);
    }
    
};
//CheckId list is a priority queue to decide which empty employe is going to take the order
struct checkId{
    checkId* next;
    int idno;
    checkId(int idno){
        this->idno = idno;
        next = NULL;
    }
};
struct checkIdList{
    checkId* front;
    checkId* rear;
    checkIdList(){
        front = rear = NULL;
    }
    void enqueue(int id){
        checkId* temp;
        checkId* prev;
        temp = new checkId(id);
        
        if (front == NULL || temp->idno < front->idno)
        {
            temp->next = front;
            front = rear = temp;
            
        }
        else
        {
            prev = front;
            while (prev->next != NULL && prev->next->idno < temp->idno)
                prev = prev->next;
            
            temp->next = prev->next;
            prev->next = temp;
        }
    }
    checkId* dequeue(){
        if(front == NULL){
            return NULL;
        }
        checkId* temp = front;
        front = front->next;
        
        if(front == NULL){
            rear = NULL;
        }
        
        return temp;
    }
    bool isEmpty(){
        return (front == NULL) && (rear == NULL);
    }
    
};


int main(int argc, const char * argv[]) {
    ifstream infile(argv[1]);
    FILE *fout = fopen(argv[2], "w");
    double arr, ord, maketime, prc;
    vector<Cust *> custs;

    int i, j;
    infile >> i >> j;
    
    fprintf(fout, "%d\n", -1);
    fprintf(fout, "%d\n", -1);
    fprintf(fout, "%d\n", -1);
    for (int m = 0; m < i; ++m) {
        fprintf(fout, "%d\n", -1);
      }
    for (int m = 0; m < i/3; ++m) {
        fprintf(fout, "%d\n", -1);
      }
    for (int m = 0; m < j; ++m) {
        fprintf(fout, "%d\n", -1);
      }
    fprintf(fout, "\n");
    fprintf(fout, "%d\n", -1);
    fprintf(fout, "%d\n", -1);
    for (int m = 0; m < i/3; ++m) {
        fprintf(fout, "%d\n", -1);
      }
    for (int m = 0; m < i; ++m) {
        fprintf(fout, "%d\n", -1);
      }
    
    for (int m = 0; m < i/3; ++m) {
        fprintf(fout, "%d\n", -1);
      }
    for (int m = 0; m < j; ++m) {
        fprintf(fout, "%d\n", -1);
      }
    return 0;
}
