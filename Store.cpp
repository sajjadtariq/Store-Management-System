#include <iostream>
#include <cstring>
using namespace std;

#define MAX_NAME_LENGTH 1024

struct ItemNode {
    char name[MAX_NAME_LENGTH];
    int count;
    ItemNode* next;

    ItemNode(const char* itemName, int itemCount) {
        strcpy(name, itemName);
        count = itemCount;
        next = nullptr;
    }
};

struct Customer {
    char name[MAX_NAME_LENGTH];
    ItemNode* cart;
    Customer* next;

    Customer(const char* customerName) {
        strcpy(name, customerName);
        cart = nullptr;
        next = nullptr;
    }

    ~Customer() {
        ItemNode* temp = cart;
        while (temp != nullptr) {
            ItemNode* nextItem = temp->next;
            delete temp;
            temp = nextItem;
        }
    }
};

struct CheckoutLaneNode {
    Customer* customer;
    CheckoutLaneNode* front;
    CheckoutLaneNode* back;

    CheckoutLaneNode(Customer* cust) {
        customer = cust;
        front = nullptr;
        back = nullptr;
    }
};

struct CheckoutLane {
    CheckoutLaneNode* first;
    CheckoutLaneNode* last;

    CheckoutLane() {
        first = nullptr;
        last = nullptr;
    }

    ~CheckoutLane() {
        CheckoutLaneNode* temp1 = first;
        while (temp1 != nullptr) {
            CheckoutLaneNode* temp2 = temp1;
            temp1 = temp1->front;
            delete temp2;
        }
    }
};

struct CustomerList {
    Customer* head;

    CustomerList() {
        head = nullptr;
    }

    ~CustomerList() {
        Customer* current = head;
        while (current != nullptr) {
            Customer* next = current->next;
            delete current;
            current = next;
        }
    }

    void addCustomer(Customer* newCustomer) {
        if (head == nullptr) {
            head = newCustomer;
        } else {
            Customer* temp = head;
            while (temp->next != nullptr) {
                temp = temp->next;
            }
            temp->next = newCustomer;
        }
    }

    Customer* findCustomer(const char* customerName) {
        Customer* temp = head;
        while (temp != nullptr) {
            if (strcmp(temp->name, customerName) == 0) {
                return temp;
            }
            temp = temp->next;
        }
        return nullptr;
    }
};

void displayAllCustomers(const CustomerList& customerList) {
    Customer* temp = customerList.head;
    while (temp != nullptr) {
        cout << "Customer Name: " << temp->name;
        ItemNode* cartItem = temp->cart;
        while (cartItem != nullptr) {
            cout << "  Item: " << cartItem->name << ", Count: " << cartItem->count;
            cartItem = cartItem->next;
        }
        temp = temp->next;
        cout << endl;
    }
}

void add_item_to_cart(Customer* customer, const char* item_name, int amount) {
    if (amount <= 0)
        return;
    if (customer->cart == nullptr) {
        customer->cart = new ItemNode(item_name, amount);
        return;
    }

    ItemNode* new_item = new ItemNode(item_name, amount);
    ItemNode* temp1 = customer->cart, * temp2 = nullptr;

    while (temp1 != nullptr) {
        if (strcmp(temp1->name, item_name) == 0) {
            temp1->count += amount;
            delete new_item;
            return;
        }
        if (strcmp(item_name, temp1->name) < 0) {
            if (temp2 == nullptr) {
                new_item->next = temp1;
                customer->cart = new_item;
                return;
            }
            else {
                temp2->next = new_item;
                new_item->next = temp1;
                return;
            }
        }
        temp2 = temp1;
        temp1 = temp1->next;
    }
    temp2->next = new_item;
}

void remove_item_from_cart(Customer* customer, const char* item_name, int amount) {
    if (amount <= 0)
        return;
    ItemNode* temp1 = customer->cart, * temp2 = nullptr;
    while (temp1 != nullptr) {
        if (strcmp(temp1->name, item_name) == 0) {
            temp1->count -= amount;
            if (temp1->count <= 0) {
                if (temp2 == nullptr) {
                    customer->cart = temp1->next;
                    delete temp1;
                    return;
                }
                else {
                    temp2->next = temp1->next;
                    delete temp1;
                    return;
                }
            }
        }
        temp2 = temp1;
        temp1 = temp1->next;
    }
}


int total_number_of_items(Customer* customer) {
    int cnt = 0;
    ItemNode* temp = customer->cart;
    while (temp != nullptr) {
        cnt += temp->count;
        temp = temp->next;
    }
    return cnt;
}

void queue(Customer* customer, CheckoutLane* lane) {
    CheckoutLaneNode* new_node = new CheckoutLaneNode(customer);
    if (lane->first == nullptr) {
        lane->first = new_node;
        lane->last = new_node;
    }
    else {
        lane->last->front = new_node;
        new_node->back = lane->last;
        lane->last = new_node;
    }
}

