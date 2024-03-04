#include "wackystore.c"
#include <assert.h>

void print_customer(Customer* customer) {
    printf("Customer: %s\n", customer->name);
    printf("  Cart [%d]:\n", total_number_of_items(customer));

    ItemNode* head = customer->cart;
    while (head != NULL) {
        printf("    - %s x %d\n", head->name, head->count);
        head = head->next;
    }

    printf("\n");
}

void test_single_item_in_cart() {
    Customer* customer = new_customer("Charles");

    // First item in customer's cart should be some V-Bucks
    add_item_to_cart(customer, "V-Bucks", 2800);
    assert(strcmp(customer->cart->name, "V-Bucks") == 0);
    assert(customer->cart->count == 2800);

    // Customer wants some more V-Bucks.
    add_item_to_cart(customer, "V-Bucks", 10);
    assert(strcmp(customer->cart->name, "V-Bucks") == 0);
    assert(customer->cart->count == 2810);

    // Customer removes some V-Bucks.
    remove_item_from_cart(customer, "V-Bucks", 23);
    assert(strcmp(customer->cart->name, "V-Bucks") == 0);
    assert(customer->cart->count == 2787);

    // Customer removes all V-Bucks. Cart is empty.
    remove_item_from_cart(customer, "V-Bucks", 999999);
    assert(customer->cart == NULL);

    free_customer(customer);
}

void test_many_items_in_cart() {
    Customer* customer = new_customer("Charles");

    // Customer wants some more items.
    add_item_to_cart(customer, "bAnAnA", 63);
    add_item_to_cart(customer, "duRiAn", 55);
    add_item_to_cart(customer, "AppLe", 42);
    add_item_to_cart(customer, "CheRRy", 23);

    // There should be a total of 183 items.
    assert(total_number_of_items(customer) == 183);

    // Print out customer.
    print_customer(customer);

    // All items should be in ascending strcmp() sorted order.
    assert(strcmp(customer->cart->name, "AppLe") == 0);
    assert(strcmp(customer->cart->next->name, "CheRRy") == 0);
    assert(strcmp(customer->cart->next->next->name, "bAnAnA") == 0);
    assert(strcmp(customer->cart->next->next->next->name, "duRiAn") == 0);

    free_customer(customer);
}

void test_single_checkout_lane() {
    Customer* charles = new_customer("Charles");
    add_item_to_cart(charles, "RP", 10000);

    Customer* helen = new_customer("Helen");
    add_item_to_cart(helen, "Pikachu Plushie", 1);
    add_item_to_cart(helen, "Advil", 30);

    CheckoutLane* lane = open_new_checkout_line();
    queue(helen, lane);
    queue(charles, lane);

    // Helen is first in the queue, and bought 31 items.
    assert(process(lane) == 31);

    // Charles is next, with 10000 items.
    assert(process(lane) == 10000);

    // Queue should be empty now.
    assert(process(lane) == 0);
    assert(lane->first == NULL);
    assert(lane->last == NULL);

    // Close the store. There was only one lane open.
    close_store(&lane, 1);
}

void print_customers_in_lane(char lane_id[], CheckoutLane* lane) {
    printf("Lane %s:\n\t-> ", lane_id);

    CheckoutLaneNode* head = lane->first;
    while (head != NULL) {
        printf("%s ", head->customer->name);
        head = head->back;
    }

    printf("\n");
}

void test_multiple_checkout_lanes() {
    // Create all lanes and customers.
    CheckoutLane* lanes[] = {open_new_checkout_line(), open_new_checkout_line(),
                             open_new_checkout_line()};

    Customer* alex = new_customer("Alex");
    Customer* billy = new_customer("Billy");
    Customer* charles = new_customer("Charles");
    Customer* devin = new_customer("Devin");
    Customer* eric = new_customer("Eric");
    Customer* frank = new_customer("Frank");
    Customer* gary = new_customer("Gary");
    Customer* henry = new_customer("Henry");
    Customer* issac = new_customer("Issac");

    // Add customers to lanes, as shown on diagram.
    queue(alex, lanes[0]);

    queue(billy, lanes[1]);
    queue(charles, lanes[1]);
    queue(devin, lanes[1]);
    queue(eric, lanes[1]);

    queue(frank, lanes[2]);
    queue(gary, lanes[2]);
    queue(henry, lanes[2]);
    queue(issac, lanes[2]);

    // Show the lanes after each call to balance_lanes()
    printf("Showing Initial Lanes ...\n");
    print_customers_in_lane("Lane 1", lanes[0]);
    print_customers_in_lane("Lane 2", lanes[1]);
    print_customers_in_lane("Lane 3", lanes[2]);
    printf("\n");

    balance_lanes(lanes, 3);

    printf("Showing Lanes after calling balance_lanes()...\n");
    print_customers_in_lane("Lane 1", lanes[0]);
    print_customers_in_lane("Lane 2", lanes[1]);
    print_customers_in_lane("Lane 3", lanes[2]);
    printf("\n");

    balance_lanes(lanes, 3);

    printf("Showing Lanes after calling balance_lanes()...\n");
    print_customers_in_lane("Lane 1", lanes[0]);
    print_customers_in_lane("Lane 2", lanes[1]);
    print_customers_in_lane("Lane 3", lanes[2]);
    printf("\n");

    // Serve no customers, and cleanup the store.
    close_store(lanes, 3);
}

int main(void) {
    test_single_item_in_cart();
    test_many_items_in_cart();
    test_single_checkout_lane();
    test_multiple_checkout_lanes();
    printf("Looks good. Have you made any test cases of your own?\n");
    return 0;
}
