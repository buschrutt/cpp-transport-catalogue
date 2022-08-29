#include "json_builder.h"

namespace json {

    void KeyLogic(Builder& builder){
        if (!builder.GetNodeStack().empty()){
            if (!builder.GetNodeStack().back()->IsDict() || builder.GetIsKey()){
                throw std::logic_error ("logic_error: Key called out of Dict or right after key entered");
            }
        } else {
            throw std::logic_error ("logic_error: Key called out of Dict or right after key entered");
        }
    }

    // Begin Of %%%%%%%%%% %%%%%%%%%% %%%%%%%%%% Key %%%%%%%%%% %%%%%%%%%% %%%%%%%%%%

    Builder& Builder::BuildKey(const std::string &node_key) {
        KeyLogic(*this);
        is_previous_key_ = true;
        key_ = node_key;
        Node * pair_node = new Node();
        auto& dict = std::get<Dict>(node_stack_.back()->SetValue());
        dict.insert({node_key, *pair_node});
        return *this;
    }

    KeyContext& ValueFullContext::Key(const std::string &node_key) {
        KeyLogic(*builder_);
        builder_->SetIsKey(true);
        builder_->SetKey(node_key);
        //Node * pair_node = new Node();
        //auto& dict = std::get<Dict>(builder_->node_stack_.back()->SetValue());
        auto * key_context = new KeyContext(builder_);
        return *key_context;
    }

    KeyContext& ValueKeyContext::Key(const std::string &node_key) {
        KeyLogic(*builder_);
        builder_->SetIsKey(true);
        builder_->SetKey(node_key);
        Node * pair_node = new Node();
        auto& dict = std::get<Dict>(builder_->GetNodeStack().back()->SetValue());
        dict.insert({node_key, *pair_node});
        auto * key_context = new KeyContext(builder_);
        return *key_context;
    }

    KeyContext& DictContext::Key(const std::string &node_key) {
        KeyLogic(*builder_);
        builder_->SetIsKey(true);
        builder_->SetKey(node_key);
        Node * pair_node = new Node();
        auto& dict = std::get<Dict>(builder_->GetNodeStack().back()->SetValue());
        dict.insert({node_key, *pair_node});
        auto * key_context = new KeyContext(builder_);
        return *key_context;
    }

    // End Of %%%%%%%%%% %%%%%%%%%% %%%%%%%%%% Key %%%%%%%%%% %%%%%%%%%% %%%%%%%%%%

    void ValueLogic(Builder& builder){
        if (!builder.GetNodeStack().empty()){
            if ((!builder.GetNodeStack().back()->IsArray()) && !builder.GetIsKey()){
                throw std::logic_error ("logic_error: Value called(1) not after constructor, not within Array or after key entered");
            }
        } else {
            if (builder.GetRootPtr() != nullptr){
                throw std::logic_error ("logic_error: Value called(2) not after constructor, not within Array or after key entered");
            }
        }
    }

// Begin Of %%%%%%%%%% %%%%%%%%%% %%%%%%%%%% Value %%%%%%%%%% %%%%%%%%%% %%%%%%%%%%
    Builder &Builder::BuildValue(const Node::Value& node_value) {
        ValueLogic(*this);
        is_previous_key_ = false;
        Node * node = new Node();
        node->SetValue() = node_value;
        if (node_stack_.empty()){
            root_ptr_ = node;
            root_ = *node;
        } else {
            if (node_stack_.back()->IsArray()){
                auto& array = std::get<Array>(node_stack_.back()->SetValue());
                array.emplace_back(*node);
            } else if (node_stack_.back()->IsDict()){
                auto& dict = std::get<Dict>(node_stack_.back()->SetValue());
                dict[key_] = *node;
            }
        }
        return *this;
    }

    ValueKeyContext &KeyContext::Value(const Node::Value &node_value) {
        ValueLogic(*builder_);
        builder_->SetIsKey(false);
        Node * node = new Node();
        node->SetValue() = node_value;
        if (builder_->GetNodeStack().empty()){
            builder_->SetRootPtr(node);
            builder_->SetRoot(*node);
        } else {
            if (builder_->GetNodeStack().back()->IsArray()){
                auto& array = std::get<Array>(builder_->GetNodeStack().back()->SetValue());
                array.emplace_back(*node);
            } else if (builder_->GetNodeStack().back()->IsDict()){
                auto& dict = std::get<Dict>(builder_->GetNodeStack().back()->SetValue());
                dict[builder_->GetKey()] = *node;
            }
        }
        auto * value_context = new ValueKeyContext(builder_);
        return *value_context;
    }

    ValueArrayContext &ValueArrayContext::Value(const Node::Value &node_value) {
        ValueLogic(*builder_);
        builder_->SetIsKey(false);
        Node * node = new Node();
        node->SetValue() = node_value;
        if (builder_->GetNodeStack().empty()){
            builder_->SetRootPtr(node);
            builder_->SetRoot(*node);
        } else {
            if (builder_->GetNodeStack().back()->IsArray()){
                auto& array = std::get<Array>(builder_->GetNodeStack().back()->SetValue());
                array.emplace_back(*node);
            } else if (builder_->GetNodeStack().back()->IsDict()){
                auto& dict = std::get<Dict>(builder_->GetNodeStack().back()->SetValue());
                dict[builder_->GetKey()] = *node;
            }
        }
        auto * value_context = new ValueArrayContext(builder_);
        return *value_context;
    }

    ValueArrayContext &ArrayContext::Value(const Node::Value &node_value) {
        ValueLogic(*builder_);
        builder_->SetIsKey(false);
        Node * node = new Node();
        node->SetValue() = node_value;
        if (builder_->GetNodeStack().empty()){
            builder_->SetRootPtr(node);
            builder_->SetRoot(*node);
        } else {
            if (builder_->GetNodeStack().back()->IsArray()){
                auto& array = std::get<Array>(builder_->GetNodeStack().back()->SetValue());
                array.emplace_back(*node);
            } else if (builder_->GetNodeStack().back()->IsDict()){
                auto& dict = std::get<Dict>(builder_->GetNodeStack().back()->SetValue());
                dict[builder_->GetKey()] = *node;
            }
        }
        auto * value_context = new ValueArrayContext(builder_);
        return *value_context;
    }
// End Of %%%%%%%%%% %%%%%%%%%% %%%%%%%%%% Value %%%%%%%%%% %%%%%%%%%% %%%%%%%%%%

    void StartDictLogic(Builder& builder){
        if (!builder.GetNodeStack().empty()){
            if ((!builder.GetNodeStack().back()->IsArray()) && !builder.GetIsKey()){
                throw std::logic_error ("logic_error: StartDict called(1) not after constructor, not within Array or after key entered");
            }
        } else {
            if (builder.GetRootPtr() != nullptr){
                throw std::logic_error ("logic_error: StartDict called(2) not after constructor, not within Array or after key entered");
            }
        }
    }

// Begin Of %%%%%%%%%% %%%%%%%%%% %%%%%%%%%% StartDict %%%%%%%%%% %%%%%%%%%% %%%%%%%%%%
    Builder &Builder::BuildStartDict() {
        StartDictLogic(*this);
        is_previous_key_ = false;
        Dict node_dict;
        Node * node = new Node();
        node->SetValue() = node_dict;
        if (node_stack_.empty()){
            node_stack_.emplace_back(node);
        } else if (node_stack_.back()->IsArray()){
            auto& array = std::get<Array>(node_stack_.back()->SetValue());
            array.emplace_back(*node);
            node_stack_.emplace_back(&array.back());
        } else if (node_stack_.back()->IsDict()){
            auto& dict = std::get<Dict>(node_stack_.back()->SetValue());
            dict[key_] = *node;
            node_stack_.emplace_back(&dict.at(key_));
        }
        if (node_stack_.size() == 1){
            root_ptr_ = node_stack_.back();
        }
        return *this;
    }

    DictContext &KeyContext::StartDict() {
        StartDictLogic(*builder_);
        builder_->SetIsKey(false);
        Dict node_dict;
        Node * node = new Node();
        node->SetValue() = node_dict;
        if (builder_->GetNodeStack().empty()){
            builder_->EmplaceNode(node);
        } else if (builder_->GetNodeStack().back()->IsArray()){
            auto& array = std::get<Array>(builder_->GetNodeStack().back()->SetValue());
            array.emplace_back(*node);
            builder_->EmplaceNode(&array.back());
        } else if (builder_->GetNodeStack().back()->IsDict()){
            auto& dict = std::get<Dict>(builder_->GetNodeStack().back()->SetValue());
            dict[builder_->GetKey()] = *node;
            builder_->EmplaceNode(&dict.at(builder_->GetKey()));
        }
        if (builder_->GetNodeStack().size() == 1){
            builder_->SetRootPtr(builder_->GetNodeStack().back());
        }
        auto * dict_context = new DictContext(builder_);
        return *dict_context;
    }

    DictContext &ValueArrayContext::StartDict() {
        StartDictLogic(*builder_);
        builder_->SetIsKey(false);
        Dict node_dict;
        Node * node = new Node();
        node->SetValue() = node_dict;
        if (builder_->GetNodeStack().empty()){
            builder_->EmplaceNode(node);
        } else if (builder_->GetNodeStack().back()->IsArray()){
            auto& array = std::get<Array>(builder_->GetNodeStack().back()->SetValue());
            array.emplace_back(*node);
            builder_->EmplaceNode(&array.back());
        } else if (builder_->GetNodeStack().back()->IsDict()){
            auto& dict = std::get<Dict>(builder_->GetNodeStack().back()->SetValue());
            dict[builder_->GetKey()] = *node;
            builder_->EmplaceNode(&dict.at(builder_->GetKey()));
        }
        if (builder_->GetNodeStack().size() == 1){
            builder_->SetRootPtr(builder_->GetNodeStack().back());
        }
        auto * dict_context = new DictContext(builder_);
        return *dict_context;
    }

    DictContext &ArrayContext::StartDict() {
        StartDictLogic(*builder_);
        builder_->SetIsKey(false);
        Dict node_dict;
        Node * node = new Node();
        node->SetValue() = node_dict;
        if (builder_->GetNodeStack().empty()){
            builder_->EmplaceNode(node);
        } else if (builder_->GetNodeStack().back()->IsArray()){
            auto& array = std::get<Array>(builder_->GetNodeStack().back()->SetValue());
            array.emplace_back(*node);
            builder_->EmplaceNode(&array.back());
        } else if (builder_->GetNodeStack().back()->IsDict()){
            auto& dict = std::get<Dict>(builder_->GetNodeStack().back()->SetValue());
            dict[builder_->GetKey()] = *node;
            builder_->EmplaceNode(&dict.at(builder_->GetKey()));
        }
        if (builder_->GetNodeStack().size() == 1){
            builder_->SetRootPtr(builder_->GetNodeStack().back());
        }
        auto * dict_context = new DictContext(builder_);
        return *dict_context;
    }
// End Of %%%%%%%%%% %%%%%%%%%% %%%%%%%%%% StartDict %%%%%%%%%% %%%%%%%%%% %%%%%%%%%%

    void StartArrayLogic(Builder& builder){
        if (!builder.GetNodeStack().empty()){
            if ((!builder.GetNodeStack().back()->IsArray()) && !builder.GetIsKey()){
                throw std::logic_error ("logic_error: StartArray called(1) not after constructor, not within Array or after key entered");
            }
        } else {
            if (builder.GetRootPtr() != nullptr){
                throw std::logic_error ("logic_error: StartArray called(2) not after constructor, not within Array or after key entered");
            }
        }
    }

// Begin Of %%%%%%%%%% %%%%%%%%%% %%%%%%%%%% StartArray %%%%%%%%%% %%%%%%%%%% %%%%%%%%%%
    Builder &Builder::BuildStartArray() {
        StartArrayLogic(*this);
        is_previous_key_ = false;
        Array node_array;
        Node * node = new Node();
        node->SetValue() = node_array;
        if (node_stack_.empty()){
            node_stack_.emplace_back(node);
        } else if (node_stack_.back()->IsArray()){
            auto& array = std::get<Array>(node_stack_.back()->SetValue());
            array.emplace_back(*node);
            node_stack_.emplace_back(&array.back());
        } else if (node_stack_.back()->IsDict()){
            auto& dict = std::get<Dict>(node_stack_.back()->SetValue());
            dict[key_] = *node;
            node_stack_.emplace_back(&dict.at(key_));
        }
        if (node_stack_.size() == 1){
            root_ptr_ = node_stack_.back();
        }
        return *this;
    }

    ArrayContext &KeyContext::StartArray() {
        StartArrayLogic(*builder_);
        builder_->SetIsKey(false);
        Array node_array;
        Node * node = new Node();
        node->SetValue() = node_array;
        if (builder_->GetNodeStack().empty()){
            builder_->EmplaceNode(node);
        } else if (builder_->GetNodeStack().back()->IsArray()){
            auto& array = std::get<Array>(builder_->GetNodeStack().back()->SetValue());
            array.emplace_back(*node);
            builder_->EmplaceNode(&array.back());
        } else if (builder_->GetNodeStack().back()->IsDict()){
            auto& dict = std::get<Dict>(builder_->GetNodeStack().back()->SetValue());
            dict[builder_->GetKey()] = *node;
            builder_->EmplaceNode(&dict.at(builder_->GetKey()));
        }
        if (builder_->GetNodeStack().size() == 1){
            builder_->SetRootPtr(builder_->GetNodeStack().back());
        }
        auto * array_context = new ArrayContext(builder_);
        return *array_context;
    }

    ArrayContext &ValueArrayContext::StartArray() {
        StartArrayLogic(*builder_);
        builder_->SetIsKey(false);
        Array node_array;
        Node * node = new Node();
        node->SetValue() = node_array;
        if (builder_->GetNodeStack().empty()){
            builder_->EmplaceNode(node);
        } else if (builder_->GetNodeStack().back()->IsArray()){
            auto& array = std::get<Array>(builder_->GetNodeStack().back()->SetValue());
            array.emplace_back(*node);
            builder_->EmplaceNode(&array.back());
        } else if (builder_->GetNodeStack().back()->IsDict()){
            auto& dict = std::get<Dict>(builder_->GetNodeStack().back()->SetValue());
            dict[builder_->GetKey()] = *node;
            builder_->EmplaceNode(&dict.at(builder_->GetKey()));
        }
        if (builder_->GetNodeStack().size() == 1){
            builder_->SetRootPtr(builder_->GetNodeStack().back());
        }
        auto * array_context = new ArrayContext(builder_);
        return *array_context;
    }

    ArrayContext& ArrayContext::StartArray() {
        StartArrayLogic(*builder_);
        builder_->SetIsKey(false);
        Array node_array;
        Node * node = new Node();
        node->SetValue() = node_array;
        if (builder_->GetNodeStack().empty()){
            builder_->EmplaceNode(node);
        } else if (builder_->GetNodeStack().back()->IsArray()){
            auto& array = std::get<Array>(builder_->GetNodeStack().back()->SetValue());
            array.emplace_back(*node);
            builder_->EmplaceNode(&array.back());
        } else if (builder_->GetNodeStack().back()->IsDict()){
            auto& dict = std::get<Dict>(builder_->GetNodeStack().back()->SetValue());
            dict[builder_->GetKey()] = *node;
            builder_->EmplaceNode(&dict.at(builder_->GetKey()));
        }
        if (builder_->GetNodeStack().size() == 1){
            builder_->SetRootPtr(builder_->GetNodeStack().back());
        }
        auto * array_context = new ArrayContext(builder_);
        return *array_context;
    }
    // End Of %%%%%%%%%% %%%%%%%%%% %%%%%%%%%% StartArray %%%%%%%%%% %%%%%%%%%% %%%%%%%%%%

    void EndDictLogic(Builder& builder){
        if (builder.GetNodeStack().empty()){
            throw std::logic_error ("logic_error: EndDict called not within its context");
        } else if (!builder.GetNodeStack().back()->IsDict()){
            throw std::logic_error ("logic_error: EndDict called not within its context");
        }
    }

    // Begin Of %%%%%%%%%% %%%%%%%%%% %%%%%%%%%% EndDict %%%%%%%%%% %%%%%%%%%% %%%%%%%%%%
    Builder &Builder::EndDict() {
        EndDictLogic(*this);
        node_stack_.pop_back();
        if (node_stack_.empty()){
            root_ = *root_ptr_;
        }
        return *this;
    }

    Builder &ValueKeyContext::EndDict() {
        EndDictLogic(*builder_);
        builder_->PopNode();
        if (builder_->GetNodeStack().empty()){
            builder_->SetRoot(*builder_->GetRootPtr());
        }
        return *this->builder_;
    }

    Builder &ValueFullContext::EndDict() {
        EndDictLogic(*builder_);
        builder_->PopNode();
        if (builder_->GetNodeStack().empty()){
            builder_->SetRoot(*builder_->GetRootPtr());
        }
        return *this->builder_;
    }

    Builder &DictContext::EndDict() {
        EndDictLogic(*builder_);
        builder_->PopNode();
        if (builder_->GetNodeStack().empty()){
            builder_->SetRoot(*builder_->GetRootPtr());
        }
        return *this->builder_;
    }
    // End Of %%%%%%%%%% %%%%%%%%%% %%%%%%%%%% EndDict %%%%%%%%%% %%%%%%%%%% %%%%%%%%%%

    void EndArrayLogic(Builder& builder){
        if (builder.GetNodeStack().empty()){
            throw std::logic_error ("logic_error: EndArray called not within its context");
        } else if (!builder.GetNodeStack().back()->IsArray()){
            throw std::logic_error ("logic_error: EndArray called not within its context");
        }
    }

    // Begin Of %%%%%%%%%% %%%%%%%%%% %%%%%%%%%% EndArray %%%%%%%%%% %%%%%%%%%% %%%%%%%%%%
    Builder &Builder::EndArray() {
        EndArrayLogic(*this);
        node_stack_.pop_back();
        if (node_stack_.empty()){
            root_ = *root_ptr_;
        }
        return *this;
    }

    Builder &ValueArrayContext::EndArray() {
        EndArrayLogic(*builder_);
        builder_->PopNode();
        if (builder_->GetNodeStack().empty()){
            builder_->SetRoot(*builder_->GetRootPtr());
        }
        return *this->builder_;
    }

    Builder &ArrayContext::EndArray() {
        EndArrayLogic(*builder_);
        builder_->PopNode();
        if (builder_->GetNodeStack().empty()){
            builder_->SetRoot(*builder_->GetRootPtr());
        }
        return *this->builder_;
    }
    // End Of %%%%%%%%%% %%%%%%%%%% %%%%%%%%%% EndArray %%%%%%%%%% %%%%%%%%%% %%%%%%%%%%

    // Begin Of %%%%%%%%%% %%%%%%%%%% %%%%%%%%%% Build %%%%%%%%%% %%%%%%%%%% %%%%%%%%%%
    json::Node Builder::Build() {
        if (!node_stack_.empty() || root_ptr_ == nullptr){
            throw std::logic_error ("logic_error: Build called after constructor or with Array or Dict that wasn't complited");
        }
        return root_;
    }

    json::Node ValueFullContext::Build() {
        if (!builder_->GetNodeStack().empty() || builder_->GetRootPtr() == nullptr){
            throw std::logic_error ("logic_error: Build called after constructor or with Array or Dict that wasn't complited");
        }
        return builder_->GetRoot();
    }
    // End Of %%%%%%%%%% %%%%%%%%%% %%%%%%%%%% Build %%%%%%%%%% %%%%%%%%%% %%%%%%%%%%
}