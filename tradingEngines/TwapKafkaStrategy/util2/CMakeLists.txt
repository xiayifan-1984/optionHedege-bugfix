

#把当前目录的cpp文件，加入到变量UTIL2_SRCS
aux_source_directory(. UTIL2_SRCS)

#生成链接库
add_library(util2_twap ${UTIL2_SRCS} )


