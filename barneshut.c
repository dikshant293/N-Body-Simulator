#include "QTree.h"

long long count = 0;                // to keep count of total number of force calculations

void ran_init(Particle *p, int nParticles){
    float size = 10.0f;
    for (int i = 0; i < nParticles; i++){
        p[i].x = size * (rand() / (double)RAND_MAX) - (size/2.0f);
        p[i].y = size * (rand() / (double)RAND_MAX) - (size/2.0f);
        p[i].vx= size * (rand() / (double)RAND_MAX) - (size/2.0f);
        p[i].vy= size * (rand() / (double)RAND_MAX) - (size/2.0f);
    }
}

void lattice_init(Particle *p, int nParticles){
    int side =sqrt(nParticles);
    if(side*side!=nParticles) side++;   // partially filled lattice if nParticles is not a perfect square
    for(int i=0;i<side;i++){
        for(int j=0;j<side;j++){
            if(i*side+j>=nParticles) break;
            p[i*side+j].x=i+1;
            p[i*side+j].y=j+1;
            p[i*side+j].vx=0.0f;
            p[i*side+j].vy=0.0f;
        }
    }
}

void circle_init(Particle *p, int nParticles){
    double rad_per_particle=2.0f*PI/nParticles;
    double radius = 1.0f/sin(rad_per_particle);
    for(int i=0;i<nParticles;i++){
        p[i].x=radius * cos(i*rad_per_particle);
        p[i].y=radius * sin(i*rad_per_particle);
        p[i].vx=0.0f;
        p[i].vy=0.0f;
    }
}

void calc_force_on_particle(QTNode *n,QTNode *root, double F[], double *theta){
    if(n==root || !root) return;
    double dx = root->particle->x - n->particle->x;
    double dy = root->particle->y - n->particle->y;
    double distSqr = dx*dx + dy*dy + SOFTENING;
    if((root->rb-root->lb)/sqrt(distSqr)<*theta || root->size==1){  // if check satisfied or leaf node (size==1)
        double invDist = 1.0f / sqrt(distSqr);
        double invDist3 = invDist * invDist * invDist;
        F[0] += n->total_mass * root->total_mass * dx * invDist3; 
        F[1] += n->total_mass * root->total_mass * dy * invDist3;
        count++;
    }
    else{
        for(int i=0;i<N;i++) calc_force_on_particle(n,root->child[i],F,theta);
    }
}

void update_particles(QTNode *n,QTNode *root, const double *dt, double *theta){
    if(!n || !root) return;
    if(n->size==1){         // particle found
        double F[2]={0.0f, 0.0f};
        calc_force_on_particle(n,root,F,theta); // calculate force on it from others
        n->particle->vx+=(*dt)*F[0];            // update velocities
        n->particle->vy+=(*dt)*F[1];
    }
    for(int i=0;i<N;i++) update_particles(n->child[i],root,dt,theta);
}

void simulate_timesteps(Particle *p, int nParticles, int nIters, double theta, const double *dt, FILE *datafile){
    for(int iter = 0; iter<nIters; iter++){
        QTNode *root = create_node(NULL,-1,p,nParticles);
        for (int i = 0; i < nParticles; ++i){
            qTree_insert(p+i,root);
        }
        root = remove_empty_nodes(root);
        update_particles(root,root,dt,&theta);
        
        for (int i = 0 ; i < nParticles; i++) {  /* compute new position */
            p[i].x += p[i].vx*(*dt);
            p[i].y += p[i].vy*(*dt);
        }
        if(datafile) for(int i = 0; i < nParticles; ++i){
            fprintf(datafile, "%f %f \t %f %f\n",
                    p[i].x, p[i].y,
                    p[i].vx, p[i].vy);
        }
        qTree_free(root);   
    }
}

void run_simulation(Particle *p, int nParticles, int nIters, double theta,int init_type, const double *dt){
    FILE *datafile = NULL;
    datafile = fopen("particles.dat", "w");
    fprintf(datafile, "%d %d %f %d\n", nParticles, nIters, theta, init_type);
    for (int i = 0; i < nParticles; ++i){
        fprintf(datafile, "%f %f \t %f %f\n",
                p[i].x, p[i].y,
                p[i].vx, p[i].vy);
    }
    simulate_timesteps(p,nParticles,nIters,theta,dt,datafile);
    fclose(datafile);
}

