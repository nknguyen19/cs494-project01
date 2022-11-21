#include "question.h"
#include <iostream>
#include <cctype>

Question::Question(string question, vector<string>& choices, char answer)
{
    this->question = question;
    this->choices = choices;
    this->answer = answer;
}

Question::~Question()
{
}

bool Question::isCorrect(char answer)
{
    return toupper(this->answer) == answer;
}

void Question::print()
{
    cout << this->question << endl;
    for (int i = 0; i < this->choices.size(); i++)
    {
        cout << (char)(i + 'A') << ". " << this->choices[i] << endl;
    }
}

string Question::getQuestion()
{      
    string question = this->question + "\n";
    for (int i = 0; i < this->choices.size(); i++)
    {
        question += this->choices[i] + "\n";
    }
    return question;
}
