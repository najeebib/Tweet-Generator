#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>

#define MAX_WORDS_IN_SENTENCE_GENERATION 20
#define MAX_WORD_LENGTH 100
#define MAX_SENTENCE_LENGTH 1000

typedef struct WordStruct {
  char *word;
  struct WordProbability *prob_list;
  int sizeOfList;
  int count;
} WordStruct;

typedef struct WordProbability {
  struct WordStruct *word_struct_ptr;
  int numOfOccurences;
} WordProbability;

/************ LINKED LIST ************/
typedef struct Node {
  WordStruct *data;
  struct Node *next;
} Node;

typedef struct LinkList {
  Node *first;
  Node *last;
  int size;
} LinkList;

/**
 * Add data to new node at the end of the given link list.
 * @param link_list Link list to add data to
 * @param data pointer to dynamically allocated data
 * @return 0 on success, 1 otherwise
 */
int add(LinkList *link_list, WordStruct *data)
{
  Node *new_node = malloc(sizeof(Node));
  if (new_node == NULL)
  {
	return 1;
  }
  *new_node = (Node){data, NULL};

  if (link_list->first == NULL)
  {
	link_list->first = new_node;
	link_list->last = new_node;
  }
  else
  {
	link_list->last->next = new_node;
	link_list->last = new_node;
  }

  link_list->size++;
  return 0;
}
/*************************************/

/**
 * Get random number between 0 and max_number [0, max_number).
 * @param max_number
 * @return Random number
 */
int get_random_number(int max_number)
{
  return((rand() % max_number)); 
}

int checkIfWordIsLast(char *str)//check if the word ends with a point ('.')
{
  if(str == NULL)
  {
    return 1;
  }
  int size = strlen(str);
  for(int i = 0; i< size;i++)
  {
    if(str[i] == '.')
    {
      return 0;
    }
  }
  return 1;
}

