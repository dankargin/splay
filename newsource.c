#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "del_comm.h"

extern int delete_comments(char* startname);
struct splay_tree {
    unsigned char word[20];
    int value;
    int amount;
    struct splay_tree* left, * right, * parent;
};

struct splay_tree* root;
void display_answer(struct splay_tree* root);
int find_in_keywords(char* word, char keywords[34][20]);
int get_value(char the_word[]);
struct splay_tree* search_in_tree(struct splay_tree* root, int x, struct splay_tree* result);
struct splay_tree* insert_in_tree(struct splay_tree* root, char new_word[]);
void display_tree(struct splay_tree* root, int node_number);
struct splay_tree* zig(struct splay_tree* x) {

    x->parent->left = x->right;
    if (x->right) x->right->parent = x->parent;
    x->right = x->parent;
    x->parent->parent = x;
    x->parent = NULL;

    return(x);
}

struct splay_tree* zag(struct splay_tree* x) {

    x->parent->right = x->left;
    if (x->left) x->left->parent = x->parent;
    x->left = x->parent;
    x->parent->parent = x;
    x->parent = NULL;

    return(x);
}

struct splay_tree* right_rotate(struct splay_tree* x) {

    int buf = 3;
    if (x->parent->parent != NULL) buf = 1;
    else buf = 0;

    x->parent->left = x->right;
    if (x->parent->left) x->parent->left->parent = x->parent;
    x->right = x->parent;
    if (buf == 1) x->parent = x->parent->parent;
    x->right->parent = x;
    if ((buf == 1) && (x->parent->left == x->right)) x->parent->left = x;
    if ((buf == 1) && (x->parent->right == x->right)) x->parent->right = x;

    if (buf == 0) x->parent = NULL;
    return(x);
}

struct splay_tree* left_rotate(struct splay_tree* x) {

    int buf = 3;
    if (x->parent->parent != NULL) buf = 1;
    else buf = 0;

    x->parent->right = x->left;
    if (x->parent->right) x->parent->right->parent = x->parent;
    x->left = x->parent;
    if (buf == 1) x->parent = x->parent->parent;
    x->left->parent = x;
    if ((buf == 1) && (x->parent->left == x->left)) x->parent->left = x;
    if ((buf == 1) && (x->parent->right == x->left)) x->parent->right = x;

    if (buf == 0) x->parent = NULL;
    return(x);
}


void zig_zig(struct splay_tree* x) {

    struct splay_tree* parent_of_x = x->parent;
    parent_of_x = right_rotate(parent_of_x);

    x = parent_of_x->left;
    x = right_rotate(x);

    return;
}

void zag_zag(struct splay_tree* x) {

    struct splay_tree* parent_of_x = x->parent;
    parent_of_x = left_rotate(parent_of_x);

    x = parent_of_x->right;
    x = left_rotate(x);

    return;
}

void zig_zag(struct splay_tree* x) {

    x = left_rotate(x);
    x = right_rotate(x);

    return;
}

void zag_zig(struct splay_tree* x) {

    x = right_rotate(x);
    x = left_rotate(x);

    return;
}

struct splay_tree* splay(struct splay_tree* root, struct splay_tree* x) {

    while (x->parent != NULL) {

        if ((x->parent->parent == NULL) && (x->parent->left == x)) {
            x = zig(x);
            break;
        }
        if ((x->parent->parent == NULL) && (x->parent->right == x)) {
            x = zag(x);
            break;
        }
        if ((x->parent->parent != NULL) && (x->parent->left == x) && (x->parent->parent->left == x->parent)) {

            zig_zig(x);
            continue;
        }
        if ((x->parent->parent != NULL) && (x->parent->right == x) && (x->parent->parent->right == x->parent)) {

            zag_zag(x);
            continue;
        }

        if ((x->parent->parent != NULL) && (x->parent->right == x) && (x->parent->parent->left == x->parent)) {

            zig_zag(x);
            continue;
        }

        if ((x->parent->parent != NULL) && (x->parent->left == x) && (x->parent->parent->right == x->parent)) {

            zag_zig(x);
            continue;
        }
    }
    root = x;
    return(root);
}

