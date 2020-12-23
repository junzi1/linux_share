CURRENT_DIR = $(abspath ./)

DEST = netif_op

#COMPILE_PREX ?= msdk-linux-
AR = $(COMPILE_PREX)ar
CC = $(COMPILE_PREX)gcc
NM = $(COMPILE_PREX)nm
CPP = $(COMPILE_PREX)g++
STRIP = $(COMPILE_PREX)strip

LINKFLAGS = -lm -pthread
CCFLAGS = \
	-g -fPIC -Wall  -Wno-missing-braces -Wno-unused-function -Wno-unused-variable -Wno-pointer-sign -Wno-unused-but-set-variable
CFLAGS = $(LINKFLAGS) $(CCFLAGS) $(INCFLAGS)

OUTPUT_DIR = $(CURRENT_DIR)/output/

#user app基准路径
USER_SRC_BASE_DIR =  $(CURRENT_DIR)
USER_INC_BASE_DIR = $(CURRENT_DIR)

#user app 编译文件 user头文件
SRC_DIRNAME = src 
INC_DIRNAME = include
DEMO_DIRNAME = demo
DEMO_NAME = demo.c


SRC_DIR_PARENT = $(addprefix $(USER_SRC_BASE_DIR)/,$(SRC_DIRNAME))
SRC_DIRS = $(foreach dir, $(SRC_DIR_PARENT), $(shell find $(dir) -type d))
INCS_PARENT = $(addprefix $(USER_INC_BASE_DIR)/,$(INC_DIRNAME))
INCS_DIRS = $(foreach dir, $(INCS_PARENT), $(shell find $(dir) -type d))
INCFLAGS = $(addprefix -I,$(INCS_DIRS))

USER_SRCS += $(foreach dir, $(SRC_DIRS), $(wildcard $(dir)/*.c)) 

USER_OBJS = $(addsuffix .o, $(basename  $(USER_SRCS) ) )



all: demo
	@mkdir -p $(OUTPUT_DIR)/bin/
	@cp ./$(DEMO_DIRNAME)/$(DEST) $(OUTPUT_DIR)/bin/ -rf
	@rm -rf ./$(DEMO_DIRNAME)/$(DEST)
	@echo "Build APP Finish"

demo: sdk
	@cd ./$(DEMO_DIRNAME)/;$(CC) -o  $(DEST) $(DEMO_NAME) -I$(OUTPUT_DIR)/include -L$(OUTPUT_DIR)/lib/static/ -l$(DEST) $(LINKFLAGS) $(CCFLAGS)
	
sdk: lib
	@echo "Build LIB FINISH"


lib: $(USER_OBJS)
    #static
	@mkdir -p $(OUTPUT_DIR)/include/
	@mkdir -p $(OUTPUT_DIR)/lib/static/
	@mkdir -p $(OUTPUT_DIR)/lib/shared/
	@$(AR)  -rcs $(OUTPUT_DIR)/lib/static/lib$(DEST).a $(USER_OBJS)
	@cp $(OUTPUT_DIR)/lib/static/lib$(DEST).a $(OUTPUT_DIR)/lib/static/lib$(DEST).a.stripped
	@$(STRIP) --strip-debug $(OUTPUT_DIR)/lib/static/lib$(DEST).a.stripped
    #shared
	@$(CC) -shared -DPIC -fPIC -rdynamic -Wl,-Bsymbolic -o $(OUTPUT_DIR)/lib/shared/lib$(DEST).so $(USER_OBJS);
	@cp $(OUTPUT_DIR)/lib/shared/lib$(DEST).so $(OUTPUT_DIR)/lib/shared/lib$(DEST).so.stripped
	@$(STRIP) --strip-debug $(OUTPUT_DIR)/lib/shared/lib$(DEST).so.stripped 
    #include
	@cp ./$(INC_DIRNAME)/*.h $(OUTPUT_DIR)/include/ -rf
	
%.o: %.c
	@$(CC) -o  $@ -c $< $(CFLAGS)
	
	
.PHONY:clean all
clean:
	@echo "clean finish"
	@rm -rf *~
	@rm -rf $(USER_OBJS)
	@rm -rf $(OUTPUT_DIR)
