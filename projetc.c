#include <stdio.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include <ctype.h>
#define N 1000
#include <stdio.h>
//all the list structures used in this code are simply linked lists.

//these 3 first functions are gonna be used to print ,the distance (in the bonus), using colors
void red () {
  printf("\033[1;31m");
}

void yellow() {
  printf("\033[1;33m");
}

void reset () {
  printf("\033[0m");
}
void Transform(FILE* source, FILE* interm, FILE* interw){
    char c;
    char* C_key_words[N] = {"break", "case", "char", "const", "continue", "default", "do",
    "double", "else", "enum", "extern", "float", "for", "if", "int", "long", "return", "short", "sizeof", "static", "struct",
    "switch", "typedef", "union", "unsigned", "void", "while"};
    // In the following loop we will detect characters by using their decimal value in the ASCII table
    while((c = fgetc(source)) != EOF){ //we read the file character by character
        if(c == 32){}// We detect spaces and ignore them
        else if(c == 47){ //We start by dectecting comments
            c = fgetc(source);
            if(c == 47){// If there is two slashs it's a comment so we go to the end of the line
                while(c != 10 && c != EOF){
                    c = fgetc(source);
                }
                fprintf(interm,"%c",c);// Print the "\n"
                fprintf(interw,"%c",c);// Print the "\n"
            }
        }
        else if(c == 34){// We identify a double quote
            fprintf(interm,"%c",39);
            fprintf(interw,"%c",39);
            c = fgetc(source);
            while(c != 34 && c != EOF){// We read the entire string and "discard" it
                c = fgetc(source);
            }
            fprintf(interm,"%c",39);
            fprintf(interw,"%c",39);
        }
        else if(c == 39){// We identify a single quote
            fprintf(interm,"%c",39);
            fprintf(interw,"%c",39);
            c = fgetc(source);
            while(c != 39 && c != EOF){// We read the entire string and "discard" it
                c = fgetc(source);
            }
            fprintf(interm,"%c",39);
            fprintf(interw,"%c",39);
        }
        else if(isalpha(c) || isdigit(c) || c == 95 ){// we detect a letter or a number (beginning of a word)
            char word[N];
            int i = 0;
            while(isalpha(c) || isdigit(c) || c == 95){//we go through the word and stock it
                word[i++] = c;
                c = fgetc(source);
            }
            word[i] = 0;
            int j = 0;
            while(j < 27){
                if(strcmp(word,C_key_words[j]) == 0){//We compare the word with C key words if it is a key word we print m
                    fprintf(interm,"m");
                    break;
                }
                j++;
            }
            if(j >= 27){
                fprintf(interm,"w");// We replace the word with w
            }
            fprintf(interw,"w");
            if(c != 32 ){
                fprintf(interm,"%c",c);// we print the non letter character which got us out of the loop if it is not a space
                fprintf(interw,"%c",c);
            }
        }
        else{
            fprintf(interm,"%c",c); // If we end up here that means the character is c code ponctuation so we print it
            fprintf(interw,"%c",c);
        }
    }
}

void SuppVoid(FILE* inter, FILE* dest){// This function removes empty lines and lines which are just { or } from the file
  char line[N];
    while (fgets(line, N, inter)){
        if ((line[0] != '\n') && (line[0] != '}') && (line[0] != '{')){
            fprintf(dest,"%s",line);
        }
        if ((line[0]=='}')&& (line[1]!='\n')){ //if line is '}' followed by a string or character we do not remove it
            fprintf(dest,"%s",line);
        }
        if ((line[0]=='{')&& (line[1]!='\n')){ //if line is '{' followed by a string or character we do not remove it
            fprintf(dest,"%s",line);
        }
    }
}

typedef struct digraphs{ //creation of a structure to save digraphs for n=2
    char string[2];
    struct digraphs *next;
} Digraphs;

typedef struct { //creation of a linked list to represent all the digraphs in one segment
    Digraphs *first;
} DigraphsList;

