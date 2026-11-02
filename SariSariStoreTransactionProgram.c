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
