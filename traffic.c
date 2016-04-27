#include "traffic.h"

tw_peid mapping(tw_lpid gid)
{
    return (tw_peid)gid / g_tw_nlp;
}

void init(traffic_state * s, tw_lp * lp)
{
    int i;
    tw_event *e;
    traffic_message *m;

    //init state struct variables
    s->num_cars_finished_here = 0;
    s->num_cars_arrived_here = 0;
    s->waiting_time = 0;
    s->num_cars_in_north = 0;
    //s->num_cars_out_north = 0;
    s->num_cars_in_south = 0;
    //s->num_cars_out_south = 0;
    s->num_cars_in_east = 0;
    //s->num_cars_out_east = 0;
    s->num_cars_in_west = 0;
    //s->num_cars_out_west = 0;

    //seed initial events
    for (i = 0; i < initial_cars_per_intersection; i++) {
        e = tw_event_new(lp->gid, tw_rand_exponential(lp->rng, MEAN_DEPARTURE), lp);
        m = tw_event_data(e);
        m->type = ARRIVAL;
        m->car.x_to_go = tw_rand_exponential(lp->rng, grid_size);
        m->car.y_to_go = tw_rand_exponential(lp->rng, grid_size);
        m->car.direction = rand() % 4;
        tw_event_send(e);
    }
}

traffic_direction_t change_dir(traffic_direction_t cur_dir) {
    switch(cur_dir){
    case SOUTH:
        return NORTH;
    case WEST:
        return EAST;
    case NORTH:
        return SOUTH;
    case EAST:
        return WEST;
    }
}

traffic_direction_t find_path(traffic_message * msg) {
    if(msg->car.x_to_go < 0){
        msg->car.x_to_go++;
        return WEST;
    } else if (msg->car.x_to_go > 0) {
        msg->car.x_to_go--;
        return EAST;
    } else {
        if(msg->car.y_to_go < 0) {
            msg->car.y_to_go++;
            return NORTH;
        } else if(msg->car.y_to_go > 0){
            msg->car.y_to_go--;
            return SOUTH;
        } else {
            return -1;
        }
    }
}

tw_lpid resolve_neighbor(traffic_direction_t dir, tw_lp * lp){
    switch(dir) {
    case NORTH:
        if (lp->gid < grid_size)
            return lp->gid + (grid_size - 1) * grid_size;
        else
            return lp->gid - (grid_size - 1);
    case EAST:
        if ((lp->gid % grid_size) == (grid_size - 1))
            return lp->gid - (grid_size - 1);
        else
            return lp->gid + 1;
    case SOUTH:
        if (lp->gid >= (grid_size - 1) * grid_size)
            return lp->gid - (grid_size - 1) * grid_size;
        else
            return lp->gid + (grid_size - 1);
    case WEST:
        if ((lp->gid % grid_size) == 0)
            return lp->gid + (grid_size - 1);
        else
            return lp->gid - 1;
    }
}

tw_stime calculate_traversal_time(){
    return lane_capacity * lane_unit_traversal_time;
}

