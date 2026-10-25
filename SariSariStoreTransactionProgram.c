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
