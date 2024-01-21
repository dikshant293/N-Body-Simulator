#include "QTree.h"

void qtnode_print(QTNode *n, int lvl){
    if(!n) return;
    printf("lvl = %d\n",lvl);
    if(n->particle) printf("p.x = %f, p.y = %f\n",n->particle->x,n->particle->y);
    printf("which_child = %d, size = %d\nxcom = %f, ycom = %f, tmass = %f\nlb = %f, rb = %f, db = %f, ub = %f\n\n",
            n->which_child,n->size,
            n->x_com,n->y_com,n->total_mass,
            n->lb,n->rb,n->db,n->ub);
}

void root_boundary(double ans[], Particle *p, int nParticles){
    /* ans = {xmin, xmax, ymin, ymax} */
    ans[0]= p[0].x;     ans[1]= p[0].x;
    ans[2]= p[0].y;     ans[3]= p[0].y;
    for(int i=1;i<nParticles;i++){
        ans[0] = MIN(ans[0],p[i].x);
        ans[1] = MAX(ans[1],p[i].x);
        ans[2] = MIN(ans[2],p[i].y);
        ans[3] = MAX(ans[3],p[i].y);
    }
    double xdiff=ans[1]-ans[0], ydiff=ans[3]-ans[2];
    if(xdiff>ydiff) ans[3]=ans[2]+xdiff;
    else if (xdiff<ydiff) ans[1]=ans[0]+ydiff;
}

QTNode* create_node(QTNode *parent, int child_index, Particle *p, int nParticles){
    /* Particle *p, int nParticles only needed when root node is made ,i.e., parent==NULL */
    QTNode *new_node = malloc(sizeof(QTNode));
    new_node->parent = parent;
    new_node->which_child=child_index;
    for(int i=0;i<N;i++) new_node->child[i]=NULL;
    new_node->particle=NULL;
    new_node->size=0;
    new_node->x_com=0.0;
    new_node->y_com=0.0;
    new_node->total_mass=0.0;
    if(parent==NULL){               // inserting into empty quad tree
        double boundary[N];
        root_boundary(boundary,p,nParticles);
        new_node->lb=boundary[0];
        new_node->rb=boundary[1]+prec;
        new_node->db=boundary[2];
        new_node->ub=boundary[3]+prec;
    }
    else{
        double vb = (parent->lb+parent->rb)/2.0f;
        double hb = (parent->db+parent->ub)/2.0f;
        if(child_index==0){         // north west region
            new_node->lb=parent->lb;
            new_node->rb=vb;
            new_node->db=hb;
            new_node->ub=parent->ub;
        }
        else if(child_index==1){    // north east region
            new_node->lb=vb;
            new_node->rb=parent->rb;
            new_node->db=hb;
            new_node->ub=parent->ub;
        }
        else if(child_index==2){    // south west region
            new_node->lb=parent->lb;
            new_node->rb=vb;
            new_node->db=parent->db;
            new_node->ub=hb;
        }
        else if(child_index==3){    // south east region
            new_node->lb=vb;
            new_node->rb=parent->rb;
            new_node->db=parent->db;
            new_node->ub=hb;
        }
    }
    return new_node;
}

int which_child_contains(QTNode *n, Particle *p){
    for(int i=0;i<N;i++){   // lb,db are inclusive whereas rb,ub are exculsive
        if( p->x >= n->child[i]->lb && 
            p->x < n->child[i]->rb && 
            p->y >= n->child[i]->db &&
            p->y < n->child[i]->ub
            ){
                return i;
        }
    }
    // if child containing the element not found
    printf("...............which child failed\n");
    qtnode_print(n,-1);
    printf("%f %f\n\n",p->x,p->y);
    printf("%f %f\n",n->ub,1e-6*((int)(1e6*n->ub)+1));
    for(int i=0;i<N;i++){
        printf("i = %d\n",i);
        qtnode_print(n->child[i],-1);
    }
    exit(1);
    return -1;
}

int is_leaf(QTNode *n){
    return (!n->child[0] && !n->child[1] && !n->child[2] && !n->child[3]);
}

void qTree_insert(Particle *p,QTNode *root){
    if(root->size==0){      // empty valid node found, insert here
        root->particle=p;
    }
    else{ 
        if(root->size==1){  // only one particle in node
            for(int i=0;i<N;i++) root->child[i] = create_node(root,i,NULL,-1);
            qTree_insert(root->particle,root->child[which_child_contains(root,root->particle)]);
            qTree_insert(p,root->child[which_child_contains(root,p)]);
        }
        else{               // go into the appropriate subtree
            qTree_insert(p,root->child[which_child_contains(root,p)]);
        }
    }
    // update number of particles, Center of Mass and total mass of node
    root->size+=1;
    root->x_com = ((root->x_com*root->total_mass)+PMASS*p->x)/(root->total_mass+PMASS);
    root->y_com = ((root->y_com*root->total_mass)+PMASS*p->y)/(root->total_mass+PMASS);
    root->total_mass+=PMASS;
}

QTNode* remove_empty_nodes(QTNode *root){
    if(!root) return root;
    if(root->size==0){  // remove node if it doesn't contain any particle
        free(root);
        return NULL;
    }
    for(int i=0;i<N;i++) root->child[i] = remove_empty_nodes(root->child[i]);
    return root;
}

void preorder_traversal(QTNode *root, int lvl){
    if(!root) return;
    for(int i=0;i<N;i++) preorder_traversal(root->child[i],lvl+1);
}

void qTree_free(QTNode *root){
    if(!root) return;
    for(int i=0;i<N;i++) qTree_free(root->child[i]);
    free(root);
}

int height(QTNode *root){
    if(!root) return 0;
    int ans = 0;
    for(int i=0;i<N;i++) ans = MAX(ans,height(root->child[i]));
    return 1+ans;
}
