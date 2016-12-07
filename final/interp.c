/**********************************************************************************/
// racket-interpreter.c
// input: (* 1000 (+ 10 (-20 10)))
// output: 20000
// program takes input as string, then put into FIFO,
// FIFO goes through Lexer to be tokenized
// tokens are parsed into Expression Tree
// Expression Tree is evaluated and printed out
/**********************************************************************************/
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <ctype.h>
#define MAX_BUFFER_SIZE 1000
#define MAX_VALUE_LENGTH 20
char buff_two[MAX_BUFFER_SIZE];
int is_user_define = 0;


/**********************************************************************************/
// FIFO with registers of
// num   holding integer
// string holding either variable name or operation
// dataype include: 
// 'I'-INT 'L'-"LEFTP" 'R'-"RIGHTP" 'D'-"DEFINE" 'O'-"OPERATION" S'-"STRING"
// 'V'-VRIABLE NAME  'F'- FUNCTION NAME
/**********************************************************************************/
struct reg
{
    char type; 
    int num; 
    char s[MAX_VALUE_LENGTH];
    struct  reg *next;
};
struct queue {
    struct reg *head;
    struct reg *last;
};
struct queue *enqueue (char type, int num, char s[MAX_VALUE_LENGTH], struct queue *q) {
    if (q->head == NULL) {
        q->head = malloc(sizeof(struct reg));
        q->last = q->head;
        q->head->type = type;
        q->head->num = num;
        strcpy(q->head->s,s);
        q->head->next = NULL;
    } else {
        q->last->next = malloc(sizeof(struct reg));
        q->last = q->last->next;
        q->last->type = type;
        q->last->num = num;
        strcpy(q->last->s,s);
        q->last->next = NULL;
    }
    return q;
}
struct queue *dequeue (struct queue *q) {
    if (q->head == NULL) {
        /* Should probably error here but we'll just return */
        return q;
    }
    struct reg *old_head = q->head;
    q->head = q->head->next;
    free(old_head);
    if (q->head == NULL) {
        q->last = NULL;
    }
    return q;
}
/***************************************************************************/
// print_fifo: Que -> console 
// print lexer output 
/***************************************************************************/
void print_fifo_with_start_reg(struct reg *q){
    printf("------------------\n");
    printf("last-half-que: \n");
    for (struct reg *p = q; p != NULL; p = p->next) {
        printf("%c->",p->type);
        
        switch(p->type) {
            case 'O'  :
            printf("%s\n",p->s);
            break; 
    
            case  'I' :
            printf("%d\n",p->num);
            break; 

            case  'L' :
            printf("(\n");
            break; 

            case  'R' :
            printf(")\n");
            break; 

            case  'D' :
            printf("%s\n",p->s);
            break; 

            case  'V' :
            printf("%s\n",p->s);
            break; 

            case  'F' :
            printf("%s\n",p->s);
            break;
         
            default : /* Optional */
            printf("%s\n",p->s);
        }
    }   
    printf("------------------\n");
}
/***************************************************************************/
// print_fifo: Que -> console 
// print lexer output 
/***************************************************************************/
void print_fifo(struct queue *q){
    printf("------------------\n");
    printf("Lexer Output: \n");
    for (struct reg *p = q->head; p != NULL; p = p->next) {
        printf("%c->",p->type);
        
        switch(p->type) {
            case 'O'  :
            printf("%s\n",p->s);
            break; 
    
            case  'I' :
            printf("%d\n",p->num);
            break; 

            case  'L' :
            printf("(\n");
            break; 

            case  'R' :
            printf(")\n");
            break; 

            case  'D' :
            printf("%s\n",p->s);
            break; 

            case  'V' :
            printf("%s\n",p->s);
            break; 

            case  'F' :
            printf("%s\n",p->s);
            break;
         
            default : /* Optional */
            printf("%s\n",p->s);
        }
    }   
    printf("------------------\n");
}
/***************************************************************************/
// lexer: String -> Queue
// split string into singletons, also checking parenthsis  
/***************************************************************************/
struct queue *lexer (char inbuff[MAX_BUFFER_SIZE], struct queue *q){
    int a = 0; 
    int j = 0;
    int fun_name_decleared=0;
    char value[MAX_VALUE_LENGTH];
    // init value holder
    for(int i = 0; i<MAX_VALUE_LENGTH; i++){
            value[i]=-1; 
    }  

