#include "Akinator.hpp"
#include "../Shared/Tools.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <search.h>

#define GameEvent 1

int answComparator(const void* akinAnsw1, const void* akinAnsw2)
{
    Answer* answ1 = (Answer*)akinAnsw1;
    Answer* answ2 = (Answer*)akinAnsw2;
    return doubleCompare(answ2->probability, answ1->probability);
}

void initQuestions(Akinator* akinator)
{
    FILE* queIn = fopen(INPUT_QUESTIONS_FILE_NAME, "r+");
    assert(queIn && "Wrong input filename");
    fseek(queIn, 0, SEEK_END);
    size_t bytes = ftell(queIn);
    rewind(queIn);
    char* queStr = (char*)safeCalloc(bytes + 1, sizeof(char));
    fread(queStr, sizeof(char), bytes, queIn);

    size_t queCount = 0;
    sscanf(queStr, "%zu", &queCount);
    akinator->qCount = queCount;
    queStr = strchr(queStr, '\n') + 1;

    akinator->questions = (questionDataBase*)safeCalloc(queCount + 1, sizeof(questionDataBase));
    for (size_t k = 0; k < queCount; ++k)
    {
        char* question = (char*)safeCalloc(50, sizeof(char));
        sscanf(queStr, "%[^/]", question);
        queStr = strchr(queStr, '/') + 1;
        size_t id;
        sscanf(queStr, "%zu", &id);
        akinator->questions[k].id = id;
        akinator->questions[k].question = question;
        akinator->questions[k].isAsked = 0;
        queStr = strchr(queStr, '\n') + 1;
    }
}

void initAnwsers(Akinator* akinator)
{
    FILE* answIn = fopen(INPUT_ANSWERS_FILE_NAME, "r+");
    assert(answIn && "Wrong input filename");
    fseek(answIn, 0, SEEK_END);
    size_t bytes = ftell(answIn);
    rewind(answIn);
    char* answStr = (char*)safeCalloc(bytes + 1, sizeof(char));
    fread(answStr, sizeof(char), bytes, answIn);
    parsAkinAnsws(akinator, answStr);
}

void parsAkinAnsws(Akinator* akinator, char* answStr)
{
    sscanf(answStr, "%zu", &akinator->answCount);
    answStr = strchr(answStr, '\n') + 1;
    akinator->answers = (Answer*)safeCalloc(akinator->answCount, sizeof(Answer));
    for (size_t i = 0; i < akinator->answCount; ++i)
    {
        skipDelims(&answStr);
        char* name = (char*)safeCalloc(20, sizeof(char));
        sscanf(answStr, "%[^/]", name);
        answStr += strlen(name) + 1;
        akinator->answers[i].name = name;
        akinator->answers[i].queCount = 0;

        double popularity = 0.0;
        sscanf(answStr, "pop:%lf,", &popularity);
        akinator->answers[i].probability = popularity/(double)akinator->answCount;
        answStr = strchr(answStr, '{');

        size_t capacity = 20;
        akinator->answers[i].personalQuestions = (Question*)safeCalloc(capacity, sizeof(Question));
        size_t curlenansw = 0;
        bool newAnsw = 0;
        while (!newAnsw)
        {
            akinator->answers[i].queCount++;
            size_t id = 0, y = 0, n = 0, dm = 0;
            sscanf(answStr, "{%zu, %zu, %zu, %zu}", &id, &y, &n, &dm);
            akinator->answers[i].personalQuestions[curlenansw].yes_answ = y;
            akinator->answers[i].personalQuestions[curlenansw].no_answ = n;
            akinator->answers[i].personalQuestions[curlenansw].dm_answ = dm;
            akinator->answers[i].personalQuestions[curlenansw].id = id;
            answStr = *(strchr(answStr, '}') + 1) != '\n'? strchr(answStr + 1, '{') : strchr(answStr, '}') + 1;
            if (!answStr) {break;}
            else {newAnsw = *answStr == '\n'? 1 : 0;}
            if (++curlenansw + 1 == capacity) 
            {
                capacity *= 2;
                akinator->answers[i].personalQuestions = (Question*)realloc(akinator->answers[i].personalQuestions, capacity);
            }
        }
    }
}

Akinator* initAkinator()
{
    Akinator* akinator = (Akinator*)safeCalloc(1, sizeof(Akinator));
    initAnwsers(akinator);
    initQuestions(akinator);
    akinator->requiredDiff = 3.0/akinator->answCount;
    return akinator;
}

void sortAnswByProb(Akinator* akinator)
{   
    qsort(akinator->answers, akinator->answCount, sizeof(Answer), answComparator);
}

#define curId akinator->answers[0].personalQuestions[j].id

void askQuestion(Akinator* akinator)
{
    int count = 3;
    for (size_t j = 0; j < akinator->answers[0].queCount && count; ++j)
    {
        int tmp = curId;
        if (akinator->questions[curId].isAsked == 1) {continue;}
        printf("%s\n", akinator->questions[curId].question);
        int userAnswer = getUserAnswer();

        switch (userAnswer)
        {
        case NEGATIVE:
            akinator->answers[0].personalQuestions[j].no_answ++;
            break;
        case DM_ANSWER:
            akinator->answers[0].personalQuestions[j].dm_answ++;
            break;
        case POSITIVE:
            akinator->answers[0].personalQuestions[j].yes_answ++;
            break;
        default:
            assert(0 && "Smells like shit");
            break;
        }

        akinator->questions[curId].isAsked = 1;
    }
    akinator->answers[0].probability -= 0.5;
}

#undef curId

int getUserAnswer()
{
    char* userInput;
    scanf("%ms", &userInput);
    size_t len = strlen(userInput);
    for (size_t k = 0; k < len; ++k)
        userInput[k] = tolower(userInput[k]);
    if(!strcmp("yes", userInput)) {return POSITIVE;}
    if(!strcmp("no", userInput)) {return NEGATIVE;}
    if(!strcmp("dm", userInput)) {return DM_ANSWER;}
    assert(0 && "Wrong input answer");
}


void doAkinator(Akinator* akinator)
{
    size_t id;
    int tmp = 10;
    while(GameEvent && tmp--)
    {
        sortAnswByProb(akinator);
        askQuestion(akinator);
    }
    printf("%s", akinator->answers[1].name);
}
