#include <stdio.h>
#include <stdlib.h>
#define INITIAL_BUFFER_SIZE 10
#define MAX_WORD_LENGTH 50
#define OUTPUT_WORDS 25
#define STOP_WORD_COUNT 11

typedef struct frequency_pair{
  char word[MAX_WORD_LENGTH];
  int frequency;
  int wordLength;
} freq;

char *stopWords[STOP_WORD_COUNT] = {"in", "for", "the", "is", "of", "and", "at", "by", "to", "was", "in"};

int main(int argc, char **argv){
    int bufferSize = INITIAL_BUFFER_SIZE;
    char *input;
    input = (char *) malloc(sizeof(char) * bufferSize);

    int frequenciesLength = 0;
    freq *frequencies;
    frequencies = (freq *) calloc(frequenciesLength, sizeof(freq));

    FILE *inputFile;
    inputFile = fopen(argv[1], "r");
    if(inputFile == NULL) return 1;

    int count = 0, wordStart = 0;
    char inputChar;
    read_character:
      inputChar = fgetc(inputFile);
      if(inputChar <= 'Z' && inputChar >= 'A') inputChar += 'a' - 'A';

      if (count >= bufferSize) {
        bufferSize *= 2;
        input = realloc(input, sizeof(char) * bufferSize);
      }
      input[count] = inputChar;
      count++;

      if (!(inputChar <= 'z' && inputChar >= 'a')) {
        int wordLength = count - wordStart;
        if(wordLength <= 1) {
          if(inputChar == EOF) goto end_reading;
          wordStart = count;
          goto read_character;
        }

        char inputWord[wordLength];
        int i = 0;
        fill_input_word:
          if(i >= wordLength - 1) goto fill_input_word_end;
          inputWord[i] = *(input + wordStart + i);
          i++;
          goto fill_input_word;
        fill_input_word_end:
        inputWord[wordLength - 1] = '\0';
        wordStart = count;
        
        unsigned existsAlready = 0;
        int i1 = 0;
        frequency_exists_already:
          if(i1 >= frequenciesLength) goto frequency_exists_already_end;
          if (frequencies[i1].wordLength != wordLength) {
            i1++;
            goto frequency_exists_already;
          }
          int j = 0;
          compare:
            if(j >= wordLength) goto compare_end;
            existsAlready = frequencies[i1].word[j] == inputWord[j];
            if(!existsAlready) goto compare_end;
            j++;
            goto compare;
          compare_end:

          if(existsAlready) {
            frequencies[i1].frequency++;
            goto frequency_exists_already_end;
          }
          i1++;
          goto frequency_exists_already;
        frequency_exists_already_end:;

        int isStopWord = 0;
        int i2 = 0;
        stop_check:
          if(i2 >= STOP_WORD_COUNT) goto stop_check_end;

          int j2 = 0;
          compare2:
            isStopWord = stopWords[i2][j2] == inputWord[j2];
            if(!isStopWord) goto compare2_end;
            if(stopWords[i2][j2] == '\0' || inputWord[j2] == '\0') goto compare2_end;

            j2++;
            goto compare2;
          compare2_end:

          if(isStopWord) goto stop_check_end;
          i2++;
          goto stop_check;
        stop_check_end:

        if (existsAlready) goto read_character;
        if (isStopWord) goto read_character;
        
        frequencies = realloc(frequencies, (frequenciesLength + 1) * sizeof(freq));
        int k = 0;
        new_frequency_entry:
          if (k >= wordLength) goto new_frequency_entry_end;
          frequencies[frequenciesLength].word[k] = inputWord[k];
          k++;
          goto new_frequency_entry;
        new_frequency_entry_end:

        frequencies[frequenciesLength].frequency = 1;
        frequencies[frequenciesLength].wordLength = wordLength;
        frequenciesLength++;

        if(inputChar == EOF) goto end_reading;
      }
      goto read_character;
    end_reading: ;

    int i = 0;
    outer_bubble:
      if(i >= frequenciesLength - 1) goto outer_bubble_end;
      int j = 0;
      inner_bubble:
        if(j >= frequenciesLength - i - 1) goto inner_bubble_end;
        if (frequencies[j].frequency < frequencies[j + 1].frequency) {
          struct frequency_pair temp = frequencies[j];
          frequencies[j] = frequencies[j + 1];
          frequencies[j + 1] = temp;
        }
        j++;
        goto inner_bubble;
      inner_bubble_end:
      i++;
      goto outer_bubble;
    outer_bubble_end:
    free(input);
    fclose(inputFile);

    int k = 0;
    output_frequency:
      if(k >= OUTPUT_WORDS || k >= frequenciesLength) goto end_output;
      printf("%s - %d\n", frequencies[k].word, frequencies[k].frequency);
      k++;
      goto output_frequency;
    end_output:
    return 0;
}