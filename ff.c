#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#define MAX_ID_NUMBER 100

struct Node *head;
struct Node *block_of_space;
struct Node *temp;

char request[3];
char process[3];
char algo_type[2];
int last_address_space;
int space_requested;

struct Node {
    int available_space;
    int start_address;
    int end_address;
    struct Node *next;
    char process_id[MAX_ID_NUMBER];
};

void merge_adjacent_free_blocks() {
    struct Node *current = head->next;
    while (current != NULL && current->next != NULL) {
        if (strcmp(current->process_id, "Unused") == 0 &&
            strcmp(current->next->process_id, "Unused") == 0) {
            current->end_address = current->next->end_address;
            current->available_space += current->next->available_space;
            struct Node *tempNode = current->next;
            current->next = tempNode->next;
            free(tempNode);
        } else {
            current = current->next;
        }
    }
}

void first_fit(char process_id[3], int space_requested) {
    temp = head;
    int left_over_space = 0;
    while (temp->next != NULL) {
        if (strcmp(temp->next->process_id, "Unused") == 0 &&
            temp->next->available_space >= space_requested) {
            head->available_space -= space_requested;
            strcpy(temp->next->process_id, process_id);
            temp->next->end_address = temp->next->start_address + space_requested - 1;
            left_over_space = temp->next->available_space - space_requested;

            if (left_over_space > 0) {
                struct Node *newNode = (struct Node *)malloc(sizeof(struct Node));
                strcpy(newNode->process_id, "Unused");
                newNode->available_space = left_over_space;
                temp->next->available_space = space_requested;
                newNode->start_address = temp->next->end_address + 1;
                newNode->end_address = newNode->start_address + left_over_space - 1;
                if (newNode->end_address > last_address_space)
                    newNode->end_address = last_address_space;
                newNode->next = temp->next->next;
                temp->next->next = newNode;
            }

            merge_adjacent_free_blocks();
            return;
        } else
            temp = temp->next;
    }
    printf("There is no space to place process %s, of %dkb\n", process_id, space_requested);
}

void best_fit(char process_id[3], int space_requested) {
    struct Node *best_node = NULL;
    int smallest_space = 999999;
    temp = head;

    while (temp->next != NULL) {
        if (strcmp(temp->next->process_id, "Unused") == 0 &&
            temp->next->available_space >= space_requested &&
            temp->next->available_space < smallest_space) {
            smallest_space = temp->next->available_space;
            best_node = temp;
        }
        temp = temp->next;
    }

    if (best_node != NULL) {
        temp = best_node;
        head->available_space -= space_requested;
        strcpy(temp->next->process_id, process_id);
        temp->next->end_address = temp->next->start_address + space_requested - 1;

        int left_over_space = temp->next->available_space - space_requested;
        if (left_over_space > 0) {
            struct Node *newNode = (struct Node *)malloc(sizeof(struct Node));
            strcpy(newNode->process_id, "Unused");
            newNode->available_space = left_over_space;
            temp->next->available_space = space_requested;
            newNode->start_address = temp->next->end_address + 1;
            newNode->end_address = newNode->start_address + left_over_space - 1;
            if (newNode->end_address > last_address_space)
                newNode->end_address = last_address_space;
            newNode->next = temp->next->next;
            temp->next->next = newNode;
        }

        merge_adjacent_free_blocks();
        return;
    }

    printf("There is no space to place process %s, of %dkb\n", process_id, space_requested);
}

void worst_fit(char process_id[3], int space_requested) {
    struct Node *worst_node = NULL;
    int largest_space = -1;
    temp = head;

    while (temp->next != NULL) {
        if (strcmp(temp->next->process_id, "Unused") == 0 &&
            temp->next->available_space >= space_requested &&
            temp->next->available_space > largest_space) {
            largest_space = temp->next->available_space;
            worst_node = temp;
        }
        temp = temp->next;
    }

    if (worst_node != NULL) {
        temp = worst_node;
        head->available_space -= space_requested;
        strcpy(temp->next->process_id, process_id);
        temp->next->end_address = temp->next->start_address + space_requested - 1;

        int left_over_space = temp->next->available_space - space_requested;
        if (left_over_space > 0) {
            struct Node *newNode = (struct Node *)malloc(sizeof(struct Node));
            strcpy(newNode->process_id, "Unused");
            newNode->available_space = left_over_space;
            temp->next->available_space = space_requested;
            newNode->start_address = temp->next->end_address + 1;
            newNode->end_address = newNode->start_address + left_over_space - 1;
            if (newNode->end_address > last_address_space)
                newNode->end_address = last_address_space;
            newNode->next = temp->next->next;
            temp->next->next = newNode;
        }

        merge_adjacent_free_blocks();
        return;
    }

    printf("There is no space to place process %s, of %dkb\n", process_id, space_requested);
}

