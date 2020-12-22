## CubeCode
**测试使用项目**   

### 说明
- 包含RapidJson(C++ Json库)   
- 包含zlib库(压缩库)  
- 包含SystemV:消息队列、信号量、共享内存  
- 包含Posix:内存映射  

### 编译  
直接执行脚本./compiler.sh可以查看帮助信息  
- 第一个参数为交叉编译使能项: ON为使能交叉编译,OFF为失能交叉编译(PC下编译)  
- 第二个参数为Release版本号,格式为"1.0.0"  
- 第三个参数为Debug版本号,格式为"1.0.0.1"  
   
#### PC下编译  
1. 执行命令 `./compiler.sh OFF 1.0.0 1.0.0.1`    
3. 在build_x86下生成可执行文件CubeCode   

#### ARM下编译  
1. 修改./compiler.sh的交叉编译工具链,见以下内容 
> 交叉编译工具链路径，只需修改这里的路径，无需修改CMAKE配置文件中的路径  
> `toolchain=prebuilts/gcc/linux-x86/aarch64/gcc-linaro-6.3.1-2017.05-x86_64_aarch64-linux-gnu/bin`  
> 交叉编译链     
> `cross_toolchain=aarch64-linux-gnu-`                  
> 交叉编译主机                
> `cross_toolchain_host=aarch64-linux-gnu`  
      
2. 执行命令 `./compiler.sh ON 1.0.0 1.0.0.1` 
3. 在build_arm下生成可执行文件CubeCode    

### 测试用例
在test目录下有以下测试用例,(通过CMakeLists.txt中的`option(TEST_BUILD "Excute Test Source Build" ON)`控制编译)
- POSIX的内存映射的读写测试
> PC下编译生成文件路径: build_x86/test/POSIX  
> ARM下编译生成文件路径: build_arm/test/POSIX  
    
- SystemV的共享内存, 信号量, 消息队列的读写测试  
> PC下编译生成文件路径: build_x86/test/SystemV  
> ARM下编译生成文件路径: build_arm/test/SystemV  
