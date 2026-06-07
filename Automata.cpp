#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <set>
#include <map>
#include <utility>
#include <algorithm>
#include <stdexcept>
#include <vector>
#include <sstream>
#include <stack>
#include <tuple>

namespace Parser
{
    std::vector<std::string> getTokens(const std::string& str)
    {
        std::vector<std::string> tokens;
        std::stringstream ss(str);
        std::string token;

        while (std::getline(ss, token, ',')) {
        size_t start = token.find_first_not_of(" \t\r\n");
        size_t end = token.find_last_not_of(" \t\r\n");

        if (start != std::string::npos && end != std::string::npos) {
            tokens.push_back(token.substr(start, end - start + 1));
        }
    }
    return tokens;
    }



}


class Automata
{

    std::string type{};

    public:


    virtual ~Automata()=default;

    virtual void Parse(std::ifstream& file)=0;
    virtual bool Run(const std::string& input) const=0;
    virtual bool Validate() const=0;


       std::string getType() const
    {
        return type;
    }
    void setType(const std::string& arg)
    {
        type=arg;
    }
};


class DFA: public Automata
{

    int startState{};
    std::set<char> alphabet{};
    std::set<int> states{};
    std::set<int> acceptStates{};
    std::map<std::pair<int,char>,int> transitionTable;


    public:


    void Parse(std::ifstream& file) override
    {
        std::string line{};
        bool parsingTransition{false};

        while(std::getline(file, line))
        {   

            if(parsingTransition)
            {
                std::vector<std::string> parts = Parser::getTokens(line);

                if(parts.size()==3)
                {
                    int fromState = std::stoi(parts[0]);
                    char symbol = parts[1][0];
                    int toState = std::stoi(parts[2]);\

                    transitionTable[{fromState,symbol}]=toState;
                }

                continue;;
            }

            size_t valuesStart=line.find(':');

            if(valuesStart==std::string::npos)
                continue;

            std::string key = line.substr(0, valuesStart);
            std::string value= line.substr(valuesStart+1);

            std::vector<std::string> tokens=Parser::getTokens(value);

            if(key=="STATES")
            {
                for(const auto& token: tokens)
                    states.insert(std::stoi(token));
            }
            else
            if(key=="ALPHABET")
            {
                for(const auto& token : tokens)
                {
                    if(!token.empty())
                        alphabet.insert(token[0]);
                }
            }
            else if(key=="START")
            {
                startState=std::stoi(tokens[0]);
            }
            else if(key=="ACCEPT")
            {
                 for(const auto& token: tokens)
                    acceptStates.insert(std::stoi(token));
            }
            else if(key=="TRANSITIONS")
            {
                parsingTransition=true;
            }
            else
            {
                throw std::runtime_error("???\n");
            }
        }
    }

    bool Validate() const override
    {
        if(!states.count(startState))
            {
                return false;
            }
        
        for(const auto& state:acceptStates)
            {
                if(!states.count(state))
                    return false;
            }

        for(const auto& state:states)
        {
            for(const auto& symbol: alphabet)
                {
                    const auto& transition =transitionTable.find({state,symbol});

                    if(transition==transitionTable.end())
                        return false;
                    
                    if(!states.count(transition->second))
                        return false;
                }
        }

        


        return true;
    }


    bool Run(const std::string& input) const override
    {
        int currentState=startState;
        for(const char symbol: input)
        {
            if(!alphabet.count(symbol))
            {
                std::cerr<<"Invalid string!\n";
                return false;
            }
            currentState=transitionTable.at({currentState,symbol});
        }

        if(acceptStates.count(currentState))
            return true;
        else
            return false;
    }

};

class NFA: public Automata
{

    int startState{};
    std::set<char> alphabet{};
    std::set<int> states{};
    std::set<int> acceptStates{};

    std::map<std::pair<int, char>, std::set<int>> transitionTable;

    std::set<int> computeEpsilon(const std::set<int>& activeStates) const
    {
        std::set<int> closure = activeStates;

        std::stack<int> stack{};

        for(int state:activeStates)
            stack.push(state);
        
        while(!stack.empty())
        {
            int currentState=stack.top();
            stack.pop();

            auto epsilon =transitionTable.find({currentState,'~'});
            if(epsilon!=transitionTable.end())
            {
                for(int nextState:epsilon->second)
                {
                    if(closure.find(nextState)==closure.end())
                    {
                        closure.insert(nextState);
                        stack.push(nextState);
                    }
                }
            }
        }


        return closure;
        

    }







    public:

    void Parse(std::ifstream& file) override {
       std::string line{};
        bool parsingTransition{false};

        while(std::getline(file, line))
        {   

            if(parsingTransition)
            {
                std::vector<std::string> parts = Parser::getTokens(line);

                if(parts.size()==3)
                {
                    int fromState = std::stoi(parts[0]);
                    char symbol = parts[1][0];
                    int toState = std::stoi(parts[2]);\

                    transitionTable[{fromState,symbol}].insert(toState);
                }

                continue;;
            }

            size_t valuesStart=line.find(':');

            if(valuesStart==std::string::npos)
                continue;

            std::string key = line.substr(0, valuesStart);
            std::string value= line.substr(valuesStart+1);

            std::vector<std::string> tokens=Parser::getTokens(value);

            if(key=="STATES")
            {
                for(const auto& token: tokens)
                    states.insert(std::stoi(token));
            }
            else
            if(key=="ALPHABET")
            {
                for(const auto& token : tokens)
                {
                    if(!token.empty())
                        alphabet.insert(token[0]);
                }
            }
            else if(key=="START")
            {
                startState=std::stoi(tokens[0]);
            }
            else if(key=="ACCEPT")
            {
                 for(const auto& token: tokens)
                    acceptStates.insert(std::stoi(token));
            }
            else if(key=="TRANSITIONS")
            {
                parsingTransition=true;
            }
            else
            {
                throw std::runtime_error("???\n");
            }
        }
    }



    
    bool Validate() const override {
         if(!states.count(startState))
            {
                return false;
            }
        
        for(const auto& state:acceptStates)
            {
                if(!states.count(state))
                    return false;
            }

      for(const auto& transition: transitionTable)
      {
        int fromState=transition.first.first;
        const std::set<int>& destinationStates=transition.second;


        if(!states.count(fromState))
            return false;
        
        for(const int toState: destinationStates)
            if(!states.count(toState))
                return false;

      }

        return true;

    }

    bool Run(const std::string& input) const override {
        
        std::set<int> currentStates{startState};

        currentStates=computeEpsilon(currentStates);

        for(const char symbol: input)
        {
            std::set<int> nextStates;

            for(int state:currentStates)
            {
                auto it =transitionTable.find({state,symbol});
                if(it!= transitionTable.end())
                {
                    nextStates.insert(it->second.begin(),it->second.end());
                }
            }
              currentStates=computeEpsilon(nextStates);

              if(currentStates.empty())
              return false;

        }

        for(int state:currentStates)
        {
            if(acceptStates.count(state))
                return true;
        }


        return false;

      


    }

};


struct ResultPDA
{
    int nextState;
    std::string symbolToPush;
    
};

class PDA: public Automata
{
    protected:

    int startState{};
    std::set<char> alphabet{};
    std::set<int> states{};
    std::set<int> acceptStates{};

    char startStackSymbol{};
    
    std::map<std::tuple<int,char,char>, std::vector<ResultPDA>> transitionTable;




    public:
    void Parse(std::ifstream& file) override {
          std::string line{};
        bool parsingTransition{false};

        while(std::getline(file, line))
        {   

            if(parsingTransition)
            {
                std::vector<std::string> parts = Parser::getTokens(line);

                if(parts.size()==5)
                {
                    int fromState = std::stoi(parts[0]);
                    char inputSymbol = parts[1][0];
                    char popSymbol=parts[2][0];
                    int toState = std::stoi(parts[3]);
                    std::string pushSymbols =parts[4];

                    if(pushSymbols=="~")
                        pushSymbols="";

                    transitionTable[{fromState,inputSymbol,popSymbol}].push_back({toState,pushSymbols});
                }



                continue;;
            }

            size_t valuesStart=line.find(':');

            if(valuesStart==std::string::npos)
                continue;

            std::string key = line.substr(0, valuesStart);
            std::string value= line.substr(valuesStart+1);

            std::vector<std::string> tokens=Parser::getTokens(value);

            if(key=="STATES")
            {
                for(const auto& token: tokens)
                    states.insert(std::stoi(token));
            }
            else
            if(key=="ALPHABET")
            {
                for(const auto& token : tokens)
                {
                    if(!token.empty())
                        alphabet.insert(token[0]);
                }
            }
            else if(key=="START")
            {
                startState=std::stoi(tokens[0]);
            }
            else if(key=="ACCEPT")
            {
                 for(const auto& token: tokens)
                    acceptStates.insert(std::stoi(token));
            }
            else if(key=="TRANSITIONS")
            {
                parsingTransition=true;
            }
            else if(key=="STACK_START")
            {
                startStackSymbol=tokens[0][0];
            }
            else
            {
                throw std::runtime_error("???\n");
            }
        }
    }
    
    bool Validate() const override
     {
              if(!states.count(startState))
            {
                return false;
            }
        
        for(const auto& state:acceptStates)
            {
                if(!states.count(state))
                    return false;
            }
        for(const auto& transition:transitionTable)
        {
            int fromState=std::get<0>(transition.first);
            const std::vector<ResultPDA>& destinations=transition.second;

            if(!states.count(fromState))
                return false;

            for(const auto& result :destinations)
            {
                if(!states.count(result.nextState))
                return false;
            }
        }

        return true;
    }
    bool explore(int state, size_t inputIndex, const std::string& input,std::stack<char> stack) const
    {

        char top=stack.empty() ? '~':stack.top();

        if(inputIndex==input.length())
        {
            if(acceptStates.count(state))
            return true;
        }

        char symbol=(inputIndex<input.length()) ? input[inputIndex] : '~';


        std::vector<char> possibleInputs={symbol,'~'};
        std::vector<char> possiblePops={top,'~'};


        for(char in:possibleInputs)
        {
            for(char pop:possiblePops)
            {
                const auto& it=transitionTable.find({state,in,pop});
                
                if(it !=transitionTable.end())
                {
                    for(const auto& result :it->second)
                    {
                        std::stack<char> nextStack=stack;
                        if(pop!='~'&& !nextStack.empty())
                            nextStack.pop();

                            for(auto rit=result.symbolToPush.rbegin();rit!=result.symbolToPush.rend();++rit)
                                nextStack.push(*rit);

                            if(explore(result.nextState,(in=='~'?inputIndex:inputIndex+1),input,nextStack))
                            return true;
                    }
                }
            }
        }

        return false;
    }

    bool Run(const std::string& input) const override {
        std::stack<char> initialStack;
        initialStack.push(startStackSymbol);

        return explore(startState,0,input,initialStack);
    }


};


class Game: public PDA
{
    int activeState{};
    std::map<int, std::string> gameRooms{};
    std::stack<char> inventory{};

    int Step(char inputCommand)
    {
        char top=inventory.empty() ? '~':inventory.top();

        auto it=transitionTable.find({activeState,inputCommand,top});

        if(it==transitionTable.end())
        {
            it=transitionTable.find({activeState,inputCommand,'~'});
        }

        if(it!=transitionTable.end())
        {
            const auto& result=it->second[0];

            char popChar=std::get<2>(it->first);
            if(popChar!='~'&&!inventory.empty())
                inventory.pop();
        

        for (auto rit = result.symbolToPush.rbegin(); rit != result.symbolToPush.rend(); ++rit) {
                inventory.push(*rit);
            }

            activeState = result.nextState;
            return activeState;
        }
        return -1;
    }

    public:

    Game()
    {
     gameRooms={
    {0, "START"},
    {1, "Library"},
    {2, "Workshop"},
    {3, "Lab"},
    {4, "Garden"},
    {5, "Gate"},
    {6, "Heaven"},
    {7, "Hell"}
};

    }


    bool Run(const std::string& input) const override
    {
        Game* mutableThis=const_cast<Game*>(this);
        mutableThis->activeState=startState;

        while(!mutableThis->inventory.empty())
            mutableThis->inventory.pop();

        mutableThis->inventory.push(startStackSymbol);


        std::cout<<"Game Start!\n";

        while(true)
        {
            std::cout << "\n-----------------------------------\n";
            std::cout << "You are in: " << gameRooms.at(mutableThis->activeState) << "\n";
        

        if (acceptStates.count(mutableThis->activeState)) {
                std::cout << "Game Over.\n";
                return true; 
            }
        
            std::string command;
            std::cout << "Coordinate (N, S, E, W, or Exit): ";
            std::cin >> command;

            if (command == "Exit") {
                std::cout << "Exiting game...\n";
                return false;
            }

        int nextRoom = mutableThis->Step(command[0]);

            if (nextRoom == -1) {
                std::cout << "You can't go that way!\n";

            } else  if (mutableThis->activeState == 5) 
                    mutableThis->Step('~'); 
              
        
    
    }
    return false;
}

};


class AutomataFactory
{
      
    public:

    

    static std::shared_ptr<Automata> loadAutomata(const std::string& path)
    {

        std::ifstream file(path);
        if(!file.is_open())
        {
            std::cerr<<"Error opening the file!\n";
            return nullptr;
        }
        std::string type{};
        std::getline(file,type);

        std::transform(type.begin(),type.end(),type.begin(),::toupper);

        std::shared_ptr<Automata> automat{nullptr};

        if(type=="DFA")
        {
            automat=std::make_shared<DFA>();
            automat->Parse(file);
            automat->setType(type);
            if(automat->Validate())
                return automat;
            else
            {
                std::cerr<<"Invalid automata!\n";
                return nullptr;
            }
        }
        else if(type=="NFA")
        {
            automat=std::make_shared<NFA>();
            automat->Parse(file);
             automat->setType(type);
            if(automat->Validate())
                return automat;
            else
            {
                std::cerr<<"Invalid automata!\n";
                return nullptr;
            }
        }
        else if(type=="PDA")
        {
            automat=std::make_shared<PDA>();
            automat->Parse(file);
             automat->setType(type);
            if(automat->Validate())
                return automat;
            else
            {
                std::cerr<<"Invalid automata!\n";
                return nullptr;
            }
        }
        else if (type == "GAME")
        {
            automat = std::make_shared<Game>();
            automat->Parse(file);
             automat->setType(type);
            if (automat->Validate())
                return automat;
            else
            {
                std::cerr << "Invalid game!\n";
                return nullptr;
            }
        }
        else
        {
            std::cerr<<"Invalid type\n";
            return nullptr;
        }


    }



};



int main(int argc, char* argv[])
{

    if(argc<2)
    {
        std::cerr<<"Not enough arguments!\n";
        return 1;
    }

    std::string programName=argv[0];
    std::string filePath=argv[1];



    std::shared_ptr<Automata> machine=AutomataFactory::loadAutomata(filePath);

    if(machine==nullptr)
    {
        std::cerr<<"Failed to load!\n";
        return 1;
    }
    
    while(true)
    {
        std::string input;
        if(machine->getType()!="GAME")
        {
        std::cout<<"Enter your input string or @ to exit!\n";
        std::cin>>input;
        
        if(input=="@")
            {
                std::cout<<"Closing!\n";
                break;
            }
        
        if(machine->Run(input))
            std::cout<<"String accepted!\n";
        else
            std::cout<<"String not accepted!\n";
        }
        else
        {   
            std::string name{};
            std::cout<<"Enter your name: \n";
            std::cin>>name;

            machine->Run(name);

            break;
        }

    }





    return 0;
}