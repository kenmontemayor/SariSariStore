#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_PRODUCTS  20
#define MAX_ITEMS     20
#define NAME_LEN      100

typedef struct {
    int id;
    char name[NAME_LEN];
    double  price;
    int stock;
} Product;

typedef struct{
    int productID;
    char name[NAME_LEN];
    int qty;
    double  price;
    double  sub_total;

} Item;

typedef struct Transaction {
    int id;
    char customer[NAME_LEN];
    Item items[MAX_ITEMS];
    int itemCount;
    double total;
    double payment;
    double change;
    struct Transaction*next;
} Transaction;


static Product products[MAX_PRODUCTS] = {
    {1, "3 in 1 Nescafe", 13.00, 20},
    {2, "Bear Brand (Sachet)", 15.00, 20},
    {3, "Coke Litro", 48.00, 20},
    {4, "Piatos", 20.00, 20},
    {5, "Moby", 10.00, 20},
    {6, "Milo", 11.00, 20},
    {7, "Pancit Canton Chilimansi", 16.00, 20},
    {8, "Pancit Canton Hot & Spicy", 16.00, 20},
    {9, "Lucky Me Beef", 16.00, 20},
    {10, "San Marino (Small)", 35.00, 20},
    {11, "San Marino (Big)", 45.00, 20},
    {12, "Mega Sardines", 27.00, 20},
    {13, "Ligo Sardines", 29.00, 20},
    {14, "Zest-0", 12.00, 20},
    {15, "Cobra (Yellow)", 18.00, 20},
    {16, "San Mig Lychee (Can)", 65.00, 20},
    {17, "Red Bull (Small Can)", 90.00, 20},
    {18, "Red Bull Zero Sugar (Small Can)", 90.00, 20},
    {19, "Gin Bilog", 50.00, 20},
    {20, "Tang Grape", 22.00, 20}
};

void printProductTable(void);
void displayProducts(void);
Product *findProduct(int id);
void displaySummary(void);
void deleteTransaction(void);
void freeTransactions(void);



void printProductTable(void) {
    int i;

    printf("\n===============================================================\n");
    printf("%-5s %-35s %-12s %-10s\n",
           "ID", "PRODUCT NAME", "PRICE (PHP)", "STOCK");
    printf("---------------------------------------------------------------\n");

    for (i = 0; i < MAX_PRODUCTS; i++) {
        printf("%-5d %-35s PHP %-8.2f %-10d\n",
               products[i].id,
               products[i].name,
               products[i].price,
               products[i].stock);
    }

    printf("===============================================================\n");
}

void displayProducts(void) {
    printProductTable();
}

Product *findProduct(int id) {
    int i;

    for (i = 0; i < MAX_PRODUCTS; i++) {
        if (products[i].id == id) {
            return &products[i];
        }
    }

    return NULL;
}

/*processTransaction(), restoreStock(), addTransaction()*/
void processTransaction(void) {
    int productID, quantity;
    Product *product;
    double total;

    printProductTable();

    productID = readInt("\nEnter product ID: ");
    product = findProduct(productID);

    if (product == NULL) {
        printf("Product not found.\n");
        return;
    }

    printf("Selected product: %s\n", product->name);
    printf("Price: PHP %.2f\n", product->price);
    printf("Available stock: %d\n", product->stock);

    quantity = readInt("Enter quantity: ");

    if (quantity <= 0) {
        printf("Quantity must be greater than zero.\n");
        return;
    }

    if (quantity > product->stock) {
        printf("Insufficient stock. Sale cancelled.\n");
        return;
    }

    total = product->price * quantity;

    printf("\nTotal amount: PHP %.2f\n", total);

    {
        char confirm[10];

        printf("Confirm sale? (y/n): ");
        readLine(confirm, sizeof(confirm));

        if (tolower((unsigned char)confirm[0]) != 'y') {
            printf("Sale cancelled.\n");
            return;
        }
    }

    /* Record the transaction before changing stock. */
    if (addTransaction(product->name, quantity, total)) {
        product->stock -= quantity;

        printf("\nSale recorded successfully!\n");
        printf("Transaction ID: %d\n", nextTransactionID - 1);
        printf("Product: %s\n", product->name);
        printf("Quantity: %d\n", quantity);
        printf("Total: PHP %.2f\n", total);
        printf("Remaining stock: %d\n", product->stock);
    } else {
        printf("Unable to record sale. Stock was not changed.\n");
    }
}

void restoreStock(const char *productName, int quantity) {
    int i;

    for (i = 0; i < MAX_PRODUCTS; i++) {
        if (strcmp(products[i].name, productName) == 0) {
            products[i].stock += quantity;
            return;
        }
    }
}

int addTransaction(const char *productName,
                   int quantity, double total) {
    Transaction *newTransaction;
    Transaction *current;

    newTransaction = malloc(sizeof(Transaction));

    if (newTransaction == NULL) {
        return 0;
    }

    newTransaction->id = nextTransactionID++;
    snprintf(newTransaction->productName,
             sizeof(newTransaction->productName),
             "%s", productName);
    newTransaction->quantity = quantity;
    newTransaction->total = total;
    newTransaction->next = NULL;

    if (transactionHead == NULL) {
        transactionHead = newTransaction;
    } else {
        current = transactionHead;

        while (current->next != NULL) {
            current = current->next;
        }

        current->next = newTransaction;
    }

    return 1;
}