DigraphsList* Void_DigraphsList(){ // creation of an empty linked list that will be filled when we are analyszing each segment, all its elements will be initialized to NULL
    DigraphsList *l=malloc(sizeof(DigraphsList));
    if (l==NULL){
        return NULL;
    }//return if memory allocation failed
    l->first=NULL;
    return l;
}
Digraphs * create_digraph(char c[2]){ //creation of a new digraph from the string passed as an argument
    Digraphs *s=malloc(sizeof(Digraphs));
    if (s==NULL){
        return NULL;
    } //return if memory allocation failed
    int i=0;
    while (i<2){
        s->string[i]=c[i];
        i++;
    }
    s->string[i]='\0';
    s->next=NULL; //for now, the digraph isnt in a linked list so we initialize it's next element to NULL
    return s;
}

void Add_Digraph(DigraphsList* l, char c[2]){
    Digraphs *s=create_digraph(c);
    if ((l->first)==NULL){
        l->first=s;
    } // case where the list is empty: the new digraph structure is the first element
    else{ //if not we will run through the list until we find the last element
       Digraphs *x2=l->first;
        while (x2->next!=NULL){
            x2=x2->next;
        }
        x2->next=s;// after the last element is found, the new digraph will be added right after it.
    }
}

DigraphsList* Cuting(char c[]){ //this function creates a linked list of digraphs for a string passed as an argument.
    int n=strlen(c);
    char d[2];
    DigraphsList *l=Void_DigraphsList();
    for (int i=0; i<n-1; i++){
        d[0]=c[i];
        d[1]=c[i+1];
        Add_Digraph(l,d);
    }
    return l;
}
void empty_DiagraphsList(DigraphsList *l){// this function empties all the elements of a linked list of digraphs
    Digraphs *e=l->first;
    Digraphs *sauv=NULL;
    while (e!=NULL){
        sauv=e; //we save the current digraph that will be freed later
        e=e->next;
        free(sauv);
    }
    free(l); //after having freed all its elements , we free the list itself
}

int digraphs_lenght(char c[]){ // this function returns the number of digraphs in a string passed as an argument
    int i=0; //i is an increment that will be returned at the end
    DigraphsList *l=Cuting(c);
    Digraphs *e=l->first; // we run through th whole list to count its elements
    while (e!=NULL){
        i++;
        e=e->next;
    }
    empty_DiagraphsList(l); //after having counted the number of elements, we dont need the list anymore that is why we free it
    return i;
}

int compare_digraphs(char c[], char c1[]){ //function that allows us to count the digraphs in common between two strings passed as an argument
    int i=0;
    DigraphsList *l=Cuting(c); //creation of linked list of digraphs for the first string
    DigraphsList *l1=Cuting(c1);  //creation of linked list of digraphs for the second string
    int m=digraphs_lenght(c);
    int n=digraphs_lenght(c1);
    if (m<=n){ //the "if" tests allows us to know which list to run through first: we run through the smallest list first.
        Digraphs *e=l->first;
        while (e!=NULL){
            Digraphs *e1=l1->first;
            while (e1!=NULL){
                if (strcmp(e->string,e1->string)==0){ //if both the digraphs are similar we increase i by 1
                    i++;
                    break; //we also break from the function so that one digraph is only counted once even if its repeated.
                }
                e1=e1->next;
            }
            e=e->next;
        }
    }
    else{ //the same exact mecanism as the first loop but we run through the list l1 first
        Digraphs *e=l1->first;
        while (e!=NULL){
            Digraphs *e1=l->first;
            while (e1!=NULL){
                if (strcmp(e->string,e1->string)==0){
                    i++;
                    break;
                }
                e1=e1->next;
            }
            e=e->next;
        }
    }//after having counted the elements in common we empty both lists, that we no longer need
    empty_DiagraphsList(l1);
    empty_DiagraphsList(l);
    return i;
}
float DiceDistance(char c[] ,char c1[]){//function to calculate the Dice distance using the three functions defined before
    int nx=compare_digraphs(c,c1);
    int nc=digraphs_lenght(c);
    float nc1=digraphs_lenght(c1);
    return 1-((2*nx)/(nc+nc1)); //formula given by the project instructions
}
typedef struct segment{// creation of a structure to save each line as a segment, while also saving its line number
    char string[N];
    int lines;
    struct segment *next;
} Segment;

