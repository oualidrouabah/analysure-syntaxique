/*
*   S -> S+S | S-S | S/S | S*S | (S) | id/const 
*
*  contact@salimzaidi.com
*   Remove la recursivitie pour anlyseur LL(1)
*   S -> T I
*   I -> + T I | − T I | ϵ
*   T -> F H
*   H -> ∗ F H | / F H | ϵ
*   F -> (S) | id/conts
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Define a stack structure to manage parsing states
typedef struct Stack {
    char val;            // The value stored in the stack (non-terminal or token)
    struct Stack *next;  // Pointer to the next element in the stack
} Stack;

typedef Stack* StackPtr;


bool is_empty(StackPtr stack) {
    return stack == NULL;
}


char top(StackPtr stack) {
    return is_empty(stack) ? '\0' : stack->val;
}


StackPtr push(StackPtr *stack, char value) {
    StackPtr new_node = (Stack *)malloc(sizeof(Stack)); // Allocate memory for a new node
    if (new_node == NULL) {
        exit(EXIT_FAILURE); // Exit on allocation failure
    }
    new_node->val = value;       // Set the value of the new node
    new_node->next = *stack;     // Link the new node to the current stack
    *stack = new_node;           // Update the stack pointer
    return new_node;
}


char pop(StackPtr *stack) {
    if (is_empty(*stack)) {
        return '\0'; // Return null character if the stack is empty
    }
    char value = (*stack)->val;  // Get the value of the top node
    StackPtr temp = *stack;      // Temporary pointer to the top node
    *stack = (*stack)->next;     // Update the stack pointer
    free(temp);                  // Free the memory of the removed node
    return value;
}


void print_stack(StackPtr stack) {
    printf("Stack: ");
    if (is_empty(stack)) {
        printf("(empty)");
    } else {
        StackPtr current = stack;
        while (current != NULL) {
            printf("%c ", current->val); // Print each stack value
            current = current->next;
        }
    }
    printf("\n");
}

// Transition table for grammar parsing, with states and actions
int transition_table[5][8] = {
    {1, 1, -1, -1, -1, -1, -1, -1},  // S
    {-1, -1, 4, -1, -1, 2, 3, 4},    // I
    {5, 5, -1, -1, -1, -1, -1, -1},  // T
    {-1, -1, 8, 6, 7, 8, 8, 8},      // H
    {9, 10, -1, -1, -1, -1, -1, -1}  // F
};


int get_token(char c) {
    if (c >= '0' && c <= '9') return 0; // Digit token
    if (c == '(') return 1;             // Left parenthesis
    if (c == ')') return 2;             // Right parenthesis
    if (c == '*') return 3;             // Multiplication
    if (c == '/') return 4;             // Division
    if (c == '+') return 5;             // Addition
    if (c == '-') return 6;             // Subtraction
    if (c == '$') return 7;             // End-of-input
    return -1;                          // Invalid token
}

int analyseur_sytax(const char *input) {
    StackPtr stack = NULL;              // Initialize an empty stack

    // Initialize the stack with the end marker and start non-terminal
    push(&stack, '$');
    push(&stack, 'S');

    int production = 0;// Start state
    int i = 0;// Input index

    while (input[i] != '\0') {// Process the input
        print_stack(stack); // Debug output
        printf("Chaine: "); // Debug output
        for(int j=i; input[j]!='\0'; j++) printf("%c", input[j]); // Debug output
        printf("\n");
        printf("Top: %c, Input: %c\n", top(stack), input[i]); // Debug output

        char top_symbole = top(stack);// Get the top of the stack

        // Map non-terminal symbols to states
        switch (top_symbole) {
            case 'S': production = 0; break;
            case 'I': production = 1; break;
            case 'T': production = 2; break;
            case 'H': production = 3; break;
            case 'F': production = 4; break;
            default: break;
        }

        int token = get_token(input[i]); // Get the token index
        production = transition_table[production][token]; // Use the transition table
        
        printf("Production: %d \n",production);
        if (production == -1) {      // Invalid state transition
            printf("Parsing failed \nNot Accepted\n");
            return EXIT_FAILURE;
        }
        printf("Pop %c \n", top(stack)); // Debug output
        pop(&stack);// Pop the top of the stack
        
        // Push corresponding grammar rules based on the state
        switch (production) {
            case 1: push(&stack, 'I'); push(&stack, 'T'); printf("Apply  S -> T I\n"); break;
            case 2: push(&stack, 'I'); push(&stack, 'T'); push(&stack, '+'); printf("Apply  I -> + T I\n"); break;
            case 3: push(&stack, 'I'); push(&stack, 'T'); push(&stack, '-'); printf("Apply  I -> - T I\n"); break;
            case 4: printf("Apply I -> epsilon \n"); break;
            case 5: push(&stack, 'H'); push(&stack, 'F'); printf("Apply  T -> F H\n"); break;
            case 6: push(&stack, 'H'); push(&stack, 'F'); push(&stack, '*'); printf("Apply  H -> ∗ F H\n"); break;
            case 7: push(&stack, 'H'); push(&stack, 'F'); push(&stack, '/'); printf("Apply  H -> / F H\n"); break;
            case 8: printf("Apply H -> epsilon \n"); break;
            case 9: push(&stack, input[i]); printf("Apply  F -> id/conts\n"); break;
            case 10: push(&stack, ')'); push(&stack, 'S'); push(&stack, '('); printf("Apply  F -> (S)\n"); break;
            default: break;
        }
        if (top(stack) == input[i]) {// Match stack top and input
            if (top(stack) == '$') {
                i++;
            } else {
                printf("Top: %c = token: %c\n", top(stack), input[i]); // Debug output
                printf("Pop %c\n", top(stack)); // Debug output
                printf("Get next token ###\n"); // Debug output
                pop(&stack);
                i++;
            }
        }
    }

    // Final parsing success condition
    if (top(stack) == '$' && input[i] == '\0') {
        print_stack(stack);
        printf("token: %c\n", input[i-1]); // Debug output
        printf("Parsing successful \nAccepted input\n\n");
        return EXIT_SUCCESS;  // Parsing successful
    } else {
        printf("Top: %c ; token: %c\n", top(stack), input[i]);  // Debug outpu
        printf("Parsing failed \nNot accepted\n");
        return EXIT_FAILURE;  // Parsing failed
    }
}
int main() {
    const char *input = "2-3*(3/2)+3$"; // Input string for parsing
    analyseur_sytax(input);
    return 0;
}
