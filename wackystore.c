#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define MAX_NAME_LENGTH 1024

typedef struct ItemNode ItemNode;
struct ItemNode {
    char name[MAX_NAME_LENGTH];
    int count;
    ItemNode* next;
};

typedef struct Customer Customer;
struct Customer {
    char name[MAX_NAME_LENGTH];
    ItemNode* cart;
};

typedef struct CheckoutLaneNode CheckoutLaneNode;
struct CheckoutLaneNode {
    Customer* customer;

    CheckoutLaneNode* front;
    CheckoutLaneNode* back;
};

typedef struct CheckoutLane CheckoutLane;
struct CheckoutLane {
    CheckoutLaneNode* first;
    CheckoutLaneNode* last;
};

ItemNode* new_item_node(char* name, int count) {

    ItemNode *new_item = (ItemNode *)calloc(1, sizeof(ItemNode));

    strcpy(new_item->name, name);
    new_item->count = count;
    new_item->next = NULL;

    return new_item;
}

Customer* new_customer(char* name) {

    Customer* new_user = (Customer *)calloc(1, sizeof(Customer));
    strcpy(new_user->name, name);
    new_user->cart = NULL;  
    return new_user;
}

void free_customer(Customer* customer) {
    // TODO: Complete this function.

    ItemNode* current = customer->cart;
    while (current != NULL) {
        ItemNode* next = current->next;
        free(current);
        current = next;
    }
    free(customer);
}

CheckoutLane* open_new_checkout_line() {

    CheckoutLane *new_checoutLane = (CheckoutLane *)calloc(1, sizeof(CheckoutLane));

    return new_checoutLane;
}

CheckoutLaneNode* new_checkout_node(Customer* customer) {
    CheckoutLaneNode* new_checkoutNode = (CheckoutLaneNode *)calloc(1, sizeof(CheckoutLaneNode));

    new_checkoutNode->customer = customer;

    return new_checkoutNode;
}

void add_item_to_cart(Customer* customer, char* item_name, int amount) {
    if (amount <= 0) {
        return;
    }

    ItemNode* new_item = new_item_node(item_name, amount);
    ItemNode* current = customer->cart;
    ItemNode* prev = NULL;

    while (current != NULL && strcmp(current->name, item_name) < 0) {
        prev = current;
        current = current->next;
    }

    if (current != NULL && strcmp(current->name, item_name) == 0) {
        current->count += amount;
        free(new_item);  
    } else {
        new_item->next = current;

        if (prev == NULL) {
            customer->cart = new_item;
        } else {
            prev->next = new_item;
        }
    }
}

void remove_item_from_cart(Customer* customer, char* item_name, int amount) {

    if (amount <= 0) {
        return;
    }

    ItemNode* current = customer->cart;
    ItemNode* prev = NULL;

    while (current != NULL && strcmp(current->name, item_name) != 0) {
        prev = current;
        current = current->next;
    }

    if (current != NULL && strcmp(current->name, item_name) == 0) {
        current->count = current->count - amount;

        if (current->count <= 0) {
            if (prev == NULL) {
                customer->cart = current->next;
            } else {
                prev->next = current->next;
            }
            free(current);
        }
    }

}

int total_number_of_items(Customer* customer) {

    int count = 0;

    ItemNode* current = customer->cart;

    while (current != NULL)
    {
        count = count + current->count;
        current = current->next;
    }

    return count;
}

void queue(Customer* customer, CheckoutLane* lane) {

    CheckoutLaneNode* new_customer = new_checkout_node(customer);
    CheckoutLaneNode* current_customer = lane->first;

    if (new_customer == NULL)
    {
        return;
    }

    else if (lane->first == NULL)
    {
        lane->first = lane->last = new_customer;
        return;
    }

    else if (lane->last->back == NULL)
    {
        lane->last->back = new_customer;

        new_customer->front = lane->last;
        lane->last = new_customer;
        
        return;
    }

    

}

int process(CheckoutLane* lane) {

    if (lane->first == NULL) {
        return 0;
    }

    CheckoutLaneNode* processed_customer_node = lane->first;
    Customer* processed_customer = processed_customer_node->customer;

    int cart_total = total_number_of_items(processed_customer);

    lane->first = processed_customer_node->back;

    if (lane->first == NULL) {
        lane->last = NULL;
    }

    free(processed_customer_node);

    return cart_total;
}

bool balance_lanes(CheckoutLane* lanes[], int number_of_lanes) {
    int i;

    int busiest_lane_count = 0;
    int least_busiest_lane_count = 2000; // Initialize to a large value

    CheckoutLaneNode* busiest_lane_customer = NULL;
    CheckoutLane* busiest_lane = NULL;
    CheckoutLane* least_busiest_lane = NULL;

    // Find the busiest and least busiest lanes
    for (i = 0; i < number_of_lanes; i++) {
        CheckoutLaneNode* current_customer = lanes[i]->first;
        int current_lane_count = 0;

        while (current_customer != NULL) {
            current_lane_count++;
            current_customer = current_customer->back;
        }

        if (current_lane_count > busiest_lane_count) {
            busiest_lane_count = current_lane_count;
            busiest_lane = lanes[i];
        }

        else if (current_lane_count < least_busiest_lane_count) {
            least_busiest_lane_count = current_lane_count;
            least_busiest_lane = lanes[i];
        }
    }

    if (busiest_lane_count - least_busiest_lane_count <= 1 || number_of_lanes < 2) {
        return false;
    }

    if (busiest_lane->first != NULL) {
        CheckoutLaneNode* moved_customer = busiest_lane->last;

        if (least_busiest_lane->first == NULL) {
            least_busiest_lane->first = least_busiest_lane->last = moved_customer;
        } else {
            least_busiest_lane->last->back = moved_customer;
            least_busiest_lane->last = moved_customer;
        }

        busiest_lane->last = moved_customer->front;
        if (busiest_lane->last != NULL) {
            busiest_lane->last->back = NULL;
        }

        

        return true;
    }

    return false;
}

int process_all_lanes(CheckoutLane* lanes[], int number_of_lanes) {

    int i;
    int counter = 0;

    for (i = 0; i < number_of_lanes; i++) {
        int new_sum = process(lanes[i]);
        counter += new_sum;
    }

    return counter;
}

void close_store(CheckoutLane* lanes[], int number_of_lanes) {
    int i;
    for (i = 0; i < number_of_lanes; i++) {
        free(lanes[i]);
    }
}


