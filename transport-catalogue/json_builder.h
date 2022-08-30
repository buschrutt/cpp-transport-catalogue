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

        protected:
            Builder * builder_{};
        };

        class BaseContext1 : public ContextConstructor {
        public:
            explicit BaseContext1(Builder * builder): ContextConstructor(builder){};
            ArrayContext& StartArray();
            DictContext& StartDict();
        };

        class KeyContext : public BaseContext1 {
        public:
            explicit KeyContext(Builder * builder): BaseContext1(builder){};
            ValueKeyContext& Value(const Node::Value& node_value);
            KeyContext GetContext (){
                return *this;
            }
        };

        class ValueKeyContext : public ContextConstructor {
        public:
            explicit ValueKeyContext(Builder * builder): ContextConstructor(builder){}
            KeyContext& Key(const std::string& node_key);
            Builder& EndDict();
            ValueKeyContext& GetContext (){
                return *this;
            }
        };

        class ValueArrayContext : public BaseContext1 {
        public:
            explicit ValueArrayContext(Builder * builder): BaseContext1(builder){}
            ValueArrayContext& Value(const Node::Value& node_value);
            Builder& EndArray();
            ValueArrayContext& GetContext (){
                return *this;
            }
        };

        class ValueFullContext : public ValueArrayContext {
        public:
            explicit ValueFullContext(Builder *builder) : ValueArrayContext(builder) {}
            KeyContext& Key(const std::string& node_key);
            Builder& EndDict();
            json::Node Build();
            ValueFullContext& GetFullContext (){
                return *this;
            }
        };

        class DictContext : public ContextConstructor {
        public:
            explicit DictContext(Builder * builder): ContextConstructor(builder){};
            KeyContext& Key(const std::string& node_key);
            Builder& EndDict();
            DictContext GetContext (){
                return *this;
            }
        };

        class ArrayContext : public ValueArrayContext {
        public:
            explicit ArrayContext(Builder * builder): ValueArrayContext (builder){};
            ArrayContext GetArrayContext (){
                return *this;
            }
        };

        Builder& BuildKey(const std::string &node_key);
        KeyContext Key(const std::string& node_key) {
            KeyContext key_context(&BuildKey(node_key));
            return key_context.GetContext();
        }

        Builder& BuildValue(const Node::Value& node_value);
        Builder& Value(const Node::Value& node_value) {
            if (is_previous_key_){
                ValueKeyContext value_context( &BuildValue(node_value));
                return *value_context.GetBuilder();
            } else if (!node_stack_.empty()){
                if (node_stack_.back()->IsArray()){
                    ValueArrayContext value_context( &BuildValue(node_value));
                    return *value_context.GetBuilder();
                }
            }
            ValueFullContext value_context( &BuildValue(node_value));
            return *value_context.GetBuilder();
        }

        Builder& BuildStartDict();
        DictContext StartDict() {
            DictContext dict_context(&BuildStartDict());
            return dict_context.GetContext();
        }

        Builder& BuildStartArray();
        ArrayContext StartArray() {
            ArrayContext array_context(&BuildStartArray());
            return array_context.GetArrayContext();
        }

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