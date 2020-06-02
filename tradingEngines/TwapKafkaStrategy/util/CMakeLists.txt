

#把当前目录的cpp文件，加入到变量UTIL_SRCS
aux_source_directory(. UTIL_SRCS)

#生成链接库
add_library(util_twap ${UTIL_SRCS} )