int process(CheckoutLane* lane) {
    if (lane == nullptr || lane->first == nullptr)
        return 0;
    int items = 0;
    CheckoutLaneNode* free_node = lane->first;
    items = total_number_of_items(free_node->customer);
    lane->first = lane->first->front;
    if (lane->first != nullptr)
        lane->first->back = nullptr;
    else
        lane->last = nullptr;
    delete free_node->customer;
    delete free_node;
    return items;
}

int Num_customers(CheckoutLane* lane) {
    if (lane == nullptr)
        return 0;
    int cnt = 0;
    CheckoutLaneNode* temp = lane->first;
    while (temp != nullptr) {
        cnt++;
        temp = temp->front;
    }
    return cnt;
}

CheckoutLane* Busiest_Lane(CheckoutLane* lanes[], int number_of_lanes) {
    if (number_of_lanes < 2)
        return nullptr;
    int max_customers = 0;
    CheckoutLane* busiest_lane = nullptr;
    for (int i = 0; i < number_of_lanes; i++) {
        int no_of_customers = Num_customers(lanes[i]);
        if (no_of_customers > max_customers) {
            max_customers = no_of_customers;
            busiest_lane = lanes[i];
        }
    }
    return busiest_lane;
}

CheckoutLane* Least_Lane(CheckoutLane* lanes[], int number_of_lanes) {
    if (number_of_lanes < 2)
        return nullptr;
    int min_customers = Num_customers(lanes[0]);
    CheckoutLane* least_lane = lanes[0];
    for (int i = 1; i < number_of_lanes; i++) {
        int no_of_customers = Num_customers(lanes[i]);
        if (no_of_customers < min_customers) {
            min_customers = no_of_customers;
            least_lane = lanes[i];
        }
    }
    return least_lane;
}

Customer* remove_last_customer(CheckoutLane* lane) {
    if (lane == nullptr || lane->first == nullptr)
        return nullptr;
    if (lane->first == lane->last) {
        CheckoutLaneNode* temp = lane->first;
        Customer* customer = temp->customer;
        lane->first = nullptr;
        lane->last = nullptr;
        delete temp;
        return customer;
    }
    CheckoutLaneNode* current = lane->first;
    CheckoutLaneNode* previous = nullptr;
    while (current != lane->last) {
        previous = current;
        current = current->front;
    }
    previous->front = nullptr;
    lane->last = previous;
    Customer* customer = current->customer;
    delete current;
    return customer;
}

bool balance_lanes(CheckoutLane* lanes[], int number_of_lanes) {
    CheckoutLane* least_lane = Least_Lane(lanes, number_of_lanes);
    CheckoutLane* busy_lane = Busiest_Lane(lanes, number_of_lanes);

    cout << "Number of people in each lane before balancing:\n";
    for (int i = 0; i < number_of_lanes; i++) {
        cout << "Lane " << i << ": " << Num_customers(lanes[i]) << " customers\n";
    }

    if ((Num_customers(busy_lane) - Num_customers(least_lane)) <= 1)
        return false;
    Customer* customer = remove_last_customer(busy_lane);
    if (customer != nullptr) {
        queue(customer, least_lane);
        return true;
    }
    return false;
}

int process_all_lanes(CheckoutLane* lanes[], int number_of_lanes) {
    int result = 0;
    for (int i = 0; i < number_of_lanes; i++) {
        if (lanes[i] != nullptr)
            result += process(lanes[i]);
    }
    return result;
}

//void close_store(CheckoutLane* lanes[], int number_of_lanes) {
//    for (int i = 0; i < number_of_lanes; i++) {
//        if (lanes[i] != nullptr) {
//            CheckoutLaneNode* current = lanes[i]->first;
//            while (current != nullptr) {
//                CheckoutLaneNode* next_node = current->front;
//                delete current->customer;
//                delete current;
//                current = next_node;
//            }
//            delete lanes[i];
//            lanes[i] = nullptr;
//        }
//    }
//}

void show_number_of_people_in_each_lane(CheckoutLane* lanes[], int number_of_lanes) {
    cout << "Number of people in each lane:\n";
    for (int i = 0; i < number_of_lanes; i++) {
        cout << "Lane " << i << ": " << Num_customers(lanes[i]) << " customers\n";
    }
}

void edit_item_in_cart(Customer* customer, const char* item_name, int new_amount) {
    if (new_amount <= 0) {
        remove_item_from_cart(customer, item_name, -new_amount);
        return;
    }

    ItemNode* temp = customer->cart;
    while (temp != nullptr) {
        if (strcmp(temp->name, item_name) == 0) {
            temp->count = new_amount;
            return;
        }
        temp = temp->next;
    }

    add_item_to_cart(customer, item_name, new_amount);
}

void show_customer_cart(Customer* customer) {
    if (customer == nullptr) {
        cout << "Customer not found.\n";
        return;
    }

    cout << "Customer Name: " << customer->name << endl;
    ItemNode* cartItem = customer->cart;
    if (cartItem == nullptr) {
        cout << "Cart is empty.\n";
        return;
    }

    cout << "Items in Cart:\n";
    while (cartItem != nullptr) {
        cout << "  Item: " << cartItem->name << ", Count: " << cartItem->count << endl;
        cartItem = cartItem->next;
    }
}

