#include "rtl/Sema/Driver.h"

#include <fmt/format.h>

using namespace rtl::parser;

namespace rtl {
    namespace sema {
        Driver::Driver(std::vector<std::shared_ptr<ASTNode>> &nodes, std::vector<core::Error> &errors) : nodes(nodes), errors(errors) {
            builtinTypes = std::make_shared<BuiltinTypes>();

            builtinTypes->noneType = std::make_shared<TypeDeclaration>(TypeDeclaration::Tag::None);

            builtinTypes->boolType = std::make_shared<TypeDeclaration>(TypeDeclaration::Tag::Bool);

            builtinTypes->i8Type = std::make_shared<TypeDeclaration>(TypeDeclaration::Tag::I8);
            builtinTypes->i16Type = std::make_shared<TypeDeclaration>(TypeDeclaration::Tag::I16);
            builtinTypes->i32Type = std::make_shared<TypeDeclaration>(TypeDeclaration::Tag::I32);
            builtinTypes->i64Type = std::make_shared<TypeDeclaration>(TypeDeclaration::Tag::I64);

            builtinTypes->u8Type = std::make_shared<TypeDeclaration>(TypeDeclaration::Tag::U8);
            builtinTypes->u16Type = std::make_shared<TypeDeclaration>(TypeDeclaration::Tag::U16);
            builtinTypes->u32Type = std::make_shared<TypeDeclaration>(TypeDeclaration::Tag::U32);
            builtinTypes->u64Type = std::make_shared<TypeDeclaration>(TypeDeclaration::Tag::U64);

            builtinTypes->f32Type = std::make_shared<TypeDeclaration>(TypeDeclaration::Tag::F32);
            builtinTypes->f64Type = std::make_shared<TypeDeclaration>(TypeDeclaration::Tag::F64);
        }

        void Driver::run() {
            auto validator = std::make_shared<Validator>(nodes, errors);
            validator->run();

            // auto typer = std::make_shared<Typer>(builtinTypes, nodes, errors);
            // typer->run();

            // auto typeChecker = std::make_shared<TypeChecker>(builtinTypes, nodes, errors);
            // typeChecker->run();
        }
    }
}