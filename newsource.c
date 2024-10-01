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
    if (x->parent->left) x->parent->left->parent = x->parent; // fix 2
    x->right = x->parent;
    if (buf == 1) x->parent = x->parent->parent; // если предок, который за скобками вообще существует, то у него указатель на потомка тоже меняем
    x->right->parent = x;
    if ((buf == 1) && (x->parent->left == x->right)) x->parent->left = x; // меняем указатель на потомка у узла который за скобками (если он вообще есть)
    if ((buf == 1) && (x->parent->right == x->right)) x->parent->right = x; // меняем указатель на потомка у узла который за скобками (если он вообще есть)

    if (buf == 0) x->parent = NULL; // если узла за скобками и не было, то отчиащем указатель на предка (чтобы не получилось что splay произошел и цикл все так и работает
    return(x);
}

struct splay_tree* left_rotate(struct splay_tree* x) { // все тоже самое только левый поворот

    int buf = 3;
    if (x->parent->parent != NULL) buf = 1;
    else buf = 0;

    x->parent->right = x->left;
    if (x->parent->right) x->parent->right->parent = x->parent; // fix 2
    x->left = x->parent;
    if (buf == 1) x->parent = x->parent->parent;
    x->left->parent = x;
    if ((buf == 1) && (x->parent->left == x->left)) x->parent->left = x;
    if ((buf == 1) && (x->parent->right == x->left)) x->parent->right = x;

    if (buf == 0) x->parent = NULL;
    return(x);
}


void zig_zig(struct splay_tree* x) {

    // правый поворот 1
    struct splay_tree* parent_of_x = x->parent;
    parent_of_x = right_rotate(parent_of_x); // проверить, нужно ли здесь (и вообще в обоих поворотах) возвращать указатель, все таки мы указатель не меняли.

    // правый поворот 2
    x = parent_of_x->left;
    x = right_rotate(x);

    return;
}

void zag_zag(struct splay_tree* x) {

    // левый поворот 1
    struct splay_tree* parent_of_x = x->parent;
    parent_of_x = left_rotate(parent_of_x);

    // левый поворот 2
    x = parent_of_x->right;
    x = left_rotate(x);

    return;
}

void zig_zag(struct splay_tree* x) {

    // левый поворот 
    x = left_rotate(x);

    // правый поворот
    x = right_rotate(x);

    return;
}

void zag_zig(struct splay_tree* x) {

    // правый поворот
    x = right_rotate(x);

    // левый поворот 
    x = left_rotate(x);

    return;
}

struct splay_tree* splay(struct splay_tree* root, struct splay_tree* x) {

    while (x->parent != NULL) { // в конце концов нам нужно чтобы х стал корнем, а пока у "х" есть предок - он не корень

        if ((x->parent->parent == NULL) && (x->parent->left == x)) { // если предок узла - корень и если узел это левый потомок этого корня, то zig
            x = zig(x);
            break;
        }

        if ((x->parent->parent == NULL) && (x->parent->right == x)) { // если предок узла - корень и если узел это левый потомок этого корня, то zig
            x = zag(x);
            break;
        }

        if ((x->parent->parent != NULL) && (x->parent->left == x) && (x->parent->parent->left == x->parent)) { // если х - левый потомок и его родитель тоже левый поток, а также если родитель х - не корень, то zig-zig
            //x = zig_zig(x);
            zig_zig(x);
            continue;
        }

        if ((x->parent->parent != NULL) && (x->parent->right == x) && (x->parent->parent->right == x->parent)) { // если х - правый потомок и его родитель тоже правый потомок, а также если родитель х - не корень, то zig-zig
            //x = zag_zag(x);
            zag_zag(x);
            continue;
        }

        if ((x->parent->parent != NULL) && (x->parent->right == x) && (x->parent->parent->left == x->parent)) { // если х - правый потомок а его родитель левый потомок, а также если родитель х - не корень, то zig-zig
            //x = zig_zag(x);
            zig_zag(x);
            continue;
        }

        if ((x->parent->parent != NULL) && (x->parent->left == x) && (x->parent->parent->right == x->parent)) { // если х - левый потомок а его родитель правый потомок, а также если родитель х - не корень, то zig-zig
            //x = zag_zig(x);
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

    FILE* code = fopen("code.c", "r"); // он всегда будет с таким именем после работы функции по удалению комментов 

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

        if (!(((s >= 65) && (s <= 90)) || ((s >= 97) && (s <= 122)))) continue; // в соответсвии с таблицей ascii если символ не буква, переходим к след итерации

        //printf("%c", s);

        char* new_word = (char*)malloc(1 * sizeof(char)); // массив куда запишем слово
        int count_letters = 0; // кол-во букв в слове
        // записываем слово в новую строку
        while (((s >= 65) && (s <= 90)) || ((s >= 97) && (s <= 122))) { // пока буква
            if (s == EOF) return 0;

            count_letters++;
            if (count_letters > 1) new_word = (char*)realloc(new_word, count_letters * sizeof(char)); // это чтобы лишний раз не перевыделять память

            sch = s;
            new_word[count_letters - 1] = sch;

            s = fgetc(code);


            //printf("%c", word[count_letters - 1]);
        }
        new_word[count_letters] = '\0';
        //printf("\nnew_word - %s\n", new_word);
        if (strncmp(new_word, "size", 9999) == 0) {
            prov++;
            prov--;
        }

        // получили новую строку, ищем ее в keywords
        int gemo = find_in_keywords(new_word, keywords);

        struct splay_tree* search_ptr = NULL; // объявляем указатель для того чтобы он потом указал на найденный узел
        int current_value = 0; // аналогично

        if (gemo == 1) {// если нашли в ключевых


            // ищем в дереве
            if (root) {// аналогично (root != NULL) - если дерево не пустое, то ищем

                current_value = get_value(new_word);
                search_ptr = search_in_tree(root, current_value, NULL); //радославов после поиска не делает splay, а также делает ставку после поиска как я (если поиск неудачно - вставляем)



                if (search_ptr) { // если нашли в дереве слово

                    //amount + 1
                    search_ptr->amount = search_ptr->amount + 1;

                }
                else { // не нашли в дереве слово

                    root = insert_in_tree(root, new_word); // тут по идее незачем возвращать указатель на корень, но по другому не работает почему то

                    // splay - сделаю в функции
                }
            }
            else { // а если дерево пустое переходим в функцию вставки и там дейсвтуем по сценарию пустого дерева)

            //printf("\nroot = NULL, inserting new node\n");
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

struct splay_tree* insert_in_tree(struct splay_tree* root, char new_word[]) { // на выход - корень дерева после вставки

    // если еще нет узлов, создаем первый
    if (!root) { // аналогично (root == NULL)
        root = (struct splay_tree*)malloc(1 * sizeof(struct splay_tree)); // теперь указатель указывает на конкретную стрктуру типа splaytree, прото еще не заполненную

        strncpy(root->word, new_word, 20); // strncpy потому что strcpy типо плохо работает как то, ее никто не юзает
        root->amount = 1;
        root->parent = NULL;
        root->left = NULL;
        root->right = NULL;
        root->value = get_value(new_word);

        //printf("\nvalue = %d\n\n", root->value); // проверка

        return(root);
    }
    else { // если уже есть (дерево не пустое), содзаем новый

        struct splay_tree* new_node = (struct splay_tree*)malloc(1 * sizeof(struct splay_tree));

        strncpy(new_node->word, new_word, 20);
        new_node->amount = 1;
        new_node->parent = NULL;
        new_node->left = NULL;
        new_node->right = NULL;
        new_node->value = get_value(new_word);

        // объявляем указатель на узел, на которым мы сейчас когда идем по дереву и ищем куда вставить новый узел
        struct splay_tree* current_node = NULL;

        current_node = root; //очевидно первый узел это корень

        while (1) {
            if (new_node->value > current_node->value) { // начинаем от корня идти вниз и взависимости от значения двигаемся к правому потомку или левому

                if (!current_node->right) { // обращаемся через указатель к узлу - если правый потомок пустой, то все, просто вставляем
                    current_node->right = new_node; // теперь текущий узел указывает на новый узел, потому что это одновременно и указатель и сам элемент
                    new_node = NULL;

                    current_node->right->parent = current_node;// и также заполняем его указатель на потомка, теперь он указывает на потомка
                    current_node = current_node->right; // это чтобы потом передать верный узел в функцию сплей

                    break;
                }
                else { // а если там куда нам надо кто то есть, то просто идем далее к след итерации цикла с этим узлом
                    current_node = current_node->right;
                    continue;
                }
                //display_tree(root, 0);
                //printf("\n\n");
            }
            if (new_node->value < current_node->value) {

                if (current_node->left == NULL) { // если левый потомок пустой, то аналогично просто вставляем
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

                //display_tree(root, 0);
                //printf("\n\n");

            }
        }
        // splay
        root = splay(root, current_node);

        return(root);
    }
}

struct splay_tree* search_in_tree(struct splay_tree* current_node, int x, struct splay_tree* result) { // на вход дается id элемента, на выходе указатель пустой - не нашли, указатель не пустой - указывает на то что нашли
    // то есть у этой функции нет цели что то создать. нужно просто вернуть указатель на существующий уже жлемент, или вообще ничего не возаращать
    // также на вход дается struct splay_tree* result = NULL

    struct splay_tree* searching_node = current_node;

    if (x == searching_node->value) return(searching_node); // значит нашли

    // если значение больше текущего, идем к правому потомку (если он вообще существует)
    //if ((x > searching_node->value) && (searching_node->right != NULL)) result = search_in_tree(searching_node->right, x);
    if ((x > searching_node->value) && (searching_node->right)) result = search_in_tree(searching_node->right, x, NULL);

    // аналогично к левому
    if ((x < searching_node->value) && (searching_node->left)) result = search_in_tree(searching_node->left, x, NULL);

    if (result) return(result); // чтобы сразу отовсюду выйти если нашли

    return(NULL); // выходим с пустым указателем (т.е. "не нашли) если больше нет потомков и если мы прошли всех потомков и ничего не нашли.

}

void display_tree(struct splay_tree* root, int node_number) { // сначала на вход дается корень и 0, потом уже узел текущий и его номер
    // берем узел и запускаем функцию рекурсивно на правого и левого потомка

    node_number++;
    printf("\n level number %d: %s, val %d, am = %d", node_number, root->word, root->value, root->amount);
    printf("   ||   left - %1d, right - %1d, parent - %1d\n", root->left, root->right, root->parent);
    if (root->parent) printf("prov display, parent: val = %d\n", root->parent->value);

    if (root->left) {
        printf("\nleft one\n");
        display_tree(root->left, node_number);
    }

    if (root->right) {
        printf("\nright one\n");
        display_tree(root->right, node_number);
    }

    // выходим если больше нет потомков или уже прошлись по всем у конкретного узла
    return;
}

void display_answer(struct splay_tree* root) {

    printf("\n  %s - %d", root->word, root->amount);

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

    for (int ii = 0; ii < 34; ii++) { // идем по каждому слову
        temp = 0;
        for (int i = 0; i <= count; i++) { // идем по буквам слова
            // сравниваем iтую букву
            //printf(" %c", keywords[ii][i]);
            if (keywords[ii][i] != word[i]) {
                temp = 1;
                break; // если сравнение провалилось - идем к след слову
            }
            // если сравнение успешно, идем к след итерации
            // если сравнение успешно и i = count_of_letters, возращаем 1
            else if ((i == count) && (i != 0)) return 1; // i!=0 это фикс бага


        }
        if (temp == 1) continue;
    }
    return 0; // если пришли сюда, значит нигде не нашли слова, значит это слово - не ключевое
}