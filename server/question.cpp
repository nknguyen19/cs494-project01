#include "question.h"

Question::Question()
{
    this->id = 0;
    this->question = "";
    this->answer = "";
}

Question::Question(int id, string question, string answer)
{
    this->id = id;
    this->question = question;
    this->answer = answer;
}

Question::~Question()
{
}

bool Question::isCorrect(string answer)
{
    return this->answer == answer;
}