# include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int **G, *deg, n;
int visited(int *path,int len,int v){
    for(int i=0;i<len;i++){
        if(path[i]==v) return 1;
    }
    return 0;
}

int hasEdge(int u,int v){
    for(int i=0;i<deg[u];i++){
        if(G[u][i]==v) return 1;
    }
    return 0;
}

void hamiltonian(int *path, int len){
    printf("Process %d: ",getpid());
    for(int i=0;i<len;i++){
        printf("%d ",path[i]);
    }
    printf("\n");

    if(len==n){
        if(hasEdge(path[len-1],1)){
            printf("Hamiltonian cycle found : ");
            for(int i=0;i<len;i++){
                printf("%d ",path[i]);
            }
            printf("1\n");
            exit(0);
        }
        exit(1);
    }

    int u=path[len-1];
    for(int i=0;i<deg[u];i++){
        int v=G[u][i];
        if(!visited(path,len,v)){
            pid_t pid=fork();
            if(pid==0){
                int newpath[n];
                for(int j=0;j<len;j++){
                    newpath[j]=path[j];
                }
                newpath[len]=v;
                hamiltonian(newpath,len+1);
            }else{
                int status;
                while(wait(&status)>0){
                    if(WIFEXITED(status)&& WEXITSTATUS(status)==0){
                       exit(0);
                    }
                }
            }
        }
    }

    int status;
    while(wait(&status)>0){
        if(WIFEXITED(status)&& WEXITSTATUS(status)==0){
            exit(0);
        }
    }
    exit(1);
}
int main(){
    FILE *fp = fopen("graph.txt", "r");
    if(!fp){
        printf("File not found\n");
        return 1;
    }

    fscanf(fp,"%d",&n);
    printf("Number of Nodes: %d\n",n);

    int **adj = (int **)malloc((n+1)*sizeof(int*));
    for(int i=1;i<=n;i++){
        adj[i]=(int *)malloc(n*sizeof(int));
    }

    deg =(int *)calloc(n+1,sizeof(int));

    int u,v;
    while(fscanf(fp,"%d ->",&u)==1){
        while(fscanf(fp,"%d",&v)==1){
            adj[u][deg[u]++]=v;
            if(fgetc(fp)=='\n') break;
        }
    }

    G = (int **)malloc((n+1)*sizeof(int*));
    for(int i=1;i<=n;i++){
        G[i]=(int *)malloc(deg[i]*sizeof(int));
    }
    for(int i=1;i<=n;i++){
        for(int j=0;j<deg[i];j++){
            G[i][j]=adj[i][j];
        }
    }
    fclose(fp);

    int path[n];
    path[0]=1;
    hamiltonian(path,1);
    return 0;
}