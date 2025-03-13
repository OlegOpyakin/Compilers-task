#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>


#define BUF_LENGTH 1024
#define MULT_LENGTH 26
#define PSF_LENGTH 50


// Multiplier element from PSF
struct MultElem
{
    int num_mult_; // numerical multiplier before element
    char variables_[MULT_LENGTH]; // variables in element, x, y, z, etc

    size_t num_of_variables; // number of vars in one element
};


// Product summ form
struct PSF
{
    struct MultElem elems_[PSF_LENGTH]; // array for multiplication elements
    size_t elems_num_; // number of elements in current PSF
    int used[PSF_LENGTH]; // ХЗ

    char* buf_; // buffer where we write string before parsing
    size_t size_; // size of the buf
    size_t length_; // length of written symbols into buf_
};


// debug print of our PSF to get more info
void DebugPrint(struct PSF* PSF){
    printf("elems_num_ = %zu\n", PSF->elems_num_);
    
    for(int i = 0; i < PSF->elems_num_; ++i)
    {
        printf("element %d has mult: %d\n", i, PSF->elems_[i].num_mult_);
    
        for(int j = 0; j < PSF->elems_[i].num_of_variables; ++j)
        {
            printf("variable: %c\n", PSF->elems_[i].variables_[j]);
        }
        printf("\n");
    }
}


// standart print of our PSF
void Print(struct PSF* PSF){
    for(int i = 0; i < PSF->elems_num_; ++i)
    {
        /*
        if(PSF->elems_[i].num_mult_ != 1)
        {
            printf("%d", PSF->elems_[i].num_mult_);
        }

        if(PSF->elems_[i].num_mult_ == -1)
        {
            printf("-");
        }*/

        bool star_flag = true;

        if(PSF->elems_[i].num_mult_ == -1)
        {
            printf("-");
        }
        else if(PSF->elems_[i].num_mult_ != 1)
        {
            printf("%d", PSF->elems_[i].num_mult_);
            star_flag =false;
        }
    
        for(int j = 0; j < PSF->elems_[i].num_of_variables; ++j)
        {
            if( ( j == 0 ) && star_flag )
            {
                printf("%c", PSF->elems_[i].variables_[j]);
            }
            else
            {
                printf("*%c", PSF->elems_[i].variables_[j]);
            }
        }
        
        if(i != PSF->elems_num_ - 1)
        {
            if( PSF->elems_[i + 1].num_mult_ > 0 )
            {
                printf("+");
            }
        }
    }
    printf("\n");
}


// parser of PSF form
void LineParse(struct PSF* PSF){

    if(PSF->buf_[0] == '-') // extra case to get first number's sign
    {
        PSF->elems_[0].num_mult_ = -1;
    }
    else
    {
        PSF->elems_[0].num_mult_ = 1;
    }

    int iterator = 1;

    for(int i = 1; i < PSF->length_; ++i) // general parsing to get MultElems number
    {
        if(PSF->buf_[i] == '-')
        {
            PSF->elems_[iterator].num_mult_ = -1;
            iterator++;
        }
        if(PSF->buf_[i] == '+')
        {
            PSF->elems_[iterator].num_mult_ = +1;
            iterator++;
        }

    }
    
    char *saveptr1, *saveptr2;
    char *lexema1, *lexema2;

    int iterator1 = 0;
    int iterator2 = 0;

    PSF->elems_num_ = 0;

    char* new_buf;
    new_buf = (char*)malloc(PSF->length_ + 1);
    
    char* other_new_buf = new_buf;
    char* old_buf = PSF->buf_;

    while (*old_buf) { // get rid from spaces in string before parsing
        if ( isspace(*old_buf) ) {old_buf++; continue;}
        *other_new_buf = *old_buf;
        other_new_buf++;
        old_buf++;
    }
    *other_new_buf = '\0';

    while(1)
    {
        lexema1 = strtok_r(new_buf, "+-\n", &saveptr1); // divide string into Multiplication elements

        if(lexema1 == NULL) break;

        PSF->elems_[iterator1].num_of_variables = 0;
        while(1)
        {
            lexema2 = strtok_r(lexema1, "*", &saveptr2); // divide inside onr mult element

            if(lexema2 == NULL) break;


            if(isdigit(lexema2[0])) // check our lexema2 is int to
            {                       // change number multiplier
                PSF->elems_[iterator1].num_mult_ *= atoi(lexema2); 
                //printf("we say int: %d\n", atoi(lexema2));
            }
            else
            {
                PSF->elems_[iterator1].variables_[iterator2] = lexema2[0];
                PSF->elems_[iterator1].num_of_variables += 1;
                iterator2++;
            }

            lexema1 = NULL;
        }

        iterator1++;
        iterator2 = 0;

        //printf("lexema: %s\n", lexema1);

        PSF->elems_num_ += 1;
        new_buf = NULL;
    }

    free(new_buf);

    //DebugPrint(PSF); 
}


