sed -i '90 a bool uses_vector_instructions() {\n return false;\n }\n' $1/folly/container/detail/F14MapFallback.h
sed -i '304 a bool uses_vector_instructions() {\n return true;\n }\n' $1/folly/container/F14Map.h
