

#查找当前目录下的所有源文件，保存到变量UTIL2_SRCS
aux_source_directory(. UTIL2_SRCS)

#生成链接库
add_library(util2_park ${UTIL2_SRCS} )