int getNumOfWords(FILE *fp)//return the number of words in the file
{
  int count = 0;
  char ch;
  while ((ch = fgetc(fp)) != EOF)
  {
        if (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\0')//if the char is one of these then we have a new word, increment the counter
            count++;
  }
  return count;
}

/**
 * Choose randomly the next word from the given dictionary, drawn uniformly.
 * The function won't return a word that end's in full stop '.' (Nekuda).
 * @param dictionary Dictionary to choose a word from
 * @return WordStruct of the chosen word
 */
WordStruct *get_first_random_word(LinkList *dictionary)
{
  
  if(dictionary == NULL)
  {
    return NULL;
  }
  int seed = dictionary->size;//the random number to be choosen is betweem 0 and the dictionary size
  int index = get_random_number(seed);//get the random number
  Node *ptr = dictionary->first;
  int i =0;
  while(ptr != NULL)//go through the list to find the random word that was choosen
  {
    if(i == index)
    {
      return ptr->data;
    }
    i++;
    ptr = ptr->next;
  }
  
}

/**
 * Choose randomly the next word. Depend on it's occurrence frequency
 * in word_struct_ptr->WordProbability.
 * @param word_struct_ptr WordStruct to choose from
 * @return WordStruct of the chosen word
 */
WordStruct *get_next_random_word(WordStruct *word_struct_ptr)
{  
  if(word_struct_ptr == NULL)
  {
    return NULL;
  }
  if(word_struct_ptr->prob_list == NULL)//if the words has no words after it
  {
    return NULL;
  }
  char** words = (char**)malloc(word_struct_ptr->count*sizeof(char*));
  
  int i=0;
  for(int j=0;j<word_struct_ptr->sizeOfList&&i<word_struct_ptr->count;j++)
  {
    for(int k=0;k<word_struct_ptr->prob_list[j].numOfOccurences&&i<word_struct_ptr->count;k++)
    {
      words[i] = (char*)malloc(strlen(word_struct_ptr->prob_list[j].word_struct_ptr->word)*sizeof(char));
      strcpy(words[i],word_struct_ptr->prob_list[j].word_struct_ptr->word);
      i++;
    }
  }
  int index =get_random_number(word_struct_ptr->count);
  char* tok = words[index];
  for(int i=0;i<word_struct_ptr->sizeOfList;i++)
  {
    if(strcmp(tok,word_struct_ptr->prob_list[i].word_struct_ptr->word) == 0)
      return(word_struct_ptr->prob_list[i].word_struct_ptr);
  }
  for(int i=0;i<word_struct_ptr->count;i++)
  {
    free(words[i]);
  }
  free(words);
}

/**
 * Receive dictionary, generate and print to stdout random sentence out of it.
 * The sentence most have at least 2 words in it.
 * @param dictionary Dictionary to use
 * @return Amount of words in printed sentence
 */
int generate_sentence(LinkList *dictionary)
{
  if(dictionary == NULL)
  {
    return 0;
  }
  char *str= (char*)malloc(MAX_SENTENCE_LENGTH*sizeof(char));
  if(str == NULL)
  {
    exit(EXIT_FAILURE);
  }
  int count = 0;
  int size = 0;
  WordStruct *ptr = get_first_random_word(dictionary); 
  while(ptr == NULL || ptr->prob_list == NULL)//if the word is null or it is a word at the end of a sentince then get a new valid word
  {
    ptr = get_first_random_word(dictionary);
  }
  strcpy(str,ptr->word);//copy said word to the string
  size += strlen(ptr->word);//update size
  count++;//update count
  int i =1;
  while(i < MAX_WORDS_IN_SENTENCE_GENERATION && ptr != NULL)//this while keeps going until we reach the end of the prob list or we reach the max amount of words allowed
  {
    if((size + 1) > MAX_SENTENCE_LENGTH)//if we reached the maximum size of sentence allowed stop adding and print
    {
      printf("%s",str);
      if(strstr(str,"\n") == NULL)
        printf("\n");
      free(str);
      return count;
    }
    if(strstr(str,".") == NULL)
      strcat(str," ");
    ptr = get_next_random_word(ptr);//get next word
    if(ptr != NULL)
    {
      if((size + strlen(ptr->word)) > MAX_SENTENCE_LENGTH)
      {
        printf("%s",str);
        if(strstr(str,"\n") == NULL)
        printf("\n");
        free(str);
        return count;
      }
      else{
          strcat(str,ptr->word);
          count++;
      }
      
    }
    i++;
  }
  printf("%s",str);
  if(strstr(str,"\n") == NULL)
        printf("\n");
  free(str);
  return count;
}



/**
 * Gets 2 WordStructs. If second_word in first_word's prob_list,
 * update the existing probability value.
 * Otherwise, add the second word to the prob_list of the first word.
 * @param first_word
 * @param second_word
 * @return 0 if already in list, 1 otherwise.
 */
int add_word_to_probability_list(WordStruct *first_word,
                                 WordStruct *second_word)
{
  if(checkIfWordIsLast(first_word->word) == 0 || first_word == NULL || second_word == NULL)//if the first word is a word that ends with '.' dont add to its prob list
  {
    return 1;
  }
  int size = first_word->sizeOfList;//size of the list
  if(first_word->prob_list == NULL)//if the list is empty initilize it
  {
    size++;
    first_word->prob_list =(WordProbability*)realloc(first_word->prob_list,(size*sizeof(WordProbability)));
    if(first_word->prob_list == NULL)
    {
      exit(EXIT_FAILURE);
    }
    first_word->prob_list[size-1].numOfOccurences = 1;
    first_word->prob_list[size-1].word_struct_ptr = second_word;
    (first_word->sizeOfList)+=1;
    return 1;
  }
  else{//if the list is not empty update it
    for(int i=0;i<size;i++)
    {
      if(strcmp(first_word->prob_list[i].word_struct_ptr->word,second_word->word) == 0)//if the word occures in the list update counter
      {
        (first_word->prob_list[i].numOfOccurences)+=1;
        return 0;
      }
    }
    //add new word to list
    size++;
    first_word->prob_list =(WordProbability*)realloc(first_word->prob_list,((size+1)*sizeof(WordProbability)));
    if(first_word->prob_list == NULL)
    {
      exit(EXIT_FAILURE);
    }
    if(first_word->prob_list == NULL)
    {
      exit(EXIT_FAILURE);
    }
    first_word->prob_list[size-1].numOfOccurences = 1;
    first_word->prob_list[size-1].word_struct_ptr = second_word;
    (first_word->sizeOfList)+=1;
    return 1;
  }
  
}
//search the dictionary for a word and return a pointer to it if it exist, otherwise return null
WordStruct* searchDic(LinkList *dictionary, char *token){
  Node *ptr= dictionary->first;
  while(ptr != NULL)
  {
    if(strcmp(ptr->data->word,token) == 0){
      return ptr->data;
    }
      
    ptr = ptr->next;
  }
  return NULL;
}

/**
 * Read word from the given file. Add every unique word to the dictionary.
 * Also, at every iteration, update the prob_list of the previous word with
 * the value of the current word.
 * @param fp File pointer
 * @param words_to_read Number of words to read from file.
 *                      If value is bigger than the file's word count,
 *                      or if words_to_read == -1 than read entire file.
 * @param dictionary Empty dictionary to fill
 */


void fill_dictionary(FILE *fp, int words_to_read, LinkList *dictionary)
{
  char *str = (char*) calloc(MAX_SENTENCE_LENGTH,sizeof(char));
  if(str == NULL)
  {
    exit(EXIT_FAILURE);
  }
  char *token = (char*)calloc(MAX_WORD_LENGTH,sizeof(char));
  if(token == NULL)
  {
    exit(EXIT_FAILURE);
  }
  int i=0;
  WordStruct *prev = NULL;//pointer to the previous word
  while(fgets(str,MAX_SENTENCE_LENGTH,fp) && i< words_to_read)//get the string from the file line by line
  {
    token = strtok(str," ");
    
    while( token != NULL && i < words_to_read)//if we havent reached the end of the file or the number limit of words
    {
     
      if(searchDic(dictionary,token)== NULL)//search if this word exists in the dictionary
      {
        WordStruct *word =(WordStruct*)malloc(sizeof(WordStruct));
        if(word == NULL )
        {
          exit(EXIT_FAILURE);//malloc error
        }
        if(strlen(token) <= MAX_WORD_LENGTH)//check if word is smaller than the max size
        {
          word->word = (char*)calloc(MAX_WORD_LENGTH,sizeof(char));//initlize the wordStruct then add it to list
          if(word->word == NULL)
          {
            exit(EXIT_FAILURE);
          }
          strcpy(word->word,token);
          word->count=1;
          word->prob_list =NULL;
          word->sizeOfList=0;
          
          
          add(dictionary,word);//add to dictionary
          
          if(prev == NULL)
          {
            prev = word;//pointer to the previous word
          }  
          else{
        
            if(checkIfWordIsLast(prev->word) == 1)//if the last word is not the end of a sentence uppdate its prob list
            {
              add_word_to_probability_list(prev,word);
            }
            prev = word;
          }
        }
      
      
      }
      else{
        WordStruct *ptr = searchDic(dictionary,token);
        (ptr->count) +=1;//update the word counter
        if(prev == NULL)
        {
          prev = ptr;
        }  
        else{
          if(checkIfWordIsLast(prev->word) == 1)//if the last word is not the end of a sentence uppdate its prob list
          {
            add_word_to_probability_list(prev,ptr);
          }
          
          prev = ptr;
        }
      }
      token = strtok(NULL, " ");
      i++;
    }
    if(i >= words_to_read){//if we read the maximum amount of words we are allowed to read exit
        return;
    }
    
  }
  free(str);
  free(token);
}

/**
 * Free the given dictionary and all of it's content from memory.
 * @param dictionary Dictionary to free
 */
void free_dictionary(LinkList *dictionary) {
  Node *ptr =dictionary->first;
  Node *next;
  while(ptr != NULL) {
    if(ptr->data->sizeOfList != 0) { 
      free(ptr->data->prob_list);
    }
    free(ptr->data->word);
    
    next = ptr->next;
    free(ptr->data);
    free(ptr);
    ptr = next;
  }
 free(dictionary);
}

/**
 * @param argc
 * @param argv 1) Seed
 *             2) Number of sentences to generate
 *             3) Path to file
 *             4) Optional - Number of words to read
 */
int main(int argc, char *argv[])
{
  if(argc < 4 || argc > 5)
  {
    printf("Usage: bad arguments");
    exit(EXIT_FAILURE);
  }
  
  int seed =atoi(argv[1]);//the seed for the rand() functiob
  int numOfSenteces = atoi(argv[2]);
  
  char path[strlen(argv[3])];
  strcpy(path,argv[3]);
  int numOfWords = 0;
  FILE *fp = fopen(path , "r");
  if(fp == NULL)
  {
    printf("Error: file doen't exist");
    exit(EXIT_FAILURE);
  }
  if(argc == 5)
  {
    numOfWords = atoi(argv[4]);
  }
  
  else{
    numOfWords = getNumOfWords(fp);
  }
  LinkList *Dictionary = (LinkList*)malloc(sizeof(LinkList));
  if(Dictionary == NULL )
  {
    exit(EXIT_FAILURE);
  }
  srand(seed);
  fseek(fp, 0, SEEK_SET);

  fill_dictionary(fp,numOfWords,Dictionary);
  fclose(fp);
  for(int i = 0;i< numOfSenteces;i++)
  {
    printf("Tweet %d: ",i+1);
    generate_sentence(Dictionary);
  }
  free_dictionary(Dictionary);
  return 0;
}