tw_stime update_next_available_departure(traffic_state *s, traffic_direction_t dir, tw_lp *lp)
{
    if(dir == NORTH || dir == SOUTH){
        if(tw_now(lp) > (s->last_ns_time+time_car_takes)){
            if(tw_now(lp) > (s->cur_ns_cycle_start + traffic_light_duration)){
                s->cur_ns_cycle_start = (2 * traffic_light_duration) *
                    ceil((tw_now(lp) - s->cur_ns_cycle_start)/(2 * traffic_light_duration));
            }
            s->last_ns_time = ROSS_MAX(tw_now(lp),s->cur_ns_cycle_start);
        }
        else{
            if(s->last_ns_time + time_car_takes <= s->cur_ns_cycle_start + traffic_light_duration){
                s->last_ns_time += time_car_takes;
            }
            else{
                s->last_ns_time = s->cur_ns_cycle_start + (2 * traffic_light_duration);
                s->cur_ns_cycle_start += 2 * traffic_light_duration;
            }
        }
        return s->last_ns_time;
    }
    else{
        if(tw_now(lp) > (s->last_we_time+time_car_takes)){
            if(tw_now(lp) > (s->cur_we_cycle_start + traffic_light_duration)){
                s->cur_we_cycle_start = (2 * traffic_light_duration) *
                    ceil((tw_now(lp) - s->cur_we_cycle_start)/(2 * traffic_light_duration));
            }
            s->last_we_time = ROSS_MAX(tw_now(lp),s->cur_we_cycle_start);
        }
        else{
            if(s->last_we_time + time_car_takes <= s->cur_we_cycle_start + traffic_light_duration){
                s->last_we_time += time_car_takes;
            }
            else{
                s->last_we_time = s->cur_we_cycle_start + (2 * traffic_light_duration);
                s->cur_we_cycle_start += 2 * traffic_light_duration;
            }
        }
        return s->last_we_time;
    }
}

void event_handler(traffic_state * s, tw_bf * bf, traffic_message * msg, tw_lp * lp)
{
    int rand_result;
    tw_lpid dst_lp;
    tw_stime ts;
    tw_event *e;
    traffic_message *m;

    switch (msg->type) {
    case ARRIVAL:
        s->num_cars_arrived_here++;
        int lane_full = 0;
        switch(msg->car.direction){
        case NORTH:
            if(s->num_cars_in_south >= lane_capacity){
                lane_full = 1;
            } else {
                if(msg->car.x_to_go == 0 && msg->car.y_to_go == 0){
                    s->num_cars_finished_here++;
                } else {
                    s->num_cars_in_south++;
                }
            }
            break;
        case EAST:
            if(s->num_cars_in_west >= lane_capacity){
                lane_full = 1;
            } else {
                if(msg->car.x_to_go == 0 && msg->car.y_to_go == 0){
                    s->num_cars_finished_here++;
                } else {
                    s->num_cars_in_west++;
                }
            }
            break;
        case SOUTH:
            if(s->num_cars_in_north >= lane_capacity){
                lane_full = 1;
            } else {
                if(msg->car.x_to_go == 0 && msg->car.y_to_go == 0){
                    s->num_cars_finished_here++;
                } else {
                    s->num_cars_in_north++;
                }
            }
            break;
        case WEST:
            if(s->num_cars_in_east >= lane_capacity){
                lane_full = 1;
            } else {
                if(msg->car.x_to_go == 0 && msg->car.y_to_go == 0){
                    s->num_cars_finished_here++;
                } else {
                    s->num_cars_in_east++;
                }
            }
            break;
        }
        if(lane_full == 1){
            traffic_direction_t new_dir = change_dir(msg->car.direction);
            tw_lpid dest_lp = resolve_neighbor(new_dir, lp);
            ts = calculate_traversal_time();
            e = tw_event_new(dest_lp, ts, lp);
            m = tw_event_data(e);
            m->type = ARRIVAL;
            m->car = msg->car;
            m->car.direction = new_dir;
            tw_event_send(e);
        } else {
            if(msg->car.x_to_go != 0 || msg->car.y_to_go != 0){
                ts = update_next_available_departure(s, msg->car.direction, lp);
                e = tw_event_new(lp->gid, ts, lp);
                m = tw_event_data(e);
                m->type = DEPARTURE;
                m->car = msg->car;
                tw_event_send(e);
            }
        }
        break;
    case DEPARTURE: ;
        traffic_direction_t dir = find_path(msg);
        tw_lpid dest_lp = resolve_neighbor(dir, lp);
        ts = calculate_traversal_time();
        e = tw_event_new(dest_lp, ts, lp);
        m = tw_event_data(e);
        m->type = ARRIVAL;
        m->car = msg->car;
        m->car.direction = dir;
        tw_event_send(e);
    }
}

