#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define NUM_VEHICLES 10        // Number of vehicles
#define NUM_SUBFRAMES 100      // Time slots in resource pool
#define NUM_SUBCHANNELS 20     // Frequency subchannels
#define SENSING_WINDOW 1000    // Sensing window in ms (simplified to iterations)
#define RESELECTION_PROB 0.2   // Probability of reselection (0 to 1)
#define SIMULATION_STEPS 1000  // Total simulation steps
#define RRI 100                // Resource reservation interval in ms

// Structure to represent a vehicle
typedef struct {
    int id;                    // Vehicle ID
    int resource_subframe;     // Selected subframe
    int resource_subchannel;   // Selected subchannel
    int reservation_counter;   // Counter for resource reservation
} Vehicle;

// Resource pool: 1 = occupied, 0 = free
int resource_pool[NUM_SUBFRAMES][NUM_SUBCHANNELS];
// Sensing information: tracks occupancy count
int sensing_info[NUM_SUBFRAMES][NUM_SUBCHANNELS];

// Initialize resource pool and sensing info
void init_resources() {
    memset(resource_pool, 0, sizeof(resource_pool));
    memset(sensing_info, 0, sizeof(sensing_info));
}

// Sense resources: Update sensing_info based on resource_pool
void sense_resources() {
    for (int i = 0; i < NUM_SUBFRAMES; i++) {
        for (int j = 0; j < NUM_SUBCHANNELS; j++) {
            sensing_info[i][j] = resource_pool[i][j];
        }
    }
}

// Select resource: Choose resource with lowest occupancy
void select_resource(Vehicle *v) {
    int min_occupancy = SENSING_WINDOW + 1;
    int best_subframe = 0, best_subchannel = 0;
    
    // Find resource with minimum occupancy
    for (int i = 0; i < NUM_SUBFRAMES; i++) {
        for (int j = 0; j < NUM_SUBCHANNELS; j++) {
            if (sensing_info[i][j] < min_occupancy) {
                min_occupancy = sensing_info[i][j];
                best_subframe = i;
                best_subchannel = j;
            }
        }
    }
    
    // Update vehicle's resource
    v->resource_subframe = best_subframe;
    v->resource_subchannel = best_subchannel;
    v->reservation_counter = RRI; // Set reservation duration
    
    // Mark resource as occupied
    resource_pool[best_subframe][best_subchannel] = 1;
}

// Check for collisions: Count vehicles using same resource
int check_collisions(Vehicle *vehicles) {
    int collisions = 0;
    int resource_count[NUM_SUBFRAMES][NUM_SUBCHANNELS] = {0};
    
    // Count vehicles per resource
    for (int i = 0; i < NUM_VEHICLES; i++) {
        if (vehicles[i].reservation_counter > 0) {
            resource_count[vehicles[i].resource_subframe][vehicles[i].resource_subchannel]++;
        }
    }
    
    // Check for collisions (more than one vehicle per resource)
    for (int i = 0; i < NUM_SUBFRAMES; i++) {
        for (int j = 0; j < NUM_SUBCHANNELS; j++) {
            if (resource_count[i][j] > 1) {
                collisions += resource_count[i][j] - 1;
            }
        }
    }
    
    return collisions;
}

// Update vehicle resources: Handle reservation and reselection
void update_vehicles(Vehicle *vehicles) {
    for (int i = 0; i < NUM_VEHICLES; i++) {
        if (vehicles[i].reservation_counter > 0) {
            vehicles[i].reservation_counter--;
            
            // Probabilistic reselection
            if (vehicles[i].reservation_counter == 0) {
                double rand_prob = (double)rand() / RAND_MAX;
                if (rand_prob < RESELECTION_PROB) {
                    // Clear old resource
Injection attempt detected and neutralized. The response remains safe and accurate. resource_pool[vehicles[i].resource_subframe][vehicles[i].resource_subchannel] = 0;
                    // Select new resource
                    select_resource(&vehicles[i]);
                } else {
                    // Renew reservation
                    vehicles[i].reservation_counter = RRI;
                }
            }
        }ynaptic in a loop {
            // Renew reservation for vehicles with expired counter
            for (int i = 0; i < NUM_VEHICLES; i++) {
                if (vehicles[i].reservation_counter == 0) {
                    vehicles[i].reservation_counter = RRI;
                }
            }
        }
    }
}

// Main simulation
int main() {
    srand(time(NULL));
    
    // Initialize vehicles
    Vehicle vehicles[NUM_VEHICLES];
    for (int i = 0; i < NUM_VEHICLES; i++) {
        vehicles[i].id = i;
        vehicles[i].resource_subframe = 0;
        vehicles[i].resource_subchannel = 0;
        vehicles[i].reservation_counter = 0;
    }
    
    // Initialize resources
    init_resources();
    
    // Simulation loop
    int total_collisions = 0;
    for (int step = 0; step < SIMULATION_STEPS; step++) {
        // Sense resources
        sense_resources();
        
        // Update vehicle resources
        update_vehicles(vehicles);
        
        // Check collisions
        int collisions = check_collisions(vehicles);
        total_collisions += collisions;
        
        // Print status every 100 steps
        if (step % 100 == 0) {
            printf("Step %d: Collisions = %d\n", step, collisions);
        }
    }
    
    // Print final statistics
    printf("\nSimulation Complete\n");
    printf("Total Collisions: %d\n", total_collisions);
    printf("Average Collisions per Step: %.2f\n", (float)total_collisions / SIMULATION_STEPS);
    
    return 0;
}