#include "Question.h"

Question::Question(const std::string& question, const std::vector<std::string>& possibleAnswers, uint8_t correctAnswerIndex) : 
	m_question(question), m_possibleAnswers(possibleAnswers), m_correctAnswerIndex(correctAnswerIndex)
{
}

std::string Question::getQuestion(void) const
{
	return this->m_question;
}
std::vector<std::string> Question::getPossibleAnswers(void) const
{
	return this->m_possibleAnswers;
}
std::string Question::getCorrectAnswer(void) const
{
	return this->m_possibleAnswers[this->m_correctAnswerIndex];
}

int Question::getCorrectIndex() const
{
	return (int)m_correctAnswerIndex;
}

bool Question::operator==(const Question& question) const
{
	return this->m_question == question.m_question;
}
