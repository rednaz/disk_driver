#ifndef __DISK_H
#define __DISH_H

#define SECT_SIZE 1280
#define NUM_OF_SECTS 128
#define NUM_OF_CYLS 200
#define NUM_OF_HEADS 10

#define MAX_LOGICAL_SECTOR 256000 // NUM_OF_HEADS * NUM_OF_CYLS * NUM_OF_SECTS

#define COUNTDOWN_VALUE 20

typedef char sect[SECT_SIZE];
typedef sect head_t[NUM_OF_SECTS];
typedef head_t cylinder_t[NUM_OF_HEADS];
typedef cylinder_t disk_t[NUM_OF_CYLS];

struct DISK_REGISTER  {
     unsigned ready:1;
     unsigned error_occured:1;
     unsigned disk_spinning:1;
     unsigned write_protect:1;
     unsigned head_loaded:1;
     unsigned error_code:8;
	 unsigned head:5;
	 unsigned cylinder:9;
     unsigned sector:8;
     unsigned command:5;
};

typedef struct
{
  int cyl;
  int head;
  int sect;
} physaddr_t;

/**
This function translates a logical to a physical address.
**/
int log_to_phys(int logaddr/*, physaddr_t *phaddr*/);

/**
This function translates a physical to a logical address.
**/
int phys_to_log(physaddr_t *phaddr);

void start_timer(void);
void stop_timer(void);
void timer_callback(int);

#endif
