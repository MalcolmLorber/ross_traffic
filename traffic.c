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
    s->num_cars_out_north = 0;
    s->num_cars_in_south = 0;
    s->num_cars_out_south = 0;
    s->num_cars_in_east = 0;
    s->num_cars_out_east = 0;
    s->num_cars_in_west = 0;
    s->num_cars_out_west = 0;

    //seed initial events
    for (i = 0; i < initial_cars_per_intersection; i++) {
        e = tw_event_new(lp->gid, tw_rand_exponential(lp->rng, MEAN_DEPARTURE), lp);
        m = tw_event_data(e);
        m->type = ARRIVAL;
        m->car.x_to_go = tw_rand_exponential(lp->rng, grid_size);
        m->car.y_to_go = tw_rand_exponential(lp->rng, grid_size);
        m->car.direction = (rand() % 4) * 2;
        tw_event_send(e);
    }
}

tw_stime update_next_available_departure(traffic_state *s, traffic_direction_t dir, tw_lp *lp)
{
    if(dir == NORTH || dir == SOUTH){
        if(tw_now(lp) > (s->last_ns_time+time_car_takes)){
            if(tw_now(lp) > (s->cur_ns_cycle_start + traffic_light_duration)){
                s->cur_ns_cycle_start = (2 * traffic_light_duration) * 
                    ceil((tw_now(lp) - s->cur_ns_cycle_start)/(2 * light_time));
            }
            s->last_ns_time = ROSS_MAX(tw_now(lp),s->cur_ns_cycle_start);
        }
        else{
            if(s->last_ns_time + time_car_takes <= cur_ns_cycle_start + traffic_light_duration){
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
                    ceil((tw_now(lp) - s->cur_we_cycle_start)/(2 * light_time));
            }
            s->last_we_time = ROSS_MAX(tw_now(lp),s->cur_we_cycle_start);
        }
        else{
            if(s->last_we_time + time_car_takes <= cur_we_cycle_start + traffic_light_duration){
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
        // Schedule a landing in the future
        //msg->saved_furthest_flight_landing = s->furthest_flight_landing;
        //s->furthest_flight_landing = ROSS_MAX(s->furthest_flight_landing, tw_now(lp));
        ts = tw_rand_exponential(lp->rng, MEAN_LAND);
        ///////
        e = tw_event_new(lp->gid, ts + s->furthest_flight_landing - tw_now(lp), lp);
        m = tw_event_data(e);
        m->type = LAND;
        ///////
        m->waiting_time = s->furthest_flight_landing - tw_now(lp);
        s->furthest_flight_landing += ts;
        tw_event_send(e);
        break;
    case DEPARTURE:
        s->planes_on_the_ground--;
        ts = tw_rand_exponential(lp->rng, mean_flight_time);
        rand_result = tw_rand_integer(lp->rng, 0, 3);
        dst_lp = 0;
        switch (rand_result) {
        case 0:
            // Fly north
            if (lp->gid < 32)
                // Wrap around
                dst_lp = lp->gid + 31 * 32;
            else
                dst_lp = lp->gid - 31;
            break;
        case 1:
            // Fly south
            if (lp->gid >= 31 * 32)
                // Wrap around
                dst_lp = lp->gid - 31 * 32;
            else
                dst_lp = lp->gid + 31;
            break;
        case 2:
            // Fly east
            if ((lp->gid % 32) == 31)
                // Wrap around
                dst_lp = lp->gid - 31;
            else
                dst_lp = lp->gid + 1;
            break;
        case 3:
            // Fly west
            if ((lp->gid % 32) == 0)
                // Wrap around
                dst_lp = lp->gid + 31;
            else
                dst_lp = lp->gid - 1;
            break;
        }
        e = tw_event_new(dst_lp, ts, lp);
        m = tw_event_data(e);
        m->type = ARRIVAL;
        tw_event_send(e);
        break;
    }
}

void rc_event_handler(traffic_state * s, tw_bf * bf, traffic_message * msg, tw_lp * lp)
{
    switch (msg->type) {
    case ARRIVAL:
        s->furthest_flight_landing = msg->saved_furthest_flight_landing;
        tw_rand_reverse_unif(lp->rng);
        break;
    case DEPARTURE:
        tw_rand_reverse_unif(lp->rng);
        tw_rand_reverse_unif(lp->rng);
        break;
    case LAND:
        s->landings--;
        s->waiting_time -= msg->waiting_time;
        tw_rand_reverse_unif(lp->rng);
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

    //Ask the professor whats up
    nlp_per_pe /= (tw_nnodes() * g_tw_npe);
    g_tw_events_per_pe = (cars_per_insection * nlp_per_pe / g_tw_npe) + opt_mem;

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
