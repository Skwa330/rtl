#include "rtl/Sema/Type.h"

namespace rtl {
    namespace sema {
        TypeDeclaration::TypeDeclaration(Tag tag) {
            this->tag = tag;
        }

        TypeDeclaration::Tag TypeDeclaration::getTag() const {
            return tag;
        }

        Type::Type(const std::shared_ptr<TypeDeclaration> &decl, std::uint32_t pointer) {
            this->decl = decl;
            this->pointer = pointer;
        }

        std::uint32_t Type::getPointer() const {
            return pointer;
        }
    }
}