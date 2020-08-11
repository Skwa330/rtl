#ifndef RTL_SEMA_SEMA_H
#define RTL_SEMA_SEMA_H

namespace rtl {
    namespace sema {
        struct BuiltinTypes {
            std::shared_ptr<TypeDeclaration> noneType;

            std::shared_ptr<TypeDeclaration> boolType;

            std::shared_ptr<TypeDeclaration> i8Type;
            std::shared_ptr<TypeDeclaration> i16Type;
            std::shared_ptr<TypeDeclaration> i32Type;
            std::shared_ptr<TypeDeclaration> i64Type;

            std::shared_ptr<TypeDeclaration> u8Type;
            std::shared_ptr<TypeDeclaration> u16Type;
            std::shared_ptr<TypeDeclaration> u32Type;
            std::shared_ptr<TypeDeclaration> u64Type;

            std::shared_ptr<TypeDeclaration> f32Type;
            std::shared_ptr<TypeDeclaration> f64Type;
        };
    }
}

#endif /* RTL_SEMA_SEMA_H */