#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define ALIVE 1
#define DEAD 0
#define BORDER 2

#define MAXDELTA 1
#define MINDELTA -1
#define DIMx 16 /*CAN'T BE 1*/
#define DIMy 16 /*CAN'T BE 1*/

#define MAXTIME 1000
#define LOOPCACHE 16

#define true 1
#define false 0

int main_table[DIMx + 2][DIMy + 2];
char compressedFooter[16], prevTable [LOOPCACHE] [DIMx*DIMy*4 + 1 + 16];

int importfirsttable (){
    int xi,yi;
    FILE *fp_table;
    if((fp_table=fopen("C:\\Users\\augux\\CLionProjects\\GiocoDellaVita\\Xcel\\template.csv", "r"))==NULL) {
        return 1;
    }
    for (xi=1;xi<=DIMx;xi++) {
        for (yi = 1; yi <= DIMy; yi++) {
            fscanf(fp_table, "%d;", &main_table[xi][yi]);
        }
    }
    //set Borders
    for (yi=0;yi<DIMy+2;yi++) {
        main_table[0][yi]= BORDER;
        main_table[DIMx+1][yi]= BORDER;
    }
    for (xi=0;xi<DIMx+2;xi++){
        main_table[xi][0]= BORDER;
        main_table[xi][DIMy+1]=BORDER;
    }

    return 0;
}

int randomgenerator(){
    srand(time(NULL)+clock());
    int randomnum;
    int xi,yi;
    for (xi=1;xi<=DIMx;xi++) {
        for (yi = 1; yi <= DIMy; yi++) {
            randomnum= (rand() % 2);
            main_table[xi][yi]= randomnum;
        }
    }
    //set Borders
    for (yi=0;yi<DIMy+2;yi++) {
        main_table[0][yi]= BORDER;
        main_table[DIMx+1][yi]= BORDER;
    }
    for (xi=0;xi<DIMx+2;xi++){
        main_table[xi][0]= BORDER;
        main_table[xi][DIMy+1]=BORDER;
    }
    return 0;

}

int tablecompression(char input[]){
    int xc,yc;
    int i;
    int val,rep=0,check=true;
    int last_pos=0;
    char string[DIMx*DIMy*4 + 1 + 16];

    val=main_table[1][1];
    for (i=0;i<(DIMx*DIMy*4 + 1);i++){
        sprintf(&string[i],"\0");
    }
    for (xc=1; xc<=DIMx;xc++){
        check=true;
        for (yc=1;yc<=DIMy;yc++){
            if (val==main_table[xc][yc]){
                rep++;
            } else {
                sprintf(&string[last_pos],"%dx%d:",val,rep);
                last_pos=strlen(string);
                val=main_table[xc][yc];
                rep=1;
                check=false;
            }
        }
        //printf("%s\n",string);
    }
    if (check==true){
        sprintf(&string[last_pos],"%dx%d:",val,rep);
    }
    strcat(string,compressedFooter);
    strcpy(input,string);
    return 0;
}

void allzero (char output[]){
    sprintf(output,"0x%d:",(DIMx*DIMy));
    strcat(output,compressedFooter);
}

int checkInfinityLoop(char *input1, char *input2){
    if (strcmp(input1,input2)==0){
        printf("Short Loop, stopping the program");
        return 1;
    }
    return 0;
}
int checkLongLoop(char *input1){
    int i;
    for (i=1;i<LOOPCACHE;i++){
        if (strcmp(input1,prevTable[i])==0){
            return 1;
        }
    }
    return 0;
}


int main() {
    int alt_table[DIMx + 2][DIMy + 2];
    int generation=0,x,y;
    int deltax, deltay;
    int neighs;
    int tab;
    char decision;
    char compressedTable [DIMx*DIMy*4 + 1 + 16], allzeroes[DIMx*DIMy*4 + 1 + 16];
    sprintf(compressedFooter,"(w%dh%d)",DIMx,DIMy);
    allzero(allzeroes);
    //table decision
    printf("write L to load a preset table, G to generate one: ");
    scanf("%c", &decision);
    if (decision == 'L') {
        if (importfirsttable() != 0) {
            printf("ERROR WHILE IMPORTING TABLE\n");
            return 1;
        }
    } else if (decision=='G'){
        if (randomgenerator()!=0){
            return 1;
        }
    }
    //copying main table to alt table
    for (x=0;x<DIMx+2;x++){
        for (y=0;y<DIMy+2;y++){
            alt_table[x][y]= main_table[x][y];
        }
    }
    tablecompression(compressedTable);
    //main algorithm
        for (generation = 0; generation < MAXTIME; generation++) {
            //checks if it's the case to end the thing there
            if(strcmp(compressedTable,allzeroes)==0){
                return 1;
            }
            /*system("cls");*/
            for (x = 1; x <= DIMx; x++) {
                for (y = 1; y <= DIMy; y++) {
                    neighs = 0;
                    for (deltax = MINDELTA; deltax <= MAXDELTA; deltax++) {
                        for (deltay = MINDELTA; deltay <= MAXDELTA; deltay++) {
                            if (deltay != 0 || deltax != 0) {
                                if (main_table[deltax + x][deltay + y] == ALIVE) {
                                    neighs++;
                                }
                            }
                        }
                    }
                    switch (main_table[x][y]) {
                        case ALIVE:
                            if (neighs < 2 || neighs > 3) {
                                alt_table[x][y] = DEAD;
                            } else {
                                alt_table[x][y] = ALIVE;
                            }
                            break;
                        case DEAD:
                            if (neighs == 3) {
                                alt_table[x][y] = ALIVE;
                            } else {
                                alt_table[x][y] = DEAD;
                            }
                            break;
                        default:
                            printf("THE TABLE WASN'T FORMATTED PROPERLY");
                            return 1;
                    }
                }
            }
            for (tab=LOOPCACHE-1;tab>0;tab--){
               strcpy(prevTable[tab],prevTable[tab-1]);
            }
            strcpy(prevTable[0],compressedTable);
            tablecompression(compressedTable);
            printf("\nGeneration %d\n", generation);
            for (x = 0; x < DIMx + 2; x++) {
                for (y = 0; y < DIMy + 2; y++) {
                    printf("%d\t", main_table[x][y]);
                    main_table[x][y] = alt_table[x][y];
                }
                printf("\n");
            }
            printf("%s\n", compressedTable);
            if (generation!=0 && checkInfinityLoop(compressedTable, prevTable[0])!=0){
                return 1;
            } else if ( generation>=LOOPCACHE &&
                    (generation%LOOPCACHE)==0) {
                if ( checkLongLoop(compressedTable)!=0) {
                    printf("Long Loop, stopping the program");
                    return 1;
                }
            }

        }
    return 0;
}