void request_memory(char process_id[3], int space_requested, char algo[2]) {
    if (strcmp("W", algo) == 0)
        worst_fit(process_id, space_requested);
    else if (strcmp("B", algo) == 0)
        best_fit(process_id, space_requested);
    else if (strcmp("F", algo) == 0)
        first_fit(process_id, space_requested);
    else
        printf("Choose between best_fit, worst_fit, and first_fit. Try again\n");
}

void release_memory(char process_id[3]) {
    temp = head;
    int found = 0;

    while (temp->next != NULL) {
        if (strcmp(temp->next->process_id, process_id) == 0) {
            strcpy(temp->next->process_id, "Unused");
            head->available_space += temp->next->available_space;
            found = 1;
        }
        temp = temp->next;
    }

    if (!found)
        printf("No process with ID %s was found.\n", process_id);

    merge_adjacent_free_blocks();
}

void compact() {
    struct Node *current = head->next;
    int current_start = 0;
    struct Node *new_list = NULL, *tail = NULL;

    while (current != NULL) {
        if (strcmp(current->process_id, "Unused") != 0) {
            struct Node *allocated = (struct Node *)malloc(sizeof(struct Node));
            strcpy(allocated->process_id, current->process_id);
            allocated->start_address = current_start;
            allocated->end_address = current_start + current->available_space - 1;
            allocated->available_space = current->available_space;
            allocated->next = NULL;

            current_start = allocated->end_address + 1;

            if (new_list == NULL) {
                new_list = allocated;
                tail = allocated;
            } else {
                tail->next = allocated;
                tail = allocated;
            }
        }
        current = current->next;
    }

    int remaining_space = last_address_space - current_start + 1;
    if (remaining_space > 0) {
        struct Node *free_block = (struct Node *)malloc(sizeof(struct Node));
        strcpy(free_block->process_id, "Unused");
        free_block->start_address = current_start;
        free_block->end_address = last_address_space;
        free_block->available_space = remaining_space;
        free_block->next = NULL;

        if (new_list == NULL) {
            new_list = free_block;
        } else {
            tail->next = free_block;
        }
    }

    head->next = new_list;
}

void status_report() {
    temp = head;
    printf("Available space left: %d\n", head->available_space);
    while (temp->next != NULL) {
        printf("Addresses [%d : %d] Process %s\n", temp->next->start_address, temp->next->end_address, temp->next->process_id);
        temp = temp->next;
    }
}

int main() {
    int initial_memory;
    printf("Enter the initial memory size (in KB): ");
    scanf("%d", &initial_memory);

    printf("Initial Memory: %d KB\n", initial_memory);
    char user_input[128];

    head = (struct Node *)malloc(sizeof(struct Node));
    block_of_space = (struct Node *)malloc(sizeof(struct Node));

    strcpy(head->process_id, "Dummy Node");
    head->start_address = -1;
    head->end_address = -1;
    head->available_space = initial_memory;
    head->next = block_of_space;

    strcpy(block_of_space->process_id, "Unused");
    block_of_space->start_address = 0;
    block_of_space->end_address = initial_memory - 1;
    block_of_space->available_space = initial_memory;
    block_of_space->next = NULL;

    last_address_space = initial_memory - 1;

    while (1) {
        printf("Enter your command (RQ for request, RL for release, C for compact, S for status report, Q to quit): ");
        scanf("%s", user_input);

        if (strcmp(user_input, "RQ") == 0) {
            printf("Enter Process ID: ");
            scanf("%s", process);
            printf("Enter algorithm (F for First Fit, B for Best Fit, W for Worst Fit): ");
            scanf("%s", algo_type);
            printf("Enter space requested (in KB): ");
            scanf("%d", &space_requested);

            request_memory(process, space_requested, algo_type);
        } else if (strcmp(user_input, "RL") == 0) {
            printf("Enter Process ID to release: ");
            scanf("%s", process);
            release_memory(process);
        } else if (strcmp(user_input, "C") == 0) {
            compact();
        } else if (strcmp(user_input, "S") == 0) {
            status_report();
        } else if (strcmp(user_input, "Q") == 0) {
            break;
        } else {
            printf("Invalid command. Try again.\n");
        }
    }

    return 0;
}
