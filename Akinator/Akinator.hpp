#ifndef AKINATOR_H
#define AKINATOR_h

#include <cstddef>

#define INPUT_ANSWERS_FILE_NAME "a.txt"
#define INPUT_QUESTIONS_FILE_NAME "q.txt"

enum USER_INPUT
{
    NEGATIVE    = -1,
    DM_ANSWER   = 0,
    POSITIVE    = 1
};

struct Question
{
    size_t id;
    size_t yes_answ;
    size_t no_answ;
    size_t dm_answ;
};

/*
struct answ
{
    personId
    enum 
}
arr_que[]
*/



struct questionDataBase
{
    char* question;
    size_t* answId;
    size_t answCnt;
    size_t id;
    bool isAsked;
};

struct Answer
{
    char* name = nullptr;
    Question* personalQuestions;
    size_t queCount;
    double probability;
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

void parseAkinAnsws(Akinator* akinator, char* answStr);

void initQuestions(Akinator* akinator);

Akinator* initAkinator();

void sortAnswByProb(Akinator* akinator);

void askQuestion(Akinator* akinator);

int answComparator(const void* akinAnsw1, const void* akinAnsw2);

int getUserAnswer();

void doAkinator(Akinator* akinator);

void parseAnswIdforDB(Akinator* akinator, size_t qeuId);
#endif