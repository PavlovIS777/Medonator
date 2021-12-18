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
    akinator->questions[qeuId].answIds = (dictionary*)safeCalloc(answIdCnt, sizeof(dictionary));
    for (size_t i = 0; i < akinator->answCount; ++i)
    {
        for(size_t k = 0; k < akinator->answers[i].queCount; ++k)
        {
            if(akinator->answers[i].personalQuestions[k].id == qeuId)
            {
                akinator->questions[qeuId].answIds[size].key = i;
                akinator->questions[qeuId].answIds[size++].value = k;
                break;
            }
        }
        if (size >= answIdCnt) 
            {
                answIdCnt *= 2;
                realloc(akinator->questions[qeuId].answIds, answIdCnt*sizeof(dictionary));
            }
    }
    realloc(akinator->questions[qeuId].answIds, size*sizeof(dictionary));
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
    size_t gamesCount = 0;
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
        akinator->answers[i].probability = popularity;
        gamesCount += popularity;
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
    for (size_t h = 0; h < akinator->answCount; ++h)
        akinator->answers[h].probability /= gamesCount;
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

#define answId akinator->questions[queId].answIds[i].key
#define quePos akinator->questions[queId].answIds[i].value

void incPositiveAnsw(Akinator* akinator, size_t queId)
{
    for (size_t i = 0; i < akinator->questions[queId].answCnt; ++i)
    {
        akinator->answers[answId].personalQuestions[quePos].yes_answ++;
    }
}

void incNegativeAnsw(Akinator* akinator, size_t queId)
{
    for (size_t i = 0; i < akinator->questions[queId].answCnt; ++i)
    {
        akinator->answers[answId].personalQuestions[quePos].no_answ++;
    }
}

void incDmAnsw(Akinator* akinator, size_t queId)
{
    for (size_t i = 0; i < akinator->questions[queId].answCnt; ++i)
    {
        akinator->answers[answId].personalQuestions[quePos].dm_answ++;
    }
}
#undef quePos
#undef answId

#define curId akinator->answers[maxProbId].personalQuestions[j].id

void askQuestion(Akinator* akinator, size_t maxProbId)
{
    int count = 3;
    bool queshaned = 0;
    for (size_t j = 0; j < akinator->answers[maxProbId].queCount && count--; ++j)
    {
        if (akinator->questions[curId].isAsked == 1) {continue;}
        queshaned = 1;
        printf("%s\n", akinator->questions[curId].question);
        int userAnswer = getUserAnswer();

        switch (userAnswer)
        {
        case NEGATIVE:
            incNegativeAnsw(akinator, akinator->answers[maxProbId].personalQuestions[j].id);
            break;
        case DM_ANSWER:
            incDmAnsw(akinator, akinator->answers[maxProbId].personalQuestions[j].id);
            break;
        case POSITIVE:
            incPositiveAnsw(akinator, akinator->answers[maxProbId].personalQuestions[j].id);
            break;
        default:
            assert(0 && "Smells like shit");
            break;
        }
        recountProbability(akinator, curId, userAnswer);
        akinator->questions[curId].isAsked = 1;
    }
    if(!queshaned && maxProbId + 1 < akinator->answCount) {askQuestion(akinator, maxProbId + 1);}
    else {return;}    
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


#define quePos akinator->questions[queId].answIds[counter].value
#define getAnswField(enumAnsw) *(&(akinator->answers[j].personalQuestions[quePos].id) + enumAnsw+2)
#define sumOfField akinator->answers[j].personalQuestions[quePos].yes_answ + akinator->answers[j].personalQuestions[quePos].no_answ \
                    + akinator->answers[j].personalQuestions[quePos].dm_answ
void recountProbability(Akinator* akinator, size_t queId, size_t enumAnsw)
{
    size_t counter = 0;
    for (size_t k = 0, j = akinator->questions[queId].answIds[counter].key; k < akinator->answCount; ++k)
    {
        if (k == j && counter < akinator->questions[queId].answCnt)
        {
            double fieldVal = (double)getAnswField(enumAnsw);
            double sum = (double)sumOfField;
            akinator->answers[j].probability *= fieldVal/sum;
            j = akinator->questions[queId].answIds[++counter].key;
        } 
        else
        {
            akinator->answers[k].probability *= 1.0/3;
        }
    }
    printAnswrsWithProbability(akinator);
}

double normProbab(Akinator* akinator, size_t pos)
{
    double norm = 0;
    for (size_t k = 0; k < akinator->answCount; ++k)
    {
        norm += akinator->answers[k].probability;
    }
    norm = 1/norm;
    double normalizedProbab = 100*(akinator->answers[pos].probability*norm);
    return normalizedProbab;
}

void printAnswrsWithProbability(Akinator* akinator)
{
    printf("===============================\n");
    for (size_t i = 0; i < akinator->answCount; ++i)
    {
        printf("name: %s, prob: %lf %\n", akinator->answers[i].name, normProbab(akinator, i));
    }
    printf("===============================\n");
}

void doAkinator(Akinator* akinator)
{
    size_t maxProbId = getMostProbablyAnswer(akinator);
    size_t qCount = akinator->qCount;
    printAnswrsWithProbability(akinator);
    while(qCount--)
    {
        askQuestion(akinator, maxProbId);
        printAnswrsWithProbability(akinator);
        maxProbId = getMostProbablyAnswer(akinator);
        if (normProbab(akinator, maxProbId) >= 95) 
        {
            printf("%s", akinator->answers[maxProbId].name);
            return;
        }
    }
    printf("%s", akinator->answers[maxProbId].name);
}