int main() {
    int number_of_lanes;
    cout << "Enter the number of checkout lanes: ";
    cin >> number_of_lanes;
    cin.ignore(); // Ignore the newline character left in the buffer

    CustomerList customerList;
    CheckoutLane* lanes[number_of_lanes];
    for (int i = 0; i < number_of_lanes; i++) {
        lanes[i] = new CheckoutLane();
    }

    while (true) {
        cout << "\nMenu:\n";
        cout << "1. Add a customer\n";
        cout << "2. Display customers / cart items\n";
        cout << "3. Add item to customer's cart\n";
        cout << "4. Edit item in customer's cart\n";
        cout << "5. Queue customer at a checkout lane\n";
        cout << "6. Show number of people in each lane\n";
        cout << "7. Balance lanes\n";
        cout << "8. Process all lanes\n";
        cout << "9. Exit\n";
        cout << "Enter your choice: ";

        int choice;
        cin >> choice;
        cin.ignore(); // Ignore the newline character left in the buffer

        if (choice == 1) {
            char customer_name[MAX_NAME_LENGTH];
            cout << "Enter customer name: ";
            cin.getline(customer_name, MAX_NAME_LENGTH);
            if (strlen(customer_name) == 0) {
                cout << "Customer name cannot be empty.\n";
                continue;
            }
            Customer* new_customer = new Customer(customer_name);
            customerList.addCustomer(new_customer);
            cout << "Customer " << customer_name << " added.\n";

        } else if (choice == 2) {
            cout << "21. Display all customers\n";
            cout << "22. Display all items in customer's cart\n";
            cin >> choice;
            cin.ignore(); // Ignore the newline character left in the buffer

            if (choice == 21) {
                displayAllCustomers(customerList);
            } else if (choice == 22) {
                char customer_name[MAX_NAME_LENGTH];
                cout << "Enter customer name: ";
                cin.getline(customer_name, MAX_NAME_LENGTH);

                Customer* customer = customerList.findCustomer(customer_name);
                show_customer_cart(customer);
            }
        } else if (choice == 3) {
            char customer_name[MAX_NAME_LENGTH];
            char item_name[MAX_NAME_LENGTH];
            int amount;
            cout << "Enter customer name: ";
            cin.getline(customer_name, MAX_NAME_LENGTH);
            cout << "Enter item name: ";
            cin.getline(item_name, MAX_NAME_LENGTH);
            cout << "Enter item amount: ";
            cin >> amount;
            cin.ignore(); // Ignore the newline character left in the buffer

            Customer* customer = customerList.findCustomer(customer_name);
            if (customer) {
                add_item_to_cart(customer, item_name, amount);
                cout << "Added " << amount << " " << item_name << " to " << customer_name << "'s cart.\n";
            } else {
                cout << "Customer not found.\n";
            }

        } else if (choice == 4) {
            char customer_name[MAX_NAME_LENGTH];
            char item_name[MAX_NAME_LENGTH];
            int new_amount;
            cout << "Enter customer name: ";
            cin.getline(customer_name, MAX_NAME_LENGTH);
            cout << "Enter item name: ";
            cin.getline(item_name, MAX_NAME_LENGTH);
            cout << "Enter new item amount: ";
            cin >> new_amount;
            cin.ignore(); // Ignore the newline character left in the buffer

            Customer* customer = customerList.findCustomer(customer_name);
            if (customer) {
                edit_item_in_cart(customer, item_name, new_amount);
                cout << "Item " << item_name << " in " << customer_name << "'s cart updated to " << new_amount << ".\n";
            } else {
                cout << "Customer not found.\n";
            }

        } else if (choice == 5) {
            char customer_name[MAX_NAME_LENGTH];
            int lane_number;
            cout << "Enter customer name: ";
            cin.getline(customer_name, MAX_NAME_LENGTH);
            cout << "Enter lane number (0 to " << number_of_lanes-1 << "): ";
            cin >> lane_number;
            cin.ignore(); // Ignore the newline character left in the buffer

            if (lane_number >= number_of_lanes || lane_number < 0) {
                cout << "Invalid lane number.\n";
            } else {
                Customer* customer = customerList.findCustomer(customer_name);
                if (customer) {
                    queue(customer, lanes[lane_number]);
                    cout << "Customer " << customer_name << " queued at lane " << lane_number << ".\n";
                } else {
                    cout << "Customer not found.\n";
                }
            }

        } else if (choice == 6) {
            show_number_of_people_in_each_lane(lanes, number_of_lanes);

        } else if (choice == 7) {
            if (balance_lanes(lanes, number_of_lanes)) {
                cout << "Lanes balanced successfully.\n";
            } else {
                cout << "No need to balance lanes.\n";
            }
            show_number_of_people_in_each_lane(lanes, number_of_lanes);
        } else if (choice == 8) {
            int total_items_processed = process_all_lanes(lanes, number_of_lanes);
            cout << "Total items processed: " << total_items_processed << endl;
        } else if (choice == 9) {
            break;
        } else {
            cout << "Invalid choice. Please try again.\n";
        }
    }


    // Cleanup customers
    customerList.~CustomerList();

    return 0;
}