typedef struct {// creation of a linked list to save the segments created
    Segment *first;
} List;

Segment * create_segment(char x[N], int lines1){//creation of a new segment from the string and number of line passed as arguments
    Segment *s=malloc(sizeof(Segment));
    if (s==NULL){
        return NULL;
    } //return if memory allocation failed
    int i=0; //copy of the string in string component of the structure "segment"
    while(i<N){
        s->string[i]=x[i];
        i++;
    }
    s->string[i]='\0';
    s->next=NULL;
    s->lines=lines1; //save line number of segment
    return s;
}

List* void_list(){  // creation of an empty linked list that will be filled when we are reading the files, all its elements will be initialized to NULL
    List *l=malloc(sizeof(List));
    if (l==NULL){
        return NULL;
    }//return if memory allocation failed
    l->first=NULL;
    return l;
}

void Add_segment(List* l, char x[N], int lines){
    Segment *s=create_segment(x, lines);
    if ((l->first)==NULL){
        l->first=s;
    }// case where the list is empty: the new segment structure is the first element
    else{ //if not we will run through the list until we find the last element
       Segment *x2=l->first;
        while (x2->next!=NULL){
            x2=x2->next;
        }
        x2->next=s; // after the last element is found, the new segment will be added right after it.
    }
}
int List_lenght(List *l){// this function returns the number of segments in a linked list passed as an argument
    int i=0;
    Segment *e=l->first;
    while (e!=NULL){ // run through the whole list until we get to the end
        i++;
        e=e->next;
    }
    return i;
}

