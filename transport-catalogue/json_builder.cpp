#include "json_builder.h"

namespace json {

    // Begin Of %%%%%%%%%% %%%%%%%%%% %%%%%%%%%% Key %%%%%%%%%% %%%%%%%%%% %%%%%%%%%%
    void KeyLogic(Builder& builder, const std::string &node_key){
        builder.SetIsKey(true);
        builder.SetKey(node_key);
        Node * pair_node = new Node();
        auto& dict = std::get<Dict>(builder.GetNodeStack().back()->SetValue());
        dict.insert({node_key, *pair_node});
    }

    Builder& Builder::BuildKey(const std::string &node_key) {
        KeyLogic(*this, node_key);
        return *this;
    }

    Builder::KeyContext& Builder::ValueFullContext::Key(const std::string &node_key) {
        KeyLogic(*builder_, node_key);
        auto * key_context = new KeyContext(builder_);
        return *key_context;
    }

    Builder::KeyContext& Builder::ValueKeyContext::Key(const std::string &node_key) {
        KeyLogic(*builder_, node_key);
        auto * key_context = new KeyContext(builder_);
        return *key_context;
    }

    Builder::KeyContext& Builder::DictContext::Key(const std::string &node_key) {
        KeyLogic(*builder_, node_key);
        auto * key_context = new KeyContext(builder_);
        return *key_context;
    }

    // End Of %%%%%%%%%% %%%%%%%%%% %%%%%%%%%% Key %%%%%%%%%% %%%%%%%%%% %%%%%%%%%%

    // Begin Of %%%%%%%%%% %%%%%%%%%% %%%%%%%%%% Value %%%%%%%%%% %%%%%%%%%% %%%%%%%%%%
    void ValueLogic(Builder& builder, const Node::Value& node_value){
        builder.SetIsKey(false);
        Node * node = new Node();
        node->SetValue() = node_value;
        if (builder.GetNodeStack().empty()){
            builder.SetRootPtr(node);
            builder.SetRoot(*node);
        } else {
            if (builder.GetNodeStack().back()->IsArray()){
                auto& array = std::get<Array>(builder.GetNodeStack().back()->SetValue());
                array.emplace_back(*node);
            } else if (builder.GetNodeStack().back()->IsDict()){
                auto& dict = std::get<Dict>(builder.GetNodeStack().back()->SetValue());
                dict[builder.GetKey()] = *node;
            }
        }
    }

    Builder &Builder::BuildValue(const Node::Value& node_value) {
        ValueLogic(*this, node_value);
        return *this;
    }

    Builder::ValueKeyContext &Builder::KeyContext::Value(const Node::Value &node_value) {
        ValueLogic(*builder_, node_value);
        auto * value_context = new ValueKeyContext(builder_);
        return *value_context;
    }

    Builder::ValueArrayContext &Builder::ValueArrayContext::Value(const Node::Value &node_value) {
        ValueLogic(*builder_, node_value);
        auto * value_context = new ValueArrayContext(builder_);
        return *value_context;
    }

    Builder::ValueArrayContext &Builder::ArrayContext::Value(const Node::Value &node_value) {
        ValueLogic(*builder_, node_value);
        auto * value_context = new ValueArrayContext(builder_);
        return *value_context;
    }
    // End Of %%%%%%%%%% %%%%%%%%%% %%%%%%%%%% Value %%%%%%%%%% %%%%%%%%%% %%%%%%%%%%

    // Begin Of %%%%%%%%%% %%%%%%%%%% %%%%%%%%%% StartDict %%%%%%%%%% %%%%%%%%%% %%%%%%%%%%
    void StartDictLogic(Builder& builder){
        builder.SetIsKey(false);
        Dict node_dict;
        Node * node = new Node();
        node->SetValue() = node_dict;
        if (builder.GetNodeStack().empty()){
            builder.EmplaceNode(node);
        } else if (builder.GetNodeStack().back()->IsArray()){
            auto& array = std::get<Array>(builder.GetNodeStack().back()->SetValue());
            array.emplace_back(*node);
            builder.EmplaceNode(&array.back());
        } else if (builder.GetNodeStack().back()->IsDict()){
            auto& dict = std::get<Dict>(builder.GetNodeStack().back()->SetValue());
            dict[builder.GetKey()] = *node;
            builder.EmplaceNode(&dict.at(builder.GetKey()));
        }
        if (builder.GetNodeStack().size() == 1){
            builder.SetRootPtr(builder.GetNodeStack().back());
        }
    }

    Builder &Builder::BuildStartDict() {
        StartDictLogic(*this);
        return *this;
    }

    Builder::DictContext &Builder::KeyContext::StartDict() {
        StartDictLogic(*builder_);
        auto * dict_context = new DictContext(builder_);
        return *dict_context;
    }