void run_tests(int nParticles, int nIters, const double *dt){
    Particle *p_orig = malloc(nParticles * sizeof(Particle));
    Particle *p_final_theta0 = malloc(nParticles * sizeof(Particle));
    ran_init(p_orig,nParticles);
    double theta_list[] = {0.0, 0.01, 0.05, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1, 1.25, 1.5, 1.75, 2, 4};
    int list_size = sizeof(theta_list)/sizeof(double);
    
    printf("number of particles = %d\nnumber of iterations = %d\n\n",nParticles,nIters);
    for(int k=0;k<list_size;k++){
        Particle *p = malloc(nParticles * sizeof(Particle));
        for(int i=0;i<nParticles;i++){
            p[i].x  =p_orig[i].x;
            p[i].y  =p_orig[i].y;
            p[i].vx =p_orig[i].vx;
            p[i].vy =p_orig[i].vy;
        }
        count=0;
        simulate_timesteps(p,nParticles,nIters,theta_list[k],dt,NULL);
        if(k==0){       // for naive algorith theta = 0.0
            for(int i=0;i<nParticles;i++){
                p_final_theta0[i].x  =p[i].x;
                p_final_theta0[i].y  =p[i].y;
                p_final_theta0[i].vx =p[i].vx;
                p_final_theta0[i].vy =p[i].vy;
            }
            double reduction = (nIters*nParticles*(nParticles-1)-count)*((double)100)/(double)(nIters*nParticles*(nParticles-1));
            printf("For theta = %.2f:\nnaive nbody: #force calculations = %d\nbarnes hut: #force calculations = %lld\nPercentage reduction in claculations = %.2f %%\n\n",
                    theta_list[k],nIters*nParticles*(nParticles-1),count,reduction);
        }
        else{           // for theta != 0, compare result to naive result
            double rmse_x=0.0, rmse_y=0.0, rmse_vx=0.0, rmse_vy=0.0;
            for(int i=0;i<nParticles;i++){
                rmse_x  += pow(p_final_theta0[i].x  - p[i].x,2.0);
                rmse_y  += pow(p_final_theta0[i].y  - p[i].y,2.0);
                rmse_vx += pow(p_final_theta0[i].vx - p[i].vx,2.0);
                rmse_vy += pow(p_final_theta0[i].vy - p[i].vy,2.0);
            }
            rmse_x  = sqrt(rmse_x/(double)nParticles);
            rmse_y  = sqrt(rmse_y/(double)nParticles);
            rmse_vx = sqrt(rmse_vx/(double)nParticles);
            rmse_vy = sqrt(rmse_vy/(double)nParticles);
            double reduction = (nIters*nParticles*(nParticles-1)-count)*((double)100)/(double)(nIters*nParticles*(nParticles-1));
            printf("RMSE Errors for theta = %.2f:\nx  = %f\ty  = %f\nvx = %f\tvy = %f\n",
                    theta_list[k],rmse_x,rmse_y,rmse_vx,rmse_vy);
            printf("naive nbody: #force calculations = %d\nbarnes hut: #force calculations = %lld\nPercentage reduction in claculations = %.2f %%\n\n",
                    nIters*nParticles*(nParticles-1),count,reduction);
        }
        count=0;
        free(p);
    }
    free(p_final_theta0);
    free(p_orig);
}

int main(const int argc, const char **argv)
{
    int nParticles = 300; /* number of particles */
    int init_type = 200;
    int is_test = 0;
    const double dt = 0.01f; /* time step   */
    int nIters = 200;       /* number of steps in simulation */
    double theta = 0.5f;

    if (argc > 1)
        nParticles = atoi(argv[1]);
    if (argc > 2)
        nIters = atoi(argv[2]);
    if (argc > 3)
        theta = atof(argv[3]);
    if (argc > 4)
        init_type = atoi(argv[4]);
    if (argc > 5)
        is_test = atoi(argv[5]);
    
    if(init_type!=1 && init_type!=2 && init_type!=3){
        printf("uexpected init func parameter: expected 1, 2 or 3....found %d\nexiting..\n",init_type);
        exit(1);
    }

    if(is_test!=0 && is_test!=1){
        printf("uexpected test func parameter: expected 0 or 1....found %d\nexiting..\n",is_test);
        exit(1);
    }

    if(!is_test){
        Particle *p = malloc(nParticles * sizeof(Particle));
        if(init_type==1) ran_init(p,nParticles);
        else if(init_type==2) lattice_init(p,nParticles);
        else if(init_type==3) circle_init(p,nParticles);

        run_simulation(p,nParticles,nIters,theta,init_type,&dt);
        printf("naive nbody force calcs = %d\nbarnes hut force calcs = %lld\n",nIters*nParticles*(nParticles-1),count);
        free(p);
    }
    else{
        run_tests(nParticles,nIters,&dt);
    }
    return 0;
}