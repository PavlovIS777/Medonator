#ifndef AKINATOR_H
#define AKINATOR_h

#include <cstddef>

#define INPUT_ANSWERS_FILE_NAME "a.txt"
#define INPUT_QUESTIONS_FILE_NAME "q.txt"


struct Question
{
    size_t id;
    size_t yes_answ;
    size_t no_answ;
    size_t dm_answ;
};

struct questionDataBase
{
    char* question;
    size_t id;
    bool isAsked;
};

struct Answer
{
    char* name = nullptr;
    Question* personalQuestions;
    double q_prob;
    size_t count;
};

struct Akinator
{
    Answer* answers = nullptr;
    questionDataBase* questions = nullptr;
    size_t answCount = 0;
    size_t qCount = 0;
    double requiredDiff;
};

void initAnwsers(Akinator* akinator);

void parsAkinAnsws(Akinator* akinator, char* answStr);

void initQuestions(Akinator* akinator);

Akinator* initAkinator();

int getMostProbablyAnsw(Akinator* akinator);

void askQuestion(Akinator* akinator, size_t id);

void doAkinator(Akinator* akinator);
#endif