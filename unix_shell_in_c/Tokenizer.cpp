#include<vector>
#include<string>
#include<iostream>
#include<regex>

using namespace std;

class Tokenizer{
	private:
		vector<string> tokens;
	public:
		int currentIndex;
		Tokenizer();
		Tokenizer(string);
		Tokenizer& operator=(const Tokenizer&);
		vector<string> getTokens();
		int getSize();
};

Tokenizer::Tokenizer(){
}

Tokenizer::Tokenizer(string input){
	int space_index = input.find(" "),quote_index = input.find("\""),squote_index = input.find("\'");
	while((squote_index != string::npos) || (quote_index != string::npos) || (space_index != string::npos)){
		if(quote_index == 0){
			input = input.substr(1);
			tokens.push_back(input.substr(0,input.find('\"')));
			input = input.substr(input.find("\"")+1);
			if(space_index = input.find(" ") == 0)
				input = input.substr(1);
		}
        else if(squote_index == 0){
            input = input.substr(1);
			tokens.push_back(input.substr(0,input.find('\'')) + "\'");
			input = input.substr(input.find("\'")+1);
			if(space_index = input.find(" ") == 0)
				input = input.substr(1);
        }
		else if(space_index != string::npos){
			tokens.push_back(input.substr(0,space_index));
			input = input.substr(space_index + 1);
		}
		space_index = input.find(" ");
		quote_index = input.find("\"");
        squote_index = input.find("\'");
	}

	if(input.length() > 0){
		tokens.push_back(input);
	}
}

Tokenizer& Tokenizer::operator=(const Tokenizer& tokenizer){
	tokens = tokenizer.tokens;
	return *this;
}

vector<string> Tokenizer::getTokens(){
	return tokens;
}