    for (int i= 0; i<strlen(inbuff) ;i++){
        
        // if not () nor space: a string "abc" "define" "if" or number 23
        if (!isspace(inbuff[i]) && !(inbuff[i]== '(') && !(inbuff[i]== ')')){
            // record new value
            value[j] = inbuff[i];
            j++;
        }
        // if there is a space or a (), signifying an end, push to fifo
        if (value[0]!= -1 && (isspace(inbuff[i]) || (inbuff[i]== '('  ||  inbuff[i]== ')'))){
            value[j] = '\0';
            j = 0; 
            //printf("%s\n",value);

            //check type
            if(isdigit(value[0]))
                q = enqueue('I',atoi(value),"", q);

            else if(value[0] == '+' || value[0] == '-' || value[0] == '*' || value[0] == '/')
                q = enqueue('O',0,value, q);

            else {
                is_user_define = 1;
                if(strcmp(value, "define")==0){
                    q = enqueue('D',0,value, q);
                    fun_name_decleared =0;
                }
                else if (fun_name_decleared ==0){
                    q = enqueue('F',0,value, q); 
                    fun_name_decleared =1;        
                } 
                else
                    q = enqueue('V',0,value, q);      
            }

            //init value holder
            for(int i = 0; i<MAX_VALUE_LENGTH; i++){
                value[i]=-1;
            }          
        }
        // if Parenthesis
        if (inbuff[i]== '('  ||  inbuff[i]== ')') {
            switch(inbuff[i]) {
            case '('  :
                a++;
                q = enqueue('L',0,"", q);
            break; 
            case  ')' :
                a--;
                q = enqueue('R',0,"", q);
            break; 
            }
        }

    }
        if(a > 0){
            printf("racket-> missing %d ')' in expression\n", a);  
            exit(0); 
        }
        if(a < 0){
            printf("racket-> missing %d '(' in expression\n", abs(a));
                exit(0);
        }

        print_fifo(q);
    return q;
}


/***************************************************************/
// An Arithmatic_Expression (ar_exp) is one of 
// Number (int)
// (make-op ar-exp function ar_exp)
// 
// an arithmatic expression is either a number or 
// an opration of + - * / with two inputs 
/***************************************************************/
struct ar_exp
{
    int number;
    struct op_tree * operation_tree;
};

/******************************************************************/
// An Operation Tree (op_tree) is a struct of 
// function: '+' '-' '*' '/'
// left and right ar_exp 
/******************************************************************/
struct op_tree 
{
    char func;
    struct ar_exp *left;
    struct ar_exp *right;
};

void free_tree(struct op_tree *tree) {
    if (tree != NULL) {
        free(tree);
    }
}
/********************************************************************/
// print_tree: Arithmatic Expression -> Print 
// Print an arithmatic tree
/*********************************************************************/
void print_tree(struct ar_exp *tree) {
    /* We'll print the tree in a Racket-like way:
     * (root left-subtree right-subtree)
     * where the subtrees are recursively printed.
     * Printing x for null.
     */
    if(tree->operation_tree == NULL){
        printf("%d", tree->number);
    }
    else {
        printf("(%c ", tree->operation_tree->func);
        print_tree(tree->operation_tree->left);
        printf(" ");
        print_tree(tree->operation_tree->right);
        printf(")");
    }
}
/******************************************************************/
// evaluate: Arithmatic_Expression -> Int
// recursively evaluate left and right side of the operation tree
/******************************************************************/
int evaluate (struct ar_exp* exp_to_eval){
    if(exp_to_eval -> operation_tree ==NULL){
        return exp_to_eval -> number;
    }
    else{
        switch((exp_to_eval->operation_tree->func)) {
            case '+' :
                return  (evaluate (exp_to_eval->operation_tree->left) +
                        evaluate (exp_to_eval->operation_tree->right));
            break; 
            case '-' :
                return  (evaluate (exp_to_eval->operation_tree->left) -
                        evaluate (exp_to_eval->operation_tree->right));
            break; 
            case '*' :
                return  (evaluate (exp_to_eval->operation_tree->left) *
                        evaluate (exp_to_eval->operation_tree->right));
            break; 
            case '/' :
                return  (evaluate (exp_to_eval->operation_tree->left) /
                        evaluate (exp_to_eval->operation_tree->right));
            break; 

        default : 
            printf("Unrecognized operation, buy the full version to continue...\n");
            return -1;
            break;
        }

    }
}


/********************************************************************/
// split_que: Reg ->  Reg 
// given a queue of embedded experessions, split the que into 2 halfs
// return the Queque Pointer - Mid, start of second half 
// input:  1 2)
// output: reg at: 2 
// input:  (* 2 3) 4)
// output: reg at: 4
// input:  (+ 1 2) (* 3 4))
// output: reg at: (
/*********************************************************************/
struct reg * split_que(struct reg *q){

	struct reg *mid = q;
	int parent_count = 0;

	//if the first is not '(' , return the second number
	if(q->type == 'I')
		mid = q->next;
	else {
		//return the one after parenthsis are balanced
		for (struct reg *p = q; p != NULL; p = p->next) {
            //printf("%c \n", p->type);
			if(p->type == 'L'){
				parent_count ++;
			}
			if(p->type == 'R'){
				parent_count --;
			}
            if(parent_count ==0){
                mid = p->next;
                break;
            }
       	} 
    }   

    //printf("split at type -%c \n", mid->type);
	return mid;
}


/********************************************************************/
// make_exp_recursive: Reg(Que->head) -> Arithmatic Expression
// to form an Arithmatic Expression from a string statement
// pop queque each time one token is evaluated
/*********************************************************************/
struct ar_exp * make_exp_recursive(struct reg *p) {

	//struct reg *p = q->head;
	struct ar_exp * exp_to_eval = malloc(sizeof(struct ar_exp));

	if(p->type == 'I'){	
		exp_to_eval -> number = p->num;
    	exp_to_eval -> operation_tree = NULL;
	}
 
	//if an open paren (+ 1 2)    or (+ (+ 1 2) 3)
	if(p->type == 'L'){
        // p->next = operation
        // p->next->next = expression
		//p = p->next;  
		//if(p->type == 'O'){
		//initialize a operation tree
	    struct op_tree * newtree = malloc(sizeof(struct op_tree));
	    newtree->func = p->next->s[0];
	    	
	    // 1 2)   or      (+ 1 2) 3)
		struct reg *rest_q = p->next->next;
	    //initialize left expression tree
	    struct ar_exp * left = malloc(sizeof(struct ar_exp));
	    left = (make_exp_recursive (rest_q));

	    // 1 2)     or     (+ 1 2) 3)
	    //print
	    //print_fifo_with_start_reg(split_que(p));
		struct reg *rest_rest_q = split_que(p->next->next);


	    // initialize right expression tree
	    struct ar_exp * right = malloc(sizeof(struct ar_exp));
	    right = (make_exp_recursive (rest_rest_q));

	    // assign right and left trees to new tree
	    newtree->left = left;
	    newtree->right = right;

    	// make-expression assign tree and value
    	exp_to_eval -> number = 0;
    	exp_to_eval -> operation_tree = newtree;
		//}
	}

	// something went wrong
	else{
		struct ar_exp * exp_to_eval = malloc(sizeof(struct ar_exp));
		exp_to_eval -> number = -10000;
    	exp_to_eval -> operation_tree = NULL;
	}
    
    //printf("Arithmatic Tree:\n");
    //print_tree(exp_to_eval);
    //printf("\n");
    return exp_to_eval;		
}



/********************************************************************/
// Test case 0: (* (* 3 2) (+ 2 3)) -> 30
/*********************************************************************/
struct ar_exp * test_case0(){

    struct ar_exp * two = malloc(sizeof(struct ar_exp));
    two -> number = 2;
    two -> operation_tree = NULL;

    struct ar_exp * three = malloc(sizeof(struct ar_exp));
    three -> number = 3;
    three -> operation_tree = NULL;

    //(* 3 2)
    struct op_tree * tree_left = malloc(sizeof(struct op_tree));
    tree_left->func = '*';
    tree_left->left = three;
    tree_left->right = two;

    //(+ 2 3)
    struct op_tree * tree_right = malloc(sizeof(struct op_tree));
    tree_right->func = '+';
    tree_right->left = two;
    tree_right->right = three;

    struct ar_exp * exp_left = malloc(sizeof(struct ar_exp));
    exp_left -> number = 0;
    exp_left -> operation_tree = tree_left;

    struct ar_exp * exp_right = malloc(sizeof(struct ar_exp));
    exp_right -> number = 0;
    exp_right -> operation_tree = tree_right;


    //(* (* 3 2) (+ 2 3)
    struct op_tree * tree_final = malloc(sizeof(struct op_tree));
    tree_final->func = '*';
    tree_final->left = exp_left;
    tree_final->right = exp_right;

    struct ar_exp * exp_to_eval = malloc(sizeof(struct ar_exp));
    exp_to_eval -> number = 0;
    exp_to_eval -> operation_tree = tree_final;
    return exp_to_eval;    
}



int main(){

    //Test print-tree()
    //printf("Arithmatic Tree:\n");
    //print_tree(test_case0());
    //printf("\n");

    // Test evaluate()
    //printf("%d\n", evaluate(test_case0()));

	struct queue *q = malloc(sizeof(struct queue));
    q->head = NULL;
    q->last = NULL;

    printf("Enter Racket Expression (two argument operations):\n");
    char inbuff[MAX_BUFFER_SIZE];
    for(int i = 0; i< MAX_BUFFER_SIZE; i++){
        inbuff[i]= ' ';
    }

    fgets(inbuff, MAX_BUFFER_SIZE, stdin);
    //printf("%s",inbuff);
    q = lexer(inbuff,q);

    if(is_user_define){
        printf("defined function\n");  
    }       


    struct ar_exp * expression = make_exp_recursive(q->head);
    printf("Arithmetic Tree:\n");
    print_tree(expression);
    printf("\n");

    printf("answer-> %d\n", evaluate (expression));

    free_tree (expression->operation_tree);
    free(q);
   
}