void rc_event_handler(traffic_state * s, tw_bf * bf, traffic_message * msg, tw_lp * lp)
{
    switch (msg->type) {
    case ARRIVAL:
        s->num_cars_arrived_here--;
        int lane_full = 0;
        switch(msg->car.direction){
        case NORTH:
            if(s->num_cars_in_south >= lane_capacity){
                lane_full = 1;
            } else {
                if(msg->car.x_to_go == 0 && msg->car.y_to_go == 0){
                    s->num_cars_finished_here--;
                } else {
                    s->num_cars_in_south--;
                }
            }
            break;
        case EAST:
            if(s->num_cars_in_west >= lane_capacity){
                lane_full = 1;
            } else {
                if(msg->car.x_to_go == 0 && msg->car.y_to_go == 0){
                    s->num_cars_finished_here--;
                } else {
                    s->num_cars_in_west--;
                }
            }
            break;
        case SOUTH:
            if(s->num_cars_in_north >= lane_capacity){
                lane_full = 1;
            } else {
                if(msg->car.x_to_go == 0 && msg->car.y_to_go == 0){
                    s->num_cars_finished_here--;
                } else {
                    s->num_cars_in_north--;
                }
            }
            break;
        case WEST:
            if(s->num_cars_in_east >= lane_capacity){
                lane_full = 1;
            } else {
                if(msg->car.x_to_go == 0 && msg->car.y_to_go == 0){
                    s->num_cars_finished_here--;
                } else {
                    s->num_cars_in_east--;
                }
            }
            break;
        }
        break;
    case DEPARTURE:
        break;
    }
}

void final(traffic_state * s, tw_lp * lp)
{
    wait_time_avg += ((s->waiting_time / (double)s->num_cars_arrived_here) / nlp_per_pe);
}

tw_lptype traffic_lps[] =
{
    {
        (init_f)init,
        (pre_run_f)NULL,
        (event_f)event_handler,
        (revent_f)rc_event_handler,
        (final_f)final,
        (map_f)mapping,
        sizeof(traffic_state),
    },
    { 0 },
};

const tw_optdef app_opt [] =
{
    TWOPT_GROUP("Traffic Model"),
    TWOPT_UINT("grid_size",      grid_size,  "size of traffic grid"),
    TWOPT_UINT("memory",         opt_mem,                    "optimistic memory"),
    TWOPT_UINT("initial_cars_per_intersection",          initial_cars_per_intersection,      "the number of initially seeded cars per intersection"),
    TWOPT_UINT("lane_capacity",          lane_capacity,      "the number of cars that can be in any given lane for an intersection"),
    TWOPT_UINT("memory",         opt_mem,                    "optimistic memory"),
    TWOPT_STIME("lookahead",     lookahead,                  "lookahead for events"),
    TWOPT_STIME("traffic_light_duration",     traffic_light_duration,                        "Time for a given traffic light cycle"),
    TWOPT_STIME("time_car_takes",     time_car_takes,                        "the time the car takes to go through the light"),
    TWOPT_END()
};

int main(int argc, char **argv, char **env)
{
    //opt-mem should be around 16k
    int i;

    tw_opt_add(app_opt);
    tw_init(&argc, &argv);

    nlp_per_pe = grid_size * grid_size;

    //Ask the professor whats up
    nlp_per_pe /= (tw_nnodes() * g_tw_npe);
    g_tw_events_per_pe = (initial_cars_per_intersection * nlp_per_pe / g_tw_npe) + opt_mem;

    g_tw_lookahead = lookahead;

    //tw_lpid nlp_per_pe = grid_size/g_tw_npe;
    tw_define_lps(nlp_per_pe, sizeof(traffic_message));

    for (i = 0; i < g_tw_nlp; i++)
        tw_lp_settype(i, &traffic_lps[0]);

    tw_run();

    if (tw_ismaster()) {
        printf("Traffic Model Statistics:\n");
        //Print out stats aggregation
    }

    tw_end();

    return 0;
}