/*findTransaction(), searchTransaction()*/
extern Transaction *head;
int readInt(const char *prompt);
void printTransaction(prompt Transaction *t);

Transaction *findTransaction(int id);
void searchTransaction(void);

/* Walk the linked list and return the transaction with the given ID or NULL if there is no match.*/
Transaction *findTransaction(int id) {
    Transaction *cur = head;
 
    while (cur != NULL) {
        if (cur->id == id) {
            return cur;
        }
        cur = cur->next;
    }
 
    return NULL;
}
 
/* Asks for a transaction ID, looks it up, and prints it if found. */
void searchTransaction(void) {
    int id;
    Transaction *t;
 
    if (head == NULL) {
        printf("\nNo transactions recorded yet.\n");
        return;
    }
 
    id = readInt("Enter transaction ID to search: ");
    t = findTransaction(id);
 
    if (t == NULL) {
        printf("\nTransaction #%d not found.\n", id);
    } else {
        printTransaction(t);
    }
}

Displays the summary of all recorded transactions
void displaySummary(void) {

    // Start at the first transaction in the linked list
    Transaction *current = transactionHead;

    // Initialize variables for the sales summary
    double totalSales = 0.0;
    int totalTransactions = 0;
    int totalItemsSold = 0;
    int i;

    // Traverse the linked list to calculate sales information
    while (current != NULL) {

        // Add the transaction amount to total sales
        totalSales += current->total;

        // Count the number of transactions
        totalTransactions++;

        // Add the purchased quantity to total items sold
        totalItemsSold += current->quantity;

        // Move to the next transaction
        current = current->next;
    }

    // Display the sales summary
    printf("\n========== STORE SALES SUMMARY ==========\n");

    printf("Total transactions: %d\n", totalTransactions);
    printf("Total items sold: %d\n", totalItemsSold);
    printf("Total sales: PHP %.2f\n", totalSales);

    // Display the remaining inventory of each product
    printf("\nRemaining inventory:\n");

    // Loop through all products
    for (i = 0; i < MAX_PRODUCTS; i++) {

        // Display the product name and remaining stock
        printf("%-22s %d units\n",
               products[i].name,
               products[i].stock);
    }

    // Display the closing line
    printf("=========================================\n");
}


// Deletes a transaction using its transaction ID
void deleteTransaction(void) {

    // Variable for the transaction ID entered by the user
    int id;

    // Pointer to the current transaction
    Transaction *current = transactionHead;

    // Pointer to the previous transaction
    Transaction *previous = NULL;

    // Ask the user to enter the transaction ID
    id = readInt("Enter transaction ID to delete: ");

    // Search for the transaction with the given ID
    while (current != NULL && current->id != id) {

        // Move the previous pointer to the current node
        previous = current;

        // Move to the next transaction
        current = current->next;
    }

    // Check if the transaction was not found
    if (current == NULL) {

        // Inform the user that the transaction does not exist
        printf("Transaction not found.\n");

        return;
    }

    // Display the transaction that will be deleted
    printf("\nTransaction to delete:\n");

    // Print the transaction details
    printTransaction(current);

    // Ask the user to confirm the deletion
    {
        char confirm[10];

        printf("Delete this transaction? (y/n): ");

        // Read the user's confirmation
        readLine(confirm, sizeof(confirm));

        // Cancel the deletion if the user does not enter y
        if (tolower((unsigned char)confirm[0]) != 'y') {

            // Inform the user that deletion was cancelled
            printf("Deletion cancelled.\n");

            return;
        }
    }

    // Return the sold product quantity to the inventory
    restoreStock(current->productName, current->quantity);

    // If the transaction is the first node,
    // update the transactionHead pointer
    if (previous == NULL) {

        // Set the head to the next transaction
        transactionHead = current->next;
    }

    // Otherwise, connect the previous node
    // to the next node, skipping the deleted transaction
    else {

        // Update the previous transaction's next pointer
        previous->next = current->next;
    }

    // Free the memory used by the deleted transaction
    free(current);

    // Inform the user that the transaction was deleted
    printf("Transaction deleted. Stock has been restored.\n");
}


// Frees the memory allocated for all transactions
void freeTransactions(void) {

    // Start at the first transaction
    Transaction *current = transactionHead;

    // Temporary pointer used to store the next transaction
    Transaction *next;

    // Traverse the linked list until the end
    while (current != NULL) {

        // Save the next transaction before freeing the current one
        next = current->next;

        // Release the memory allocated for the current transaction
        free(current);

        // Move to the next transaction
        current = next;
    }

    // Set the transactionHead pointer to NULL
    // because the linked list is now empty
    transactionHead = NULL;
}

// Prints a single transaction as one formatted row in the transaction table.
void printTransaction(Transaction *transaction) {
if (transaction == NULL) {
return;
}
printf("%-8d %-22s %-10d PHP %.2f\n",
transaction->id,
transaction->productName,
transaction->quantity,
transaction->total);
}


// Displays the full transaction history in a formatted table.
void viewHistory(void) {
Transaction *current = transactionHead;
printf("\n================ TRANSACTION HISTORY ================\n");
printf("%-8s %-22s %-10s %s\n",
"ID", "PRODUCT", "QUANTITY", "TOTAL");
printf("-----------------------------------------------------\n");
if (current == NULL) {
printf("No transactions recorded yet.\n");
return;
}
while (current != NULL) {
printTransaction(current);
current = current->next;
}
}

