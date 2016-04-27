#ifndef INC_traffic_h
#define INC_traffic_h

#include <ross.h>

#define MEAN_DEPARTURE 10
#define MEAN_UTURN 1

typedef enum traffic_event_t traffic_event_t;
typedef enum traffic_direction_t traffic_direction_t;
typedef struct traffic_state traffic_state;
typedef struct traffic_message traffic_message;

enum traffic_event_t {
    ARRIVAL,
    DEPARTURE
};

enum traffic_direction_t {
    NORTH = 0,
    //OUT_NORTH,
    SOUTH,
    //OUT_SOUTH,
    EAST,
    //OUT_EAST,
    WEST
    //OUT_WEST
};

struct traffic_state {
    int num_cars_finished_here;
    int num_cars_arrived_here;
    int waiting_time;

    tw_stime cur_ns_cycle_start;
    tw_stime last_ns_time;
    tw_stime cur_we_cycle_start;
    tw_stime last_we_time;

    int num_cars_in_north;
    //int num_cars_out_north;
    int num_cars_in_south;
    //int num_cars_out_south;
    int num_cars_in_east;
    //int num_cars_out_east;
    int num_cars_in_west;
    //int num_cars_out_west;
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

static int grid_size = 256;
static int opt_mem = 16000;
static int initial_cars_per_intersection = 1;
static int lane_capacity = 4;
static int lane_unit_traversal_time = 1;
static tw_lpid nlp_per_pe = 1024;
static tw_stime lookahead = 0.00000001;
static tw_stime traffic_light_duration = 10.0;
static tw_stime time_car_takes = 1.0;
static tw_stime wait_time_avg = 0.0;

#endif