void saveimage (int vmax, char *filename, int K, int Z, float M[K][Z]){
    FILE* f=fopen(filename, "w"); //open a file in "writing" mode so that we can fill it and create an image
    if(f == NULL) { // returns if function failed to open file
        perror("Unable to open file!");
        exit(1);
    }
    fprintf(f,"%s\n", "P2"); //format of the image created
    fprintf(f,"%d ",K);//width of the image
    fprintf(f,"%d\n",Z);//height of the image
    fprintf(f,"%d\n", vmax); //maximal value of pixel used
    for (int i=0; i<K; i++){
        for (int j=0; j<Z; j++){
            fprintf(f,"%d ",(int)(1-M[i][j])*vmax);//fill the matrix of the image one by one
        }
    }
    fclose(f);
}
void empty_list(List *l){// this function empties all the elements of a linked list of segments
    Segment *e=l->first;
    Segment *sauv=NULL;
    while (e!=NULL){
        sauv=e;  //we save the current digraph that will be freed later
        e=e->next;
        free(sauv);
    }
    free(l);//after having freed all its elements, we free the list itself
}
int min1(int a, int b, int c){ // function to determine minimum of 3 integers
    if(a<b && a<c){
        return a;
    }
    else if(b<c){
        return b;
    }
    else{
        return c;
    }
}
int max(int a, int b){ //function to determine maximum of 2 integers
    if (a>b){
        return a;
    }
    else{
        return b;
    }
}
int levenshtein(char s1[], char s2[]) { //computation of levenshtein distance.
    unsigned int x, y, s1len, s2len;
    s1len = strlen(s1);
    s2len = strlen(s2);
    unsigned int matrix[s2len+1][s1len+1];
    matrix[0][0] = 0;
    for (x = 1; x <= s2len; x++)
        matrix[x][0] = matrix[x-1][0] + 1;
    for (y = 1; y <= s1len; y++)
        matrix[0][y] = matrix[0][y-1] + 1;
    for (x = 1; x <= s2len; x++)
        for (y = 1; y <= s1len; y++)
            matrix[x][y] = min1(matrix[x-1][y] + 1, matrix[x][y-1] + 1, matrix[x-1][y-1] + (s1[y-1] == s2[x-1] ? 0 : 1));

    return(matrix[s2len][s1len]);
}
int main(int argc, char *argv[]){
    int distance1;// ask the user which distance they want to use in this program
    printf("do you want to excecute program with dice distance (enter 1) or  levenshtein distance (enter 2 )?\n");
    scanf("%d", &distance1);
    int bonus; //ask the user if they want to print the plagiarized lines at the end
    printf("do you want to excecute program with the bonus (print in colors the plagiarized lines ? enter 1 if yes, 0 if not \n");
    scanf("%d", &bonus);
    // the lines variables will be used to keep track of the line number of each segment in the two files
    int lines1=1;
    int lines2=1;
    char buff[N];
    char buff1[N];
    //creation of two empty lists that will be filled when we are reading the two translated files.
    List* list1= void_list();
    List* list2= void_list();
    // We start by initialising all the files we will need for the rest of the programm and we use our translation functions on both c files in order to compare them 
    FILE* source = NULL;
    source = fopen(argv[1],"r");
    if(source == NULL){
        printf("Unable to open the file source");
        return 0; //the code returns because it failed to open the first filed (procedure that is repeated in the following lines)
    }
    FILE* interm = NULL;
    interm = fopen("interm.txt","w");
    if(interm == NULL){
        printf("Unable to create a file inter");
        return 0;
    }
    FILE* interw = NULL;
    interw = fopen("interw.txt","w");
    if(interw == NULL){
        printf("Unable to create a file2");
        return 0;
    }
    FILE* destm = NULL;
    destm = fopen("Traductionm.txt","w");//creation of the file translated with the bonus in the first part
    if(destm == NULL){
        printf("Unable to create a file3");
        return 0;
    }
    FILE* destw = NULL;
    destw = fopen("Traductionw.txt","w");//creation of the file translated without the bonus in the first part
    if(destw == NULL){
        printf("Unable to create a file3");
        return 0;
    }
    Transform(source,interm,interw);
    fclose(source);
    fclose(interm);
    fclose(interw);
    interm = fopen("interm.txt","r");
    interw = fopen("interw.txt","r");
    SuppVoid(interm,destm);
    SuppVoid(interw,destw);
    fclose(destm);
    fclose(destw);
    fclose(interw);
    fclose(interm);
    FILE* source1 = NULL;
    source1 = fopen(argv[2],"r");
    if(source1 == NULL){
        printf("Unable to open the file1");
        return 0;
    }
    FILE* interm1 = NULL;
    interm1 = fopen("interm1.txt","w");
    if(interm1 == NULL){
        printf("Unable to create a file inter");
        return 0;
    }
    FILE* interw1 = NULL;
    interw1 = fopen("interw1.txt","w");
    if(interw1 == NULL){
        printf("Unable to create a file2");
        return 0;
    }
    FILE* destm1 = NULL;
    destm1 = fopen("Traductionm1.txt","w");//creation of the file translated with the bonus in the first part
    if(destm1 == NULL){
        printf("Unable to create a file3");
        return 0;
    }
    FILE* destw1 = NULL;
    destw1 = fopen("Traductionw1.txt","w");//creation of the file translated without the bonus in the first part
    if(destw1 == NULL){
        printf("Unable to create a file3");
        return 0;
    }
    Transform(source1,interm1,interw1);
    fclose(source1);
    fclose(interm1);
    fclose(interw1);
    interm1 = fopen("interm1.txt","r");
    interw1 = fopen("interw1.txt","r");
    SuppVoid(interm1,destm1);
    SuppVoid(interw1,destw1);
    fclose(destm1);
    fclose(destw1);
    fclose(interw1);
    fclose(interm1);
    //we open the two translated files and run through them to fill our empty lists previously created
    FILE* f1=fopen("Traductionm.txt","r");
    while(fgets(buff, sizeof(buff), f1) != NULL) {
            Add_segment(list1,buff,lines1);
            lines1++;
    }
    fclose(f1);
    FILE* f2=fopen("Traductionm1.txt","r");
    if(f2 == NULL) {
        perror("Unable to open file!");
        exit(1);
    }
    while(fgets(buff1, sizeof(buff1), f2) != NULL) {
            Add_segment(list2,buff1,lines2);
            lines2++;
    }
    fclose(f2);
    Segment*e3=list1->first;
    int Lenghtl1=List_lenght(list1);
    int lenghtl2=List_lenght(list2);
    float M[Lenghtl1][lenghtl2]; //creation of the matrix M that will contain the dice distance of each two segments.
    for (int i=0;i<Lenghtl1;i++){
        Segment*e4=list2->first;
        for (int j=0; j<lenghtl2; j++){
            if (distance1==1){
                M[i][j]=DiceDistance(e3->string,e4->string);
            }
            else{
                float z=max(strlen(e3->string), strlen(e4->string));
                M[i][j]=(levenshtein(e3->string,e4->string)/z); //we normalize the levenshtein by dividing it by the maximum length of the two strings
            }
            e4=e4->next;
        }
        e3=e3->next;
    }
    float M1[Lenghtl1][lenghtl2]; //we create a copy of the matrix M, that will be useful when we have to fill the matrix C of the 4th part of the project
    float C[Lenghtl1][lenghtl2]; //creation of matrix C 
    float F[Lenghtl1][lenghtl2];//creation of matrix F 
    for (int i=0;i<Lenghtl1;i++){ //copying all elements of M in M1
        for (int j=0; j<lenghtl2; j++){
            M1[i][j]=M[i][j];
        }
    }
    saveimage(255,"dice.pgm",Lenghtl1,lenghtl2,M); //creation of the first image using the Matrix M, with the width being the number of lines of M and the height of the image being the number of rows of M
    //emptying the two segment lists that we dont need anymore
    empty_list(list1);
    empty_list(list2);
    //We start by filling all the elements of C with -1 as a value
    for (int i=0;i<Lenghtl1;i++){
        for (int j=0;j<lenghtl2;j++){
            C[i][j]=-1;
        }
    }
    float smallest=0;
    while (1){
        smallest=10;
        int lines=0;
        int row=0;
        //we look for the smallest element in the Matrix M1, and we save its line ans row in the Matrix
        for (int i=0;i<Lenghtl1;i++){
            for (int j=0;j<lenghtl2;j++){
                if (M1[i][j]<smallest){
                    smallest=M1[i][j];
                    lines=i;
                    row=j;
                    
                }
            }
        }
        if (smallest==10){ //if the smallest element is 10, that means we are done with the algorithm and we break from the function
            break;
        }
        //However, if that isnt the case, we copy the smallest element of M1 into C (in the same position)
        C[lines][row]=smallest;
        M1[lines][row]=10; //and we remove that smallest element from M1, by replacing it with the value 10 (that can never be attained because the dice distance is between 0 and 1)
        for (int i=0;i<lenghtl2;i++){
            if (i!=row){
                C[lines][i]=1;
                M1[lines][i]=10; // we get rid of all the rows corresponding to the smallest element's line
                
            }
        }
        for (int j=0;j<Lenghtl1;j++){
            if (j!=lines){
                C[j][row]=1;
                M1[j][row]=10; // we get rid of all the lines corresponding to the smallest element's row
            }
        }
    }
    //generating the image "couplage.pgm".
    saveimage(255,"couplage.pgm",Lenghtl1,lenghtl2,C);
    //filling the elements of F[Lenghtl1-2][lenghtl2-2] with the convolution formula
   for (int i=2; i<Lenghtl1-2; i++){
        for (int j=2; j<lenghtl2-2; j++){
            double res=0;
            for (int k=-2; k<3; k++){
                res+=C[i+k][j+k];
            }
            F[i][j]=(1.0/5.0)*res;
        }
    }
    // we fill with 1's the part of the matrix which cannot be calculated with the formula (the extremities)
    for (int i=2;i<lenghtl2-2;i++){
        F[0][i]=1;
        F[1][i]=1;
        F[Lenghtl1-1][i]=1;
        F[Lenghtl1-2][i]=1;
    }
    for (int i=2;i<Lenghtl1-2;i++){
        F[i][0]=1;
        F[i][1]=1;
        F[i][lenghtl2-1]=1;
        F[i][lenghtl2-2]=1;
    }
    //We fill the four corners of the matrix whth zeros on the diagonal
    F[0][0]=0;
    F[0][1]=1;
    F[1][0]=1;
    F[1][1]=0;
    
    F[0][lenghtl2-1]=1;
    F[0][lenghtl2-2]=1;
    F[1][lenghtl2-1]=1;
    F[1][lenghtl2-2]=1;
    
    F[Lenghtl1-1][0]=1;
    F[Lenghtl1-2][0]=1;
    F[Lenghtl1-1][1]=1;
    F[Lenghtl1-2][1]=1;
    
    F[Lenghtl1-2][lenghtl2-2]=0;
    F[Lenghtl1-2][lenghtl2-1]=1;
    F[Lenghtl1-1][lenghtl2-2]=1;
    F[Lenghtl1-1][lenghtl2-1]=0;
    
    // We use the threshold to remove all values over 0.7  
    for (int i=0; i<Lenghtl1; i++){
        for (int j=0; j<lenghtl2; j++){
            if (F[i][j]>=0.7){
                F[i][j]=1;
            }
        }
    }
    saveimage(255,"filtrage.pgm",Lenghtl1,lenghtl2,F); //generating last image
   float min=((Lenghtl1>lenghtl2) ? lenghtl2:Lenghtl1); //find the smallest lenghth of files.
   float m=0;
    //calculate m which is gonna be used to find final result.
    for (int i=0;i<Lenghtl1;i++){
        for (int j=0; j<lenghtl2;j++){
            m+=(1-F[i][j]);
        }
    }
    //calculate final result thanks to the formula given by the instructions
    float distance=1-(m/min);
    printf("result with bonus is :%.2f\n", distance); //print the final result with only 2 decimals
    //bonus
    if (bonus==1){
        char buff2[N];
        char buff3[N];
        int lines3=1;
        int lines4=1;
        List* list3= void_list();
        List* list4= void_list();
        FILE* f4=fopen("interm.txt","r");
        //creation of two lists List3 and list4 that will contain all the original lines of the code without the supression of the empty lines and '{' or '}' and the number of their lines.
        while(fgets(buff2, sizeof(buff2), f4) != NULL) {
                Add_segment(list3,buff2,lines3);
                lines3++;
        }
        fclose(f4);
        FILE* f5=fopen("interm1.txt","r");
        if(f5 == NULL) {
            perror("Unable to open file!");
            exit(1);
        }
        while(fgets(buff3, sizeof(buff3), f5) != NULL) {
                Add_segment(list4,buff3,lines4);
                lines4++;
        }
        fclose(f5);
        int lenghtl3=List_lenght(list3);
        int lenghtl4=List_lenght(list4);
        Segment*e7=list3->first;
        printf("color code: black/white for 2 identically similar lines, red for 2 lines that ressemble greatly, and yellow for two lines suspected to be plagiarized \n");
        // we compare each segment of the list3 to each segment of list4, if that segment is different from the empty line , '{' or '}'.
        for (int i=0;i<lenghtl3;i++){
            Segment*e8=list4->first;
            for (int j=0; j<lenghtl4; j++){
                if ((e7->string[0]!='\n') && (e8->string[0]!='\n')&& (e7->string[0]!='}') &&(e8->string[0]!='}') && (e7->string[0]!='{') && (e8->string[0]!='{')){ // if the segment is a non empty line of code we calculate dice distance and print it according to the color code set.
                    float m;
                    if (distance1==1){
                        m=DiceDistance(e7->string,e8->string);
                        
                    }
                    else{
                        float z=max(strlen(e7->string), strlen(e8->string));
                        m=(levenshtein(e7->string,e8->string)/z);
                    }
                    if (m==0){
                        printf("line %d of first file and line %d of second file \n",e7->lines,e8->lines);
                    }
                    else if(m<=0.25){
                            red();
                            printf("line %d of first file and line %d of second file \n",e7->lines,e8->lines);
                            reset();
                    }
                    else if(m<=0.5){
                        yellow();
                        printf("line %d of first file and line %d of second file \n",e7->lines,e8->lines);
                        reset();
                        
                    }

                    e8=e8->next;
                }
            }
            e7=e7->next;
        }
        empty_list(list3); //emptying the lists we no longer need
        empty_list(list4);
    }
    remove("interw.txt"); //delete all the files after we no longer need them
    remove("interm.txt");
    remove("interw1.txt");
    remove("interm1.txt");
    remove("Traductionw.txt");
    remove("Traductionw1.txt");
    return 0;
}



