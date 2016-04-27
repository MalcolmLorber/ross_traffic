#ifndef INC_traffic_h
#define INC_traffic_h

#include <ross.h>

#define MEAN_DEPARTURE 30.0
#define MEAN_LAND 10.0

typedef enum traffic_event_t traffic_event_t;
typedef enum traffic_direction_t traffic_direction_t;
typedef struct traffic_state traffic_state;
typedef struct traffic_message traffic_message;

enum traffic_event_t {
	ARRIVAL,
	DEPARTURE,
	PATHFINDING
};

enum traffic_direction_t {
	IN_NORTH = 0,
	OUT_NORTH,
	IN_SOUTH,
	OUT_SOUTH,
	IN_EAST,
	OUT_EAST,
	IN_WEST,
	OUT_WEST
};

struct traffic_state {
	int num_cars_finished_here;
	int num_cars_arrived_here;
	int waiting_time;
    
	int num_cars_in_north;
	int num_cars_out_north;
	int num_cars_in_south;
	int num_cars_out_south;
	int num_cars_in_east;
	int num_cars_out_east;
	int num_cars_in_west;
	int num_cars_out_west;
};

typedef struct {
	int x_to_go;
	int y_to_go;
	traffic_direction_t direction;
} car;

struct traffic_message {
	traffic_event_t type;
	car car;
};

static int grid_size = 128;
static tw_stime lookahead = 0.00000001;
static tw_lpid nlp_per_pe = 1024;
static tw_stime mean_flight_time = 1;
static int opt_mem = 1000;
static int cars_per_insection = 1;

static tw_stime wait_time_avg = 0.0;

#endif
