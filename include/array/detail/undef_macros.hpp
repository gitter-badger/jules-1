#ifndef JULES_ARRAY_DETAIL_DEFINE_MACROS_H
static_assert(false, "array macros are not defined.");
#else

#undef BASE_NDARRAY
#undef REF_NDARRAY
#undef INDIRECT_NDARRAY
#undef BINARY_EXPR_NDARRAY
#undef UNARY_EXPR_NDARRAY

#undef UNPACK

#undef UNARY_APPLY_OPERATION
#undef UNARY_OPERATION
#undef UNARY_OPERATIONS_LIST

#undef BINARY_APPLY_OPERATION
#undef BINARY_OPERATION
#undef BINARY_LEFT_TYPE_OPERATION
#undef BINARY_RIGHT_TYPE_OPERATION
#undef BINARY_TYPE_OPERATION
#undef BINARY_COMBINATIONS
#undef BINARY_APPLY_COMBINATIONS
#undef BINARY_OPERATIONS_LIST

#undef OPERATIONS_LIST

#undef BINARY_LEFT_TYPE_OPERATION_DECLARATION
#undef BINARY_LEFT_TYPE_OPERATION_DEFINITION

#undef FRIEND_OPERATIONS_DECLARATION
#undef FRIEND_OPERATIONS_DEFINITION
#undef FRIEND_OPERATIONS

#undef JULES_ARRAY_DETAIL_DEFINE_MACROS_H
#endif // JULES_ARRAY_DETAIL_DEFINE_MACROS_H