// Assistant func that checks if two strings are equal
bool StringsEqual(char str1[], char str2[], int len1, int len2) {
    if (len1 != len2) {
        return false;
    }

    int count1[26] = {0}; // first alphabet
    int count2[26] = {0}; // second alphabet

    for (int i = 0; i < len1; i++) {
        count1[str1[i] - 'a']++;
    }

    for (int i = 0; i < len2; i++) {
        count2[str2[i] - 'a']++;
    }

    for (int i = 0; i < 26; i++) { // compare alphabets
        if (count1[i] != count2[i]) {
            return false;
        }
    }

    return true;
}


// func to add two PSF
void Add(struct PSF* PSF1, struct PSF* PSF2){
    //bool used1[PSF_LENGTH] = { false }; // flags array for PSF1
    bool used2[PSF_LENGTH] = { false }; // flags array for PSF2 which describes
                                        // if PSF2 element was used

    for(int i = 0; i < PSF1->elems_num_; ++i) // add elements which are in both PSF's
    {
        for(int j = 0; j < PSF2->elems_num_; ++j)
        {
            if( StringsEqual( PSF1->elems_[i].variables_, PSF2->elems_[j].variables_, 
                              PSF1->elems_[i].num_of_variables, PSF2->elems_[j].num_of_variables ) )
            {
                PSF1->elems_[i].num_mult_ += PSF2->elems_[j].num_mult_;
                used2[j] = true;
            }
        }
    }

    for(int i = 0; i < PSF2->elems_num_; ++i) // add unused elements into first PSF
    {
        if(!used2[i]){
            //printf("%d\n", i);
            PSF1->elems_[PSF1->elems_num_].num_mult_ = PSF2->elems_[i].num_mult_;
            for(int j = 0; j < PSF2->elems_[i].num_of_variables; ++j)
            {
                PSF1->elems_[PSF1->elems_num_].variables_[j] = PSF2->elems_[i].variables_[j];
            }
            PSF1->elems_[PSF1->elems_num_].num_of_variables = PSF2->elems_[i].num_of_variables;
            PSF1->elems_num_ += 1;
        }
    }
}


// func to substract PSF2 from PSF1
void Substract(struct PSF* PSF1, struct PSF* PSF2){
    //bool used1[PSF_LENGTH] = { false }; // flags array for PSF1
    bool used2[PSF_LENGTH] = { false }; // flags array for PSF2

    for(int i = 0; i < PSF1->elems_num_; ++i)
    {
        for(int j = 0; j < PSF2->elems_num_; ++j)
        {
            if( StringsEqual( PSF1->elems_[i].variables_, PSF2->elems_[j].variables_, 
                              PSF1->elems_[i].num_of_variables, PSF2->elems_[j].num_of_variables ) )
            {
                if(PSF1->elems_[i].num_mult_ == PSF2->elems_[j].num_mult_) // if we get 0 from substraction
                {
                    PSF1->elems_[i] = PSF1->elems_[PSF1->elems_num_ - 1];
                    PSF1->elems_num_ -= 1;
                }
                else
                {
                    PSF1->elems_[i].num_mult_ -= PSF2->elems_[j].num_mult_;
                }
                used2[j] = true;
            }
        }
    }

    for(int i = 0; i < PSF2->elems_num_; ++i) // as in Add func we add unused elems
    {
        if(!used2[i]){
            //printf("%d\n", i);
            PSF1->elems_[PSF1->elems_num_].num_mult_ = 0 - PSF2->elems_[i].num_mult_;
            for(int j = 0; j < PSF2->elems_[i].num_of_variables; ++j)
            {
                PSF1->elems_[PSF1->elems_num_].variables_[j] = PSF2->elems_[i].variables_[j];
            }
            PSF1->elems_[PSF1->elems_num_].num_of_variables = PSF2->elems_[i].num_of_variables;
            PSF1->elems_num_ += 1;
        }
    }
}


// Func to multiply two PSF's
void Mult(struct PSF* PSF1, struct PSF* PSF2, struct PSF* PSF3){
    // x + y
    // z + u
    //
    // x*z + x*u + y*z + y*u

    PSF3->elems_num_ = 0;

    for(int i = 0; i < PSF1->elems_num_; ++i)
    {
        for(int j = 0; j < PSF2->elems_num_; ++j)
        {
            PSF3->elems_[PSF3->elems_num_] = PSF1->elems_[i];
            PSF3->elems_[PSF3->elems_num_].num_mult_ *= PSF2->elems_[j].num_mult_;

            for(int p = 0; p < PSF2->elems_[j].num_of_variables; ++p)
            {
                PSF3->elems_[PSF3->elems_num_].variables_[ PSF3->elems_[PSF3->elems_num_].num_of_variables ] = PSF2->elems_[j].variables_[p];
                PSF3->elems_[PSF3->elems_num_].num_of_variables += 1;
            }

            PSF3->elems_num_ += 1;
        }
    }


    for(int i = 0; i < PSF3->elems_num_ - 1; ++i)
    {
        for(int j = i + 1; j < PSF3->elems_num_; ++j)
        {
            //printf("i: %d, j: %d\n", i, j);
            if( StringsEqual( PSF3->elems_[i].variables_, PSF3->elems_[j].variables_, 
                              PSF3->elems_[i].num_of_variables, PSF3->elems_[j].num_of_variables ) )
            {
                //printf("i: %d, j: %d\n", i, j);
                if( (PSF3->elems_[i].num_mult_ + PSF3->elems_[j].num_mult_) == 0 )
                {
                    PSF3->elems_[i] = PSF3->elems_[PSF3->elems_num_ - 1];
                    PSF3->elems_num_ -= 1;
                    PSF3->elems_[j] = PSF3->elems_[PSF3->elems_num_ - 1];
                    PSF3->elems_num_ -= 1;
                }
                else
                {
                    PSF3->elems_[i].num_mult_ += PSF3->elems_[j].num_mult_;
                    PSF3->elems_[j] = PSF3->elems_[PSF3->elems_num_ - 1];
                    PSF3->elems_num_ -= 1;
                }
            }
        }
    }
}


