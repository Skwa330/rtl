#ifndef RTL_SEMA_TYPE_H
#define RTL_SEMA_TYPE_H

#include "rtl/Parser/AST.h"

namespace rtl {
    namespace sema {
        class Type;
        struct FunctionPrototype {
            std::shared_ptr<Type> rt;
            std::vector<std::shared_ptr<Type>> paramTypes;
        };

        class TypeDeclaration {
        public:
            using InfoType = std::variant<FunctionPrototype>;//, parser::ASTStructureDescription, parser::ASTEnumerationDescription> // A UNION SHOULD JUST BE A STRUCT WITH A 'UNION' FLAG SET.

            enum class Tag {
                None,
                Bool,
                I8,
                I16,
                I32,
                I64,
                U8,
                U16,
                U32,
                U64,
                F32,
                F64,

                FunctionPrototype,

                Structure,
                Enumeration,
                Union
            };
        private:
            Tag tag;
        public:
            InfoType info;

            TypeDeclaration(Tag tag);

            Tag getTag() const;
        };

        class Type {
        private:
            std::uint32_t pointer;
        public:
            std::shared_ptr<TypeDeclaration> decl;

            Type(const std::shared_ptr<TypeDeclaration> &decl, std::uint32_t pointer);

            std::uint32_t getPointer() const;
        };
    }
}

#endif /* RTL_SEMA_TYPE_H */