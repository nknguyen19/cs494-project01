#ifndef QUESTION_H
#define QUESTION_H

#include <string>
#include <vector>

using namespace std;

class Question
{
private:
    string question;
    vector<string> choices;
    char answer;

public:
    Question(string question, vector<string>& choices, char answer);
    ~Question();

    bool isCorrect(char answer);
    string getQuestion();
    void print();
};

#endif