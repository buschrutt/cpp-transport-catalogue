#include "json_builder.h"

namespace json {

    // %%%%%%%%%% %%%%%%%%%% %%%%%%%%%% method logics %%%%%%%%%% %%%%%%%%%% %%%%%%%%%%
    void Builder::KeyLogic(Builder& builder, const std::string &node_key){
        builder.is_previous_key_ = true;
        builder.key_ = node_key;
        Node * pair_node = new Node();
        auto& dict = std::get<Dict>(builder.node_stack_.back()->SetValue());
        dict.insert({node_key, *pair_node});
    }

    void Builder::ValueLogic(Builder& builder, const Node::Value& node_value){
        builder.is_previous_key_ = false;
        Node * node = new Node();
        node->SetValue() = node_value;
        if (builder.node_stack_.empty()){
            builder.root_ptr_ = node;
            builder.root_ = *node;
        } else {
            if (builder.node_stack_.back()->IsArray()){
                auto& array = std::get<Array>(builder.node_stack_.back()->SetValue());
                array.emplace_back(*node);
            } else if (builder.node_stack_.back()->IsDict()){
                auto& dict = std::get<Dict>(builder.node_stack_.back()->SetValue());
                dict[builder.key_] = *node;
            }
        }
    }

    void Builder::StartDictLogic(Builder& builder){
        builder.is_previous_key_ = false;
        Dict node_dict;
        Node * node = new Node();
        node->SetValue() = node_dict;
        if (builder.node_stack_.empty()){
            builder.node_stack_.emplace_back(node);
        } else if (builder.node_stack_.back()->IsArray()){
            auto& array = std::get<Array>(builder.node_stack_.back()->SetValue());
            array.emplace_back(*node);
            builder.node_stack_.emplace_back(&array.back());
        } else if (builder.node_stack_.back()->IsDict()){
            auto& dict = std::get<Dict>(builder.node_stack_.back()->SetValue());
            dict[builder.key_] = *node;
            builder.node_stack_.emplace_back(&dict.at(builder.key_));
        }
        if (builder.node_stack_.size() == 1){
            builder.root_ptr_ = builder.node_stack_.back();
        }
    }

    void Builder::StartArrayLogic(Builder& builder){
        builder.is_previous_key_ = false;
        Array node_array;
        Node * node = new Node();
        node->SetValue() = node_array;
        if (builder.node_stack_.empty()){
            builder.node_stack_.emplace_back(node);
        } else if (builder.node_stack_.back()->IsArray()){
            auto& array = std::get<Array>(builder.node_stack_.back()->SetValue());
            array.emplace_back(*node);
            builder.node_stack_.emplace_back(&array.back());
        } else if (builder.node_stack_.back()->IsDict()){
            auto& dict = std::get<Dict>(builder.node_stack_.back()->SetValue());
            dict[builder.key_] = *node;
            builder.node_stack_.emplace_back(&dict.at(builder.key_));
        }
        if (builder.node_stack_.size() == 1){
            builder.root_ptr_ = builder.node_stack_.back();
        }
    }

    void Builder::EndArrayLogic(Builder& builder){
        builder.node_stack_.pop_back();
        if (builder.node_stack_.empty()){
            builder.root_ = *builder.root_ptr_;
        }
    }

    void Builder::EndDictLogic(Builder& builder){
        builder.node_stack_.pop_back();
        if (builder.node_stack_.empty()){
            builder.root_ = *builder.root_ptr_;
        }
    }

    // Begin Of %%%%%%%%%% %%%%%%%%%% %%%%%%%%%% Key %%%%%%%%%% %%%%%%%%%% %%%%%%%%%%
    Builder::KeyContext Builder::Key(const std::string &node_key) {
        KeyLogic(*this, node_key);
        KeyContext key_context(this);
        return key_context;
    }

    Builder::KeyContext& Builder::BaseContext2::Key(const std::string &node_key) {
        KeyLogic(*builder_, node_key);
        auto * key_context = new KeyContext(builder_);
        return *key_context;
    }

    Builder::KeyContext& Builder::ValueFullContext::Key(const std::string &node_key) {
        KeyLogic(*builder_, node_key);
        auto * key_context = new KeyContext(builder_);
        return *key_context;
    }

    // Begin Of %%%%%%%%%% %%%%%%%%%% %%%%%%%%%% Value %%%%%%%%%% %%%%%%%%%% %%%%%%%%%%

    Builder Builder::Value(const Node::Value& node_value) {
        ValueLogic(*this, node_value);
        if (!node_stack_.empty()){
            if (node_stack_.back()->IsArray()){
                ValueArrayContext value_context(this);
                return *value_context.GetBuilder();
            }
        }
        ValueFullContext value_context(this);
        return *value_context.GetBuilder();
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

    // Begin Of %%%%%%%%%% %%%%%%%%%% %%%%%%%%%% StartDict %%%%%%%%%% %%%%%%%%%% %%%%%%%%%%
    Builder::DictContext Builder::StartDict() {
        StartDictLogic(*this);
        DictContext dict_context(this);
        return dict_context;
    }

    Builder::DictContext &Builder::BaseContext1::StartDict() {
        StartDictLogic(*builder_);
        auto * dict_context = new DictContext(builder_);
        return *dict_context;
    }

    // Begin Of %%%%%%%%%% %%%%%%%%%% %%%%%%%%%% StartArray %%%%%%%%%% %%%%%%%%%% %%%%%%%%%%
    Builder::ArrayContext Builder::StartArray() {
        StartArrayLogic(*this);
        ArrayContext array_context(this);
        return array_context;
    }

    Builder::ArrayContext &Builder::BaseContext1::StartArray() {
        StartArrayLogic(*builder_);
        auto * array_context = new ArrayContext(builder_);
        return *array_context;
    }

    // Begin Of %%%%%%%%%% %%%%%%%%%% %%%%%%%%%% EndDict %%%%%%%%%% %%%%%%%%%% %%%%%%%%%%
    Builder &Builder::EndDict() {
        EndDictLogic(*this);
        return *this;
    }

    Builder &Builder::BaseContext2::EndDict() {
        EndDictLogic(*builder_);
        return *this->builder_;
    }

    Builder &Builder::ValueFullContext::EndDict() {
        EndDictLogic(*builder_);
        return *this->builder_;
    }

    // Begin Of %%%%%%%%%% %%%%%%%%%% %%%%%%%%%% EndArray %%%%%%%%%% %%%%%%%%%% %%%%%%%%%%
    Builder &Builder::EndArray() {
        EndArrayLogic(*this);
        return *this;
    }

    Builder &Builder::ValueArrayContext::EndArray() {
        EndArrayLogic(*builder_);
        return *this->builder_;
    }

    // Begin Of %%%%%%%%%% %%%%%%%%%% %%%%%%%%%% Build %%%%%%%%%% %%%%%%%%%% %%%%%%%%%%
    json::Node Builder::Build() {
        return root_;
    }

    json::Node Builder::ValueFullContext::Build() {
        return builder_->root_;
    }

}