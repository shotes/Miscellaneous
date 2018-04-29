#include<string>
#include<cstring>
#include<iostream>
#include<unistd.h>
#include<sys/wait.h>
#include<vector>
#include<array>
#include<cctype>
#include "Tokenizer.cpp"
#include<algorithm>
#include<stdio.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<cmath>
#include<numeric>

using namespace std;

//Converts vector<string> to vector<char*>
//This is copied from https://stackoverflow.com/questions/42493101/how-to-convert-a-vectorstring-to-a-vectorchar
vector<char*> convertVector(vector<string> input){
    vector<char*> charVec;
    for(const auto &str : input){
        char *charStr = new char[str.size()+1];
        strcpy(charStr, str.c_str());
        charVec.push_back(charStr);
    }
    //charVec.push_back(NULL);
	return charVec;
}

//Finds the index of every single pipe in the input vector
vector<int> findPipes(vector<string> input){
    vector<int> pipeIndex;
    //loop through the vector and push_back every pipe index
    for(int x = 0; x < input.size(); x++){
        if(input[x] == "|"){
            pipeIndex.push_back(x);
        }
    }
    return pipeIndex;
}

vector<int> findRedirects(vector<string> inputs){
    vector<int> redirects = {-2,-2};
    for(int x = 1; x < inputs.size()-1;x++){
        if(inputs[x] == "<")
            redirects[0] = x;
        if(inputs[x] == ">")
            redirects[1] = x;
    }
    return redirects;
}

//return the subVector of a char* vector. Begin is inclusive. End is not inclusive.
vector<char*> subvec(vector<char*> input, int begin, int end){
    if(input.size() < end)
        end = input.size();
    vector<char*> subVector;
    for(int x = begin; x < end; x++){
        subVector.push_back(input[x]);
    }
    return subVector;
}

//execute instructions
void myExecute(string input){
    Tokenizer *tokenizer = new Tokenizer(input);
    vector<string> inputCommands = tokenizer->getTokens();
    vector<char*> cinputs = convertVector(inputCommands);
    vector<int> validated = findRedirects(inputCommands);
    int pid;
    if(validated[0] > 0){
        vector<char*> inputCommand = subvec(cinputs,0,validated[0]);
        const char* inputFile = inputCommands[validated[0]+1].c_str();
        int fd = open(inputFile,O_RDONLY,S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        dup2(fd,0);
        close(fd);
        execvp(inputCommand[0],&inputCommand[0]);
    }
    if(validated[1] > 0){
        vector<char*> outputCommand = subvec(cinputs,0,validated[1]);
        const char* outputFile = inputCommands[validated[1]+1].c_str();
        int fd = open(outputFile,O_CREAT|O_WRONLY|O_TRUNC,S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        dup2(fd,1);
        close(fd);
        execvp(outputCommand[0],&outputCommand[0]);
    }
    else{
        execvp(cinputs[0],&cinputs[0]);
    }
}

//return vector of strings that are split at all pipeIndexes
vector<string> splitVec(vector<string> input, vector<int> pipeIndex){
    vector<string> splitInput;
    string command;
    int x = 0;
    int i = 0;
    while(x < pipeIndex.size()){
        command = "";
        while(i < pipeIndex[x]-1){
            command += input[i] + " ";
            i++;
        }
        command += input[i];
        i += 2;
        splitInput.push_back(command);
        x++;
    }
    command = "";
    while(i < input.size()-1){
        command += input[i] + " ";
        i++;
    }
    command += input[i];
    i++;
    splitInput.push_back(command);
    return splitInput;
}

bool isBackgroundProcess(vector<string> inputs){
    return (inputs[inputs.size()-1] == "&");
}

void executeInOrder(vector<string> inputs,vector<int> pipeIndex){
    bool background = isBackgroundProcess(inputs);
    if(background)
        inputs.pop_back();
    vector<string> parts = splitVec(inputs,pipeIndex);
    int fds[2*parts.size()];
    for(int x = 0; x < parts.size(); x++){
        pipe(&fds[2*x]);
    }
    for(int x = 0; x < parts.size();x++){
        pid_t pid = fork();
        if(!pid){
            if(x != 0){
                dup2(fds[2*x],0);
                close(fds[2*x]);
                close(fds[2*x+1]);
            }
            if(x != parts.size()-1){
                dup2(fds[2*x+3],1);
                close(fds[2*x+3]);
                close(fds[2*x+2]);
            }
            myExecute(parts[x]);
        }
        else{
            close(fds[2*x]);
            close(fds[2*x+3]);
            //don't wait for background processes
            if(background){
                if(!fork()){
                    waitpid(pid,0,0);
                }
            }
            else{
                wait(0);
            }
        }
    }
}

int main(){
    string inputLine;
    Tokenizer *tokenizer;
    vector<string> inputs;
    vector<int> pipeIndex, validated;
    while(true){
        //print prompt
        cerr << "PA2_Shell>>";
        getline(cin,inputLine);
        //start parse
        tokenizer = new Tokenizer(inputLine);
        inputs = tokenizer->getTokens();
        //exit condition
        if(inputLine.empty()){
            cerr << "Empty Line! Exiting..." << endl;
            return 0;
        }
        //find pipes to split
        pipeIndex = findPipes(inputs);
        //Validate the redirects
        for(int x = 0; x < inputs.size(); x++){
            //If the index of '<' is after the index of the first pipe then it's bad
            if(inputs[x] == "<" && pipeIndex.size() > 0 && x >= pipeIndex[0]){
                    cerr << "Invalid < redirect! Exiting..." << endl;
                    return 0;
            }
            //If the index of '>' is before the index of the last pipe then it's bad
            if(inputs[x] == ">" && pipeIndex.size() > 0 && x <= pipeIndex[pipeIndex.size()-1]){
                    cerr << "Invalid > redirect! Exiting..." << endl;
                    return 0;
            }
        }
        if(inputs[0] == "cd"){
            if(inputs[1] == "-"){
                string back = "..";
                chdir(back.c_str());
                vector<string> pwd = {"pwd"};
                vector<int> zero;
                executeInOrder(pwd,zero);
            }
            else{
                vector<char*> cinputs = convertVector(inputs);
                chdir(cinputs[1]);
            }
        }
        if(inputs[0] == "dd"){

        }
        if(inputs[0] == "exit" || inputs[0] == "quit"){
            cerr << "Exiting the program..." << endl;
            return 0;
        }
        //pipe setup and execution
        executeInOrder(inputs,pipeIndex);
    }
}
