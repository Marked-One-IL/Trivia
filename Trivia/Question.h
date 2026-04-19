#pragma once
#include <string>
#include <vector>
#include <cstdint>

class Question
{
public:
	Question(const std::string& question, const std::vector<std::string>& possibleAnswers, uint8_t correctAnswerIndex);
	std::string getQuestion(void) const;
	std::vector<std::string> getPossibleAnswers(void) const;
	std::string getCorrectAnswer(void) const;
	int getCorrectIndex() const;

	bool operator == (const Question& question) const;

private:
	std::string m_question;
	std::vector<std::string> m_possibleAnswers;
	uint8_t m_correctAnswerIndex;
};