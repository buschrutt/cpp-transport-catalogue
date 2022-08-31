#pragma once
#include "json.h"
#include <optional>
#include <utility>

namespace json {

    class Builder {

    public:

        class ValueKeyContext;

        class ArrayContext;

        class DictContext;

        class KeyContext;

        static void KeyLogic(Builder& builder, const std::string &node_key);

        static void ValueLogic(Builder& builder, const Node::Value& node_value);

        static void StartDictLogic(Builder& builder);

        static void StartArrayLogic(Builder& builder);

        static void EndArrayLogic(Builder& builder);

        static void EndDictLogic(Builder& builder);

        class ContextConstructor {
        public:
            explicit ContextConstructor(Builder * builder): builder_(builder){};
            Builder * GetBuilder (){
                return builder_;
            }
            ContextConstructor GetContext (){
                return *this;
            }
        protected:
            Builder * builder_{};
        };

        class BaseContext1 : public ContextConstructor {
        public:
            explicit BaseContext1(Builder * builder): ContextConstructor(builder){};
            ArrayContext& StartArray();
            DictContext& StartDict();
        };

        class BaseContext2 : public ContextConstructor {
        public:
            explicit BaseContext2(Builder * builder): ContextConstructor(builder){};
            KeyContext& Key(const std::string& node_key);
            Builder& EndDict();
        };

        class KeyContext : public BaseContext1 {
        public:
            explicit KeyContext(Builder * builder): BaseContext1(builder){};
            ValueKeyContext& Value(const Node::Value& node_value);
        };

        class ValueKeyContext : public BaseContext2 {
        public:
            explicit ValueKeyContext(Builder * builder): BaseContext2(builder){}
        };

        class ValueArrayContext : public BaseContext1 {
        public:
            explicit ValueArrayContext(Builder * builder): BaseContext1(builder){}
            ValueArrayContext& Value(const Node::Value& node_value);
            Builder& EndArray();
        };

        class ValueFullContext : public ValueArrayContext {
        public:
            explicit ValueFullContext(Builder *builder) : ValueArrayContext(builder) {}
            KeyContext& Key(const std::string& node_key);
            Builder& EndDict();
            json::Node Build();
        };

        class DictContext : public BaseContext2 {
        public:
            explicit DictContext(Builder * builder): BaseContext2(builder){};
        };

        class ArrayContext : public ValueArrayContext {
        public:
            explicit ArrayContext(Builder * builder): ValueArrayContext (builder){};
        };

        KeyContext Key(const std::string& node_key);

        Builder Value(const Node::Value& node_value);

        DictContext StartDict();

        ArrayContext StartArray();

        Builder& EndDict();

        Builder& EndArray();

        json::Node Build();

    private:
        json::Node root_;
        json::Node * root_ptr_;
        std::vector<json::Node*> node_stack_;
        std::string key_;
        bool is_previous_key_ = false;
    };

}