int main() {
    unsigned __int64 STARTTIME, ENDTIME, time;
    unsigned __int64 __rdtsc();

    char keywords[34][20] = { "auto", "break", "case", "char", "const", "continue", "default", "do", "double", "else", "enum", "extern", "float", "for", "goto" ,"if","inline", "int", "long", "register", "restrict", "return", "short", "signed", "sizeof", "static" ,"struct","switch", "typedef", "union", "unsigned", "void", "volatile", "while" };
    int prov = 0;

    char name_of_file[20];
    printf("\nenter name of file: ");
    gets(name_of_file);

    delete_comments(name_of_file);

    FILE* code = fopen("code.c", "r");

    if (!code) {
        printf("File opening failed\n");
        return 0;
    }

    root = NULL;
    int s = 0;
    char sch = 0;
    STARTTIME = __rdtsc();

    while (s != EOF) {

        s = fgetc(code);

        if (!(((s >= 65) && (s <= 90)) || ((s >= 97) && (s <= 122)))) continue;

        char* new_word = (char*)malloc(1 * sizeof(char));
        int count_letters = 0;

        while (((s >= 65) && (s <= 90)) || ((s >= 97) && (s <= 122))) {
            if (s == EOF) return 0;

            count_letters++;
            if (count_letters > 1) new_word = (char*)realloc(new_word, count_letters * sizeof(char));

            sch = s;
            new_word[count_letters - 1] = sch;

            s = fgetc(code);

        }
        new_word[count_letters] = '\0';

        if (strncmp(new_word, "size", 9999) == 0) {
            prov++;
            prov--;
        }
        int gemo = find_in_keywords(new_word, keywords);

        struct splay_tree* search_ptr = NULL;
        int current_value = 0;

        if (gemo == 1) {

            if (root) {
                current_value = get_value(new_word);
                search_ptr = search_in_tree(root, current_value, NULL);
                if (search_ptr) {

                    search_ptr->amount = search_ptr->amount + 1;
                }
                else {
                    root = insert_in_tree(root, new_word);
                }
            }
            else {
                root = insert_in_tree(root, new_word);
            }
        }
    }
    ENDTIME = __rdtsc();
    time = ENDTIME - STARTTIME;
    printf("\n\ntime - %I64d tacts\n", time);
    display_answer(root);
    printf("\n\n");
    fclose(code);
    return 0;
}

struct splay_tree* insert_in_tree(struct splay_tree* root, char new_word[]) {

    if (!root) {
        root = (struct splay_tree*)malloc(1 * sizeof(struct splay_tree));

        strncpy(root->word, new_word, 20);
        root->amount = 1;
        root->parent = NULL;
        root->left = NULL;
        root->right = NULL;
        root->value = get_value(new_word);

        return(root);
    }
    else {
        struct splay_tree* new_node = (struct splay_tree*)malloc(1 * sizeof(struct splay_tree));

        strncpy(new_node->word, new_word, 20);
        new_node->amount = 1;
        new_node->parent = NULL;
        new_node->left = NULL;
        new_node->right = NULL;
        new_node->value = get_value(new_word);

        struct splay_tree* current_node = NULL;

        current_node = root;

        while (1) {
            if (new_node->value > current_node->value) {

                if (!current_node->right) {
                    current_node->right = new_node;
                    new_node = NULL;

                    current_node->right->parent = current_node;
                    current_node = current_node->right;

                    break;
                }
                else {
                    current_node = current_node->right;
                    continue;
                }

            }
            if (new_node->value < current_node->value) {

                if (current_node->left == NULL) {
                    current_node->left = new_node;
                    new_node = NULL;

                    current_node->left->parent = current_node;
                    current_node = current_node->left;

                    break;
                }
                else {
                    current_node = current_node->left;
                    continue;
                }
            }
        }
        root = splay(root, current_node);
        return(root);
    }
}

struct splay_tree* search_in_tree(struct splay_tree* current_node, int x, struct splay_tree* result) {

    struct splay_tree* searching_node = current_node;
    if (x == searching_node->value) return(searching_node);
    if ((x > searching_node->value) && (searching_node->right)) result = search_in_tree(searching_node->right, x, NULL);
    if ((x < searching_node->value) && (searching_node->left)) result = search_in_tree(searching_node->left, x, NULL);
    if (result) return(result);

    return(NULL);
}

void display_tree(struct splay_tree* root, int node_number) {
    
    node_number++;
    printf("\n level number %d: %s, val %d, am = %d", node_number, root->word, root->value, root->amount);
    printf("   ||   left - %1d, right - %1d, parent - %1d\n", root->left, root->right, root->parent);
    if (root->parent) printf("prov display, parent: val = %d\n", root->parent->value);
    if (root->left) {
        printf("\nleft one\n");
        display_tree(root->left, node_number);
    }
    if (root->right) {
        printf("\nright one\n");
        display_tree(root->right, node_number);
    }
    return;
}

void display_answer(struct splay_tree* root) {

    printf("\n  %s - %d", root->word, root->amount);
    if (root->left)
        display_answer(root->left);

    if (root->right)
        display_answer(root->right);
    return;
}

int get_value(char the_word[]) {

    int new_value = 0;
    int i = 0;
    while (the_word[i] != '\0') {
        new_value += (int)the_word[i];
        i++;
    }
    return(new_value);
}


int find_in_keywords(char* word, char keywords[34][20]) {

    int count = strlen(word);
    int temp;

    for (int ii = 0; ii < 34; ii++) {
        temp = 0;
        for (int i = 0; i <= count; i++) {

            if (keywords[ii][i] != word[i]) {
                temp = 1;
                break;
            }
            else if ((i == count) && (i != 0)) return 1;
        }
        if (temp == 1) continue;
    }
    return 0;
}
