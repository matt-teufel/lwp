#include "lwp.h"
#include "rr_scheduler.h"
#include <stdlib.h>
#include <stdio.h>

thread head = NULL;
thread tail = NULL; /*used for tracking the end node*/
// struct scheduler rr_publish = {NULL, NULL, rr_admit, rr_remove, rr_next};

void rr_admit(thread new);
void rr_remove(thread victim);
thread rr_next();
int count=0;

void rr_admit(thread new){
    count++;
    /*admits a thread and adds to the tail of the head*/
    if (head == NULL){
        /*head has no threads*/
        head = new;
    }
    else{
        /*head has threads in list*/
        tail->sched_one = new;
    }
    new->sched_one = head;
    tail = new;
}


void rr_remove(thread victim){
    count--;
    thread temp;
    if (head == NULL){
        return;
    }
    /*first node removed*/
    if (victim == head){
        if(victim->sched_one != head){
            head = victim->sched_one;
            tail->sched_one = head;
        }else{
            head = NULL;
            tail = NULL;
            }
        return;
        }
    temp = head;
    while(temp->sched_one!= victim){
        temp = temp->sched_one;
    }
    if(temp->sched_one == tail){
        tail = temp;
    }
    temp->sched_one = temp->sched_one->sched_one;
}



thread rr_next(){
    /*selects next thread to be scheduled
    may need to look into*/
    thread temp = head;
    if(head){
        tail=head;
        head = head -> sched_one;
    }
    return temp;
}

