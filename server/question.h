#ifndef QUESTION_H
#define QUESTION_H

#include <string>

using namespace std;

class Question
{
private:
    int id;
    string question;
    string answer;

public:
    Question();
    Question(int id, string question, string answer);
    ~Question();

    bool isCorrect(string answer);
};

#endif