// Func to divide PSF1 into PSF2
// (PSF2 can't have more than one element)
int Div(struct PSF* PSF1, struct PSF* PSF2){
    if(PSF2->elems_num_ != 1) 
    {
        printf("error\n");
        return 0;
    }

    for(int i = 0; i < PSF1->elems_num_; ++i)
    {
        if( (PSF1->elems_[i].num_mult_ % PSF2->elems_[0].num_mult_) != 0 ) // check if we divide without remainder
        {
            printf("error\n");
            return 0;
        }

        PSF1->elems_[i].num_mult_ = PSF1->elems_[i].num_mult_ / PSF2->elems_[0].num_mult_;

        
        for(int j = 0; j < PSF2->elems_[0].num_of_variables; ++j)
        {
            bool found = false; // flag to check if we found the needed elemen tin PSF1
                                // if we hadm't we throw an error

            for(int p = 0; p < PSF1->elems_[i].num_of_variables; ++p)
            {
                if( PSF1->elems_[i].variables_[p] == PSF2->elems_[0].variables_[j])
                {
                    PSF1->elems_[i].variables_[p] = PSF1->elems_[i].variables_[PSF1->elems_[i].num_of_variables - 1];
                    PSF1->elems_[i].num_of_variables -= 1;
                    found = true;
                    break;
                }
            }
            
            if(!found)
            {
                printf("error\n");
                return 0;
            }
        }
    }

    return 1;
}


// func to check if PSF1 equals to PSF2
void Eqal(struct PSF* PSF1, struct PSF* PSF2){
    if(PSF1->elems_num_ != PSF2->elems_num_) // different size -> not equal
    {   
        printf("NOT equal\n");
        return;
    }

    size_t counter = 0;

    for(int i = 0; i < PSF1->elems_num_; ++i)
    {
        for(int j = 0; j < PSF2->elems_num_; ++j)
        {
            if(StringsEqual(PSF1->elems_[i].variables_, PSF2->elems_[j].variables_, 
                            PSF1->elems_[i].num_of_variables, PSF2->elems_[j].num_of_variables) &&
                            PSF1->elems_[i].num_mult_ == PSF2->elems_[j].num_mult_)
                {
                    counter++; // for every pair of MultElem's we check if they are equal
                }
        }
    }

    if(counter != PSF2->elems_num_){ // not enough equal pairs -> not equal
        printf("NOT equal\n");
        return;
    }

    printf("equal\n");
    return;
}



int main(void){
    struct PSF PSF1, PSF2, PSF3;
    char action = '\0';

    PSF1.size_ = BUF_LENGTH;
    PSF1.buf_ = (char*)malloc(PSF1.size_);

    PSF2.size_ = BUF_LENGTH;
    PSF2.buf_ = (char*)malloc(PSF2.size_);

    //printf("Enter action:\n");

    action = getchar();
    while (getchar() != '\n');

    //printf("Enter PSF1:\n");

    PSF1.length_ = getline(&PSF1.buf_, &PSF1.size_, stdin);
    if(PSF1.length_ == -1)
    {
        perror("Getline error");
        exit(EXIT_FAILURE);
    }
    
    LineParse(&PSF1);
    
    //printf("Enter PSF2:\n");

    PSF2.length_ = getline(&PSF2.buf_, &PSF2.size_, stdin);
    if(PSF2.length_ == -1)
    {
        perror("Getline error");
        exit(EXIT_FAILURE);
    }

    LineParse(&PSF2);

    int success;

    switch(action)
    {
        case '+' :
            Add(&PSF1, &PSF2);
            Print(&PSF1);
            break;
        case '-' :
            Substract(&PSF1, &PSF2);
            Print(&PSF1);
            break;
        case '*' :
            Mult(&PSF1, &PSF2, &PSF3);
            Print(&PSF3);
            break;
        case '/' :
            success = Div(&PSF1, &PSF2);
            if(success > 0) Print(&PSF1);
            break;
        case '=' :
            Eqal(&PSF1, &PSF2);
            break;
    }

    free(PSF1.buf_);
    free(PSF2.buf_);

    return 0;
}
