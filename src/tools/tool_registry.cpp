// src/tools/tool_registry.cpp — YOU IMPLEMENT (spec §4.3).
//
// A hand-written hash table. No std::unordered_map / std::map / std::set.
// Every function below is a stub so the starter compiles. Replace each body.

#include "tools/tool_registry.h"

ToolRegistry::ToolRegistry() = default; // TODO: create your initial buckets (64 buckets)

   
ToolRegistry::~ToolRegistry() = default;   // unique_ptrs in your buckets free the tools

std::size_t ToolRegistry::hash(std::string_view toolname) noexcept {
    std::size_t h = 14695981039346656037ull;
    for (unsigned char c : toolname) { 
        h ^= c;
        h *= 1099511628211ull; }
    return h;
   // TODO: FNV-1a (spec Appendix A) or another real string hash
}

   // Takes ownership of `tool` and indexes it under tool->name().
    // Returns false (and destroys `tool`) if `tool` is null or the name is
    // already registered; the existing tool is left untouched.
    // Amortized O(1).
bool ToolRegistry::add(std::unique_ptr<Tool> tool) {
    if((load_factor() == 1.0) || (max_probe_length() == 64)){  //makes sure never to let the load factor get to high or the max length get too long
        rehash(2*bucket_count());
    }
    if (tool == nullptr) {
        return false;
    }

    std::string name_of_tool = tool->name();
    if (find(name_of_tool) != nullptr) {
        return false;
    }
    std::size_t key = hash(name_of_tool) % bucket_count_value;
    //std::unique_ptr<Tool> owned_tool = move(tool);
    // if (Tools[key].head != nullptr){
    //     bucket_count_value++;
    // }
    Tools[key].add(name_of_tool, std::move(tool)); 
    num_of_tools++; 

    return true;

}


    // The tool registered under `name`, or nullptr. Expected O(1).
    // The returned pointer stays valid for the registry's lifetime, even
    // across later add() calls that trigger a resize.
Tool* ToolRegistry::find(std::string_view findtoolname) const {
    std::size_t key = hash(findtoolname) % bucket_count_value;
    Node* nameptr = Tools[key].head;
    while(nameptr != nullptr){
        if(nameptr->toolname == findtoolname){
            return nameptr->AssociatedTool.get();
        }
        nameptr = nameptr->next;
    }
    return nullptr;  
}


std::size_t ToolRegistry::size() const noexcept {
    return num_of_tools;
}

void ToolRegistry::for_each(const std::function<void(const Tool&)>& fn) const {
    for(const auto& bucket : Tools) {
        Node* nameptr = bucket.head;
        while(nameptr != nullptr){
            fn(*(nameptr->AssociatedTool));
            nameptr = nameptr->next;
        }
    }
}

std::size_t ToolRegistry::bucket_count() const noexcept {
    return bucket_count_value;  
}

double ToolRegistry::load_factor() const noexcept {
    return static_cast<double>(num_of_tools) / static_cast<double>(bucket_count_value);  
}

std::size_t ToolRegistry::max_probe_length() const {
    size_t max_probe_length = 0;
    for (const auto& bucket : Tools) {
        if (bucket.probe_length > max_probe_length) {
            max_probe_length = bucket.probe_length;
        }
    }
    return max_probe_length;
}

void ToolRegistry::rehash(std::size_t new_count) {
    std::vector<LinkedList> fresh(new_count);
    for (auto& bucket : Tools) {
        Node* n = bucket.head;
        while (n != nullptr) {
            Node* next = n->next;                       // save before relinking
            std::size_t i = hash(n->toolname) % new_count;
            n->next = fresh[i].head;
            fresh[i].head = n;
            fresh[i].probe_length++;
            n = next;
        }
        bucket.head = nullptr;   
        bucket.probe_length = 0;    
    }
    Tools = std::move(fresh);
    bucket_count_value = new_count;
}

    // void NameLinkedList::add(std::string name, ToolNode* AssociatedToolNode){
    //     NameNode* newName = new NameNode(name, AssociatedToolNode);
    //     newName->next = head;
    //     head = newName;
    //     return;
    // }

    // NameNode::NameNode(std::string name, ToolNode* AssociatedToolNode, NameNode* nextNode){
    //     toolname = name;
    //     AssociatedTool = AssociatedToolNode;
    //     next = nextNode;
    // }

    // ToolNode* ToolLinkedList::add(std::unique_ptr<Tool> Tool_to_add){
    //     ToolNode* newNode = new ToolNode(std::move(Tool_to_add));
    //     newNode->next = head;
    //     head = newNode;
    //     return newNode;
    // }

    //     ToolNode::ToolNode(std::unique_ptr<Tool> ToolforConstruction, ToolNode* nextNode){
    //     Tool_in_Node = std::move(ToolforConstruction);
    //     next = nextNode;
    // }

    Node::Node(std::string name, std::unique_ptr<Tool> AssociatedToolNode, Node* nextNode){
        
        toolname = name;
        AssociatedTool = std::move(AssociatedToolNode);
        next = nextNode;
    }

    void LinkedList::add(std::string name, std::unique_ptr<Tool> AssociatedToolNode){
        Node* newNode = new Node(name, std::move(AssociatedToolNode));
        newNode->next = head;
        head = newNode;
        probe_length++;
    }

    LinkedList::~LinkedList() {
        Node* current = head;
        while (current != nullptr) {
            Node* nextNode = current->next;
            delete current;
            current = nextNode;
        }
    }

    ////////////////////////////////////////////////
    //used for testing

    bool ToolRegistry::worse_add(std::unique_ptr<Tool> tool) {
    if((load_factor() == 1.0) || (max_probe_length() == 64)){  //makes sure never to let the load factor get to high or the max length get too long
        rehash(2*bucket_count());
    }
    if (tool == nullptr) {
        return false;
    }

    std::string name_of_tool = tool->name();
    if (find(name_of_tool) != nullptr) {
        return false;
    }
    std::size_t key = worse_hash(name_of_tool) % bucket_count_value;
    //std::unique_ptr<Tool> owned_tool = move(tool);
    // if (Tools[key].head != nullptr){
    //     bucket_count_value++;
    // }
    Tools[key].add(name_of_tool, std::move(tool)); 
    num_of_tools++; 

    return true;

}

std::size_t ToolRegistry::worse_hash(std::string_view name){
    return name[0] + name.size();
}