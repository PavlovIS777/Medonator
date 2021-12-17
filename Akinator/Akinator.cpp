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
        parseAnswIdforDB(akinator, id);
        akinator->questions[k].id = id;
        akinator->questions[k].question = question;
        akinator->questions[k].isAsked = 0;
        queStr = strchr(queStr, '\n') + 1;
    }
}

void parseAnswIdforDB(Akinator* akinator, size_t qeuId)
{
    size_t answIdCnt = 10;
    size_t size = 0;
    akinator->questions[qeuId].answId = (size_t*)safeCalloc(answIdCnt, sizeof(size_t));
    for (size_t i = 0; i < akinator->answCount; ++i)
    {
        for(size_t k = 0; k < akinator->answers[i].queCount; ++k)
        {
            if(akinator->answers[i].personalQuestions[k].id == qeuId)
            {
                akinator->questions[qeuId].answId[size++] = i;
            }
            if (size == answIdCnt) 
            {
                answIdCnt *= 2;
                realloc(akinator->questions[qeuId].answId, answIdCnt);
            }
        }
    }
    akinator->questions[qeuId].answCnt = size;
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
    parseAkinAnsws(akinator, answStr);
}

void parseAkinAnsws(Akinator* akinator, char* answStr)
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

void incPositiveAnsw(Akinator* akinator, size_t queId)
{
    for (size_t i = 0; i < akinator->answCount; ++i)
    {
        for (size_t k = 0; k < akinator->answers[i].queCount; ++k)
        {
            if (akinator->answers[i].personalQuestions[k].id == queId)
            {
                akinator->answers[i].personalQuestions[k].yes_answ++;
                break;
            }
        }
    }
}

void incNegativeAnsw(Akinator* akinator, size_t queId)
{
    for (size_t i = 0; i < akinator->answCount; ++i)
    {
        for (size_t k = 0; k < akinator->answers[i].queCount; ++k)
        {
            if (akinator->answers[i].personalQuestions[k].id == queId)
            {
                akinator->answers[i].personalQuestions[k].no_answ++;
                break;
            }
        }
    }
}

void incDMAnsw(Akinator* akinator, size_t queId)
{
    for (size_t i = 0; i < akinator->answCount; ++i)
    {
        for (size_t k = 0; k < akinator->answers[i].queCount; ++k)
        {
            if (akinator->answers[i].personalQuestions[k].id == queId)
            {
                akinator->answers[i].personalQuestions[k].dm_answ++;
                break;
            }
        }
    }
}

#define curId akinator->answers[0].personalQuestions[j].id

void askQuestion(Akinator* akinator)
{
    int count = 3;
    for (size_t j = 0; j < akinator->answers[0].queCount && count--; ++j)
    {
        int tmp = curId;
        if (akinator->questions[curId].isAsked == 1) {continue;}
        printf("%s\n", akinator->questions[curId].question);
        int userAnswer = getUserAnswer();

        switch (userAnswer)
        {
        case NEGATIVE:
            incNegativeAnsw(akinator, akinator->answers[0].personalQuestions[j].id);
            break;
        case DM_ANSWER:
            incDMAnsw(akinator, akinator->answers[0].personalQuestions[j].id);
            break;
        case POSITIVE:
            incPositiveAnsw(akinator, akinator->answers[0].personalQuestions[j].id);
            break;
        default:
            assert(0 && "Smells like shit");
            break;
        }

        akinator->questions[curId].isAsked = 1;
    }
    
}

#undef curId

int getMostProbablyAnswer(Akinator* akinator)
{
    size_t id;
    double maxProb = 0;
    for (size_t j = 0; j < akinator->answCount; ++j)
    {
        if (maxProb < akinator->answers[j].probability)
        {
            maxProb = akinator->answers[j].probability;
            id = j;
        }
    }

    return id;
}

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
