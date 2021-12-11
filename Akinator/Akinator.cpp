#include "Akinator.hpp"
#include "../Shared/Tools.h"
#include <stdio.h>
#include <string.h>

#define GameEvent 1

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
        akinator->answers[i].count = 0;

        double popularity = 0.0;
        sscanf(answStr, "pop:%lf,", &popularity);
        akinator->answers[i].q_prob = popularity/(double)akinator->answCount;
        answStr = strchr(answStr, '{');

        size_t capacity = 20;
        akinator->answers[i].personalQuestions = (Question*)safeCalloc(capacity, sizeof(Question));
        size_t curlenansw = 0;
        bool newAnsw = 0;
        while (!newAnsw)
        {
            akinator->answers[i].count++;
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

int getMostProbablyAnsw(Akinator* akinator)
{
    int id = 0;
    double maxProb = 0;
    for (size_t j = 0; j < akinator->answCount; ++j)
        if (akinator->answers[j].q_prob > maxProb) 
        {
            maxProb = akinator->answers[j].q_prob;
            id = j;
        }
    
    return id;
}

#define curId akinator->answers[id].personalQuestions[j].id

void askQuestion(Akinator* akinator, size_t id)
{
    int count = 3;
    for (size_t j = 0; j < akinator->answers[id].count && count; ++j)
    {
        int tmp = curId;
        if (akinator->questions[curId].isAsked == 1) {continue;}
        printf("%s", akinator->questions[curId].question);
        akinator->questions[curId].isAsked = 1;
    }
}

#undef curId

void doAkinator(Akinator* akinator)
{
    size_t id;
    int tmp = 10;
    while(GameEvent && tmp--)
    {
        id = getMostProbablyAnsw(akinator);
        askQuestion(akinator, id);
    }
}
