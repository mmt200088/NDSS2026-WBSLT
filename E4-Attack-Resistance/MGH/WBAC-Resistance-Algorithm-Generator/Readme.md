# 白盒密码算法生成器
# WBAC-resistance Algorithm Generator
本项目用于生成普通AES、Chow白盒AES、史扬老师两种白盒AES（简称PA和NSNL）配置的组合

Purpose of this project is to make a WBAC-resistance Algorithm Generator, making it easy to generate any publicly published algorithms.

## 目录结构
## 0 Project Structure
```
WBRAG
 │
 ├── src
 │   ├── aes
 │   │   ├── aes.cc
 │   │   └── aes.h
 │   ├── cejo
 │   │   ├── cejo.cc
 │   │   └── cejo.h
 │   └── shiy
 │       ├── shiy.cc
 │       └── shiy.h
 ├── test
 │   └── ...
 ├── gen
 │   ├── table
 │   │   └── table.h
 │   ├── wbaes.c
 │   └── table_gen.cc
 └── libs
     ├── file_util
     ├── wbmatrix
     └── random
```
src - 各种AES算法实现

src - PRIVATE source files (.h and .cc files).

test - 测试代码

test - tests files if you write tests (indefinitely you should).

libs - 第三方库和自己实现的库

libs - third party or your own libraries you depend on.

gen - 查找表生成和加密算法

gen - generating look-up tables and encryption algorithm

## 1 部分工具最低版本要求
## 1 Tools Least Requirments
GNU Make 3.81

clang 14.0.3

## 2 使用说明
## 2 How to use
生成查找表

Generating look-up tables
```
cd ./gen
make table_gen
./table_gen
```
加密测试

encryption test
```
cd ./gen
make wbaes
./wbaes
```
接口说明见```./gen/table_gen.cc```文件内注释