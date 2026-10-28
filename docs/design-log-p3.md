# Project 3 Design Log
**Author:** <William Banfield> · **P2 code used:** <instructor reference>
**Tools used:** <claude used for debugging of code that I wrote myself and explaining coding concepts>

<!-- 500–800 words. An argument, not a diary: say what you decided, what
     you rejected, and what evidence backs it up. Delete these comments
     before submitting. -->

## 1. Hash table

     I chose to use chaining because it seemed far less complex for keeping track of the longest probe required and for rehashing when the number of buckets needed to be increased. For my hash function, I implemented the FNV-1a hash, which takes every character in the tool name into account when hashing. This is a more secure hash than the one presented in the example. It also makes the distribution of hash keys more uniform instead of putting similar tool names (tool_12 and tool_21) into the same bucket. My load-factor threshold was 1.0. This way, the resizing was only done when required. If there were fewer allocated tools than there were buckets, I chose to not resize. The resize policy is as follows: whenever the load factor is greater than or equal to 1.0, or whenever the max probe length is greater than or equal to 64, double the number of buckets, and rehash all of the allocated tools. By doing so, whenever the hash map is overwhelmed, it will be resized. In both of the cases mentioned, the equality should always be the reason that the hash map is resized, but the inequality is included for safety in case for some reason the double value is not precisely equal to 1.0. When adding 10,000 tools, the program ended with:
     Bucket Count equal to 16384
     Load Factor equal to 0.610352
     Maximum Probe Length equal to 6

     The alrternative hash's longest chain was 40.

     With this scheme, add() is amortized O(1) because although sometimes it will require the O(n) rehashing operation, when it does not, the add is O(1). We can see that the amount of times that the rehashing O(n) happens is low, because in the case of 10,000 tools above, a final bucket count of 16384 is equivalent to doubling the starting bucket count of 64 exactly 8 times. That is, in 10,000 add() executions, only 8 were O(n), while the other 9,992 were O(1). Thus the add() is amortized O(1).

## 2. Pointer stability

When a resize happens, the bucket_count_value is doubled. In my find() function, the key is the hash value % bucket_count_value. This means when a resize happens, the original node is mapped to a different linked list (bucket). However, find() can calculate the new linked list using the toolname and the new bucket_count_value. This means it knows where to search. On top of this, because unique_ptr is used, we are moving the actual smart pointer to the tool. This means that the find function after a resize is finding the same unique pointer. This is why it returned value of the find() is the same before and after a resize.


## 3. Calculator trace

     Before I begin, I will admit that my implementaiton of the stacks is janky. But to trace the values of the two stacks for 2 * (3 + 4) - 5, start with the first token, 2. 2 is of Kind::Number, and since the ops stack is empty, push 2 on the num stack.
     Updated stacks (top -> bottom): num: 2, ops: empty
     Then, the next token, * is of Kind::Times, so it gets pushed on the ops stack.
     Updated stacks (top -> bottom): num: 2, ops: *
     Third, the LParen token also gets pushed on the ops stack. This will be the signal for the end of operations when the RParen is parsed.
     Updated stacks (top -> bottom): num: 2, ops: LParen, *
     Next, the 3 is parsed. Since the top of the ops stack is not * or /, just add it to the stack as before.
     Updated stacks (top -> bottom): num: 3, 2, ops: LParen, *
     Next, the + is pushed to the ops stack.
     Updated stacks (top -> bottom): num: 3, 2, ops: +, LParen, *
     The next token is 4, and since the top of the ops stack is not * or /, push it on the numbers.
     Updated stacks (top -> bottom): num: 4, 3, 2, ops: +, LParen, *
     Now the next token is RParen, so we will pop everything from our stacks into reverse_stacks until the LParen is reached.
     Updated stacks (top -> bottom): num: 2, ops: *
                                     reverse_num: 3, 4 reverse_ops: +, LParen
     Then the RParen case evaluates the operation from left to right (hence the use of reverse stacks) (3 + 4) = 7, then checks what is on top of the ops stack. At this point:
          Updated stacks (top -> bottom): num: 2,  ops: *
          so it goes ahead and evaluates 2 * 7, and pops * and pops 2 and pushes 14 onto the num stack.
          Updated stacks (top -> bottom): num: 14,  ops: empty
     Next token is -, which is pushed on ops stack.
     Updated stacks (top -> bottom): num: 14,  ops: -
     Final token is 5, pushed on nums.
     Updated stacks (top -> bottom): num: 5, 14,  ops: -
     Then reverse stacks are made once again and the calcualation is done.
     Updated stacks (top -> bottom): reverse_num: 14, 5  reverse_ops: -
     14-5 = 9. The calculation is done.                                  

## 4. Errors as values

     They return errors instead of throwing because the thing that they are checking is the validity of the user input. If a user asks "What is 1 / (2 - 2)?" it should not throw an error for the harness or chat features. The model should know that the expression is invalid through using the calculator tool. If the calculator tool THREW an error, everything would halt, and no response would be given to the user. Instead, since the tool returns an error, the output text to the user can say something like "The result is undefined: division by 0" and take the next user input. The resolve() funciton is a similar story. It is just verifying that a user input for a filepath is valid within the workspace. If it is not, it should not halt everything, but it should notify the user that their input is not allowed. This is why both functions have bool as the return type. Returning true means that the input is valid and that the tool can be used, while returning false means that there is a reason the tool can't be used, and the error message can be echoed to the user.

     An exception is the right choice when the thing wrong is not just the invalidity of user input, but something that will break the functionality of the harness or char itself.

## 5. What I would design differently

If I were to redo this project, I would absolutely design my calculator stacks differently. They are so janky and I should have just used some sort of precedence marking of 2 or 1 for the different operations. Instead, I ended up making a very confusing time for myself and a less elegant solution. There are probably more things that I could do differently, but this my most egregiously bad design choice.
