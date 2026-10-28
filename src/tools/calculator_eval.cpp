// src/tools/calculator_eval.cpp — YOU IMPLEMENT (spec §4.6).
//
// calc_evaluate: evaluate a well-formed token list with two stacks
// (one for numbers, one for operators and '('). See spec §4.6 and the
// worked trace in Appendix C. std::stack or std::vector are both fine.

#include "tools/calculator.h"

bool calc_evaluate(const std::vector<CalcToken>& tokens, double* out,
                   std::string* error) {
    *error = "calc_evaluate not implemented yet";   // TODO
    std::stack<double> num_stack;
    std::stack<CalcToken::Kind> op_stack;

    for (const auto& token : tokens){
        if(token.kind == CalcToken::Kind::RParen){
            std::stack<double> reverse_num_stack;
            std::stack<CalcToken::Kind> reverse_op_stack;

            reverse_num_stack.push(num_stack.top());
            num_stack.pop();
            
            // reverse_op_stack.push(CalcToken::Kind::LParen);

            while(op_stack.top() != CalcToken::Kind::LParen){
                reverse_op_stack.push(op_stack.top());
                op_stack.pop();
                reverse_num_stack.push(num_stack.top());
                num_stack.pop();
            }
            while(!reverse_op_stack.empty()){
                double first = reverse_num_stack.top();
                reverse_num_stack.pop();
                double second = reverse_num_stack.top();
                reverse_num_stack.pop();
                CalcToken::Kind op = reverse_op_stack.top();
                reverse_op_stack.pop();
                double output;
                if(!evaluate(first, op, second, output, error)){
                    return false;
                }
                reverse_num_stack.push(output);
            }
            if(!reverse_num_stack.empty()){
                num_stack.push(reverse_num_stack.top());
                reverse_num_stack.pop();
            }
            
            op_stack.pop();  //gets rid of L parentheses
            if(!op_stack.empty()){
                if((op_stack.top() == CalcToken::Kind::Times) || (op_stack.top() == CalcToken::Kind::Divide)){  //fixes precedneces
                    double second = num_stack.top();
                    num_stack.pop();
                    double first = num_stack.top();
                    num_stack.pop();
                    CalcToken::Kind op = op_stack.top();
                    op_stack.pop();
                    double output;
                    if(!evaluate(first, op, second, output, error)){
                        return false;
                    }
                    num_stack.push(output);
                }
            }
            
        }
        else if(token.kind != CalcToken::Kind::Number){
            op_stack.push(token.kind);
        }
        else if(token.kind == CalcToken::Kind::Number){
            if(!op_stack.empty()){
                if((op_stack.top() == CalcToken::Kind::Times) || (op_stack.top() == CalcToken::Kind::Divide)){
                    double first_num = num_stack.top();
                    CalcToken::Kind op = op_stack.top();
                    num_stack.pop();
                    op_stack.pop();
                    double output;
                    if(!evaluate(first_num, op, token.value, output, error)){  //handles divion by 0
                        return false;
                    }
                    num_stack.push(output);
                }
                else{
                    num_stack.push(token.value);
                }
            }
            else{
                num_stack.push(token.value);  // if the op stack is empty
            }
        }


        
    }
    std::stack<double> reverse_num_stack;
    std::stack<CalcToken::Kind> reverse_op_stack;

    reverse_num_stack.push(num_stack.top());
    num_stack.pop();
    while(!op_stack.empty()){
        reverse_num_stack.push(num_stack.top());
        num_stack.pop();
        reverse_op_stack.push(op_stack.top());
        op_stack.pop();
    }
    while(!reverse_op_stack.empty()){
        double first = reverse_num_stack.top();
        reverse_num_stack.pop();
        double second = reverse_num_stack.top();
        reverse_num_stack.pop();
        CalcToken::Kind op = reverse_op_stack.top();
        reverse_op_stack.pop();
        double output;
        if(!evaluate(first, op, second, output, error)){
            return false;
        }
        reverse_num_stack.push(output);
    }
    *out = reverse_num_stack.top();
    reverse_num_stack.pop();
    return true;
}

bool evaluate(double first, CalcToken::Kind operation, double second, double& out, std::string* insideError){
    switch(operation){
        case CalcToken::Kind::Divide:
            if(second != 0.0){
                out = first / second;
            }
            else{
                *insideError = "division by zero";
                return false;
            }
            break;
        case CalcToken::Kind::Times:
            out = first*second;
            break;
        case CalcToken::Kind::Minus:
            out = first - second;
            break;
        case CalcToken::Kind::Plus:
            out = first + second;
            break;
        default:
        break;
    }
    return true;
}