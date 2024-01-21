#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define N 4
#define prec 0.000001f
#define PMASS 1.0f
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define ABS(x) (((x) < (0)) ? (-x) : (x))
#define SOFTENING 1e-9f
#define PI 3.14159265f

typedef struct Particle_{
    double x, y;
    double vx, vy;
} Particle;

typedef struct qtnode_{
    Particle *particle;
    int which_child;
    int size;
    double x_com,y_com, total_mass, lb, rb, ub, db;
    struct qtnode_ *parent;
    struct qtnode_ *child[N];
} QTNode;


void qtnode_print(QTNode *n, int lvl);
void root_boundary(double ans[], Particle *p, int nParticles);
QTNode* create_node(QTNode *parent, int child_index, Particle *p, int nParticles);
int which_child_contains(QTNode *n, Particle *p);
int is_leaf(QTNode *n);
void qTree_insert(Particle *p,QTNode *root);
QTNode* remove_empty_nodes(QTNode *root);
void preorder_traversal(QTNode *root, int lvl);
void qTree_free(QTNode *root);
int height(QTNode *root);