    Builder::DictContext &Builder::ValueArrayContext::StartDict() {
        StartDictLogic(*builder_);
        auto * dict_context = new DictContext(builder_);
        return *dict_context;
    }

    Builder::DictContext &Builder::ArrayContext::StartDict() {
        StartDictLogic(*builder_);
        auto * dict_context = new DictContext(builder_);
        return *dict_context;
    }
    // End Of %%%%%%%%%% %%%%%%%%%% %%%%%%%%%% StartDict %%%%%%%%%% %%%%%%%%%% %%%%%%%%%%

    // Begin Of %%%%%%%%%% %%%%%%%%%% %%%%%%%%%% StartArray %%%%%%%%%% %%%%%%%%%% %%%%%%%%%%
    void StartArrayLogic(Builder& builder){
        builder.SetIsKey(false);
        Array node_array;
        Node * node = new Node();
        node->SetValue() = node_array;
        if (builder.GetNodeStack().empty()){
            builder.EmplaceNode(node);
        } else if (builder.GetNodeStack().back()->IsArray()){
            auto& array = std::get<Array>(builder.GetNodeStack().back()->SetValue());
            array.emplace_back(*node);
            builder.EmplaceNode(&array.back());
        } else if (builder.GetNodeStack().back()->IsDict()){
            auto& dict = std::get<Dict>(builder.GetNodeStack().back()->SetValue());
            dict[builder.GetKey()] = *node;
            builder.EmplaceNode(&dict.at(builder.GetKey()));
        }
        if (builder.GetNodeStack().size() == 1){
            builder.SetRootPtr(builder.GetNodeStack().back());
        }
    }

    Builder &Builder::BuildStartArray() {
        StartArrayLogic(*this);
        return *this;
    }

    Builder::ArrayContext &Builder::KeyContext::StartArray() {
        StartArrayLogic(*builder_);
        auto * array_context = new ArrayContext(builder_);
        return *array_context;
    }

    Builder::ArrayContext &Builder::ValueArrayContext::StartArray() {
        StartArrayLogic(*builder_);
        auto * array_context = new ArrayContext(builder_);
        return *array_context;
    }

    Builder::ArrayContext& Builder::ArrayContext::StartArray() {
        StartArrayLogic(*builder_);
        auto * array_context = new ArrayContext(builder_);
        return *array_context;
    }
    // End Of %%%%%%%%%% %%%%%%%%%% %%%%%%%%%% StartArray %%%%%%%%%% %%%%%%%%%% %%%%%%%%%%

    // Begin Of %%%%%%%%%% %%%%%%%%%% %%%%%%%%%% EndDict %%%%%%%%%% %%%%%%%%%% %%%%%%%%%%
    void EndDictLogic(Builder& builder){
        builder.PopNode();
        if (builder.GetNodeStack().empty()){
            builder.SetRoot(*builder.GetRootPtr());
        }
    }

    Builder &Builder::EndDict() {
        EndDictLogic(*this);
        return *this;
    }

    Builder &Builder::ValueKeyContext::EndDict() {
        EndDictLogic(*builder_);
        return *this->builder_;
    }

    Builder &Builder::ValueFullContext::EndDict() {
        EndDictLogic(*builder_);
        return *this->builder_;
    }

    Builder &Builder::DictContext::EndDict() {
        EndDictLogic(*builder_);
        return *this->builder_;
    }
    // End Of %%%%%%%%%% %%%%%%%%%% %%%%%%%%%% EndDict %%%%%%%%%% %%%%%%%%%% %%%%%%%%%%

    // Begin Of %%%%%%%%%% %%%%%%%%%% %%%%%%%%%% EndArray %%%%%%%%%% %%%%%%%%%% %%%%%%%%%%
    void EndArrayLogic(Builder& builder){
        builder.PopNode();
        if (builder.GetNodeStack().empty()){
            builder.SetRoot(*builder.GetRootPtr());
        }
    }

    Builder &Builder::EndArray() {
        EndArrayLogic(*this);
        return *this;
    }

    Builder &Builder::ValueArrayContext::EndArray() {
        EndArrayLogic(*builder_);
        return *this->builder_;
    }

    Builder &Builder::ArrayContext::EndArray() {
        EndArrayLogic(*builder_);
        return *this->builder_;
    }
    // End Of %%%%%%%%%% %%%%%%%%%% %%%%%%%%%% EndArray %%%%%%%%%% %%%%%%%%%% %%%%%%%%%%

    // Begin Of %%%%%%%%%% %%%%%%%%%% %%%%%%%%%% Build %%%%%%%%%% %%%%%%%%%% %%%%%%%%%%

    json::Node Builder::Build() {
        return root_;
    }

    json::Node Builder::ValueFullContext::Build() {
        return builder_->GetRoot();
    }
    // End Of %%%%%%%%%% %%%%%%%%%% %%%%%%%%%% Build %%%%%%%%%% %%%%%%%%%% %%%%%%%%%%
}