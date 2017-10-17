##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=AutoGentoo
ConfigurationName      :=Debug
WorkspacePath          :=/home/atuser/git
ProjectPath            :=/home/atuser/git/AutoGentoo
IntermediateDirectory  :=./Debug
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=
Date                   :=10/16/17
CodeLitePath           :=/home/atuser/.codelite
LinkerName             :=/usr/bin/g++
SharedObjectLinkerName :=/usr/bin/g++ -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=.o.d
PreprocessSuffix       :=.i
DebugSwitch            :=-g 
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
SourceSwitch           :=-c 
OutputFile             :=$(IntermediateDirectory)/$(ProjectName)
Preprocessors          :=
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :="AutoGentoo.txt"
PCHCompileFlags        :=
MakeDirCommand         :=mkdir -p
LinkOptions            :=  
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). 
IncludePCH             := 
RcIncludePath          := 
Libs                   := 
ArLibs                 :=  
LibPath                := $(LibraryPathSwitch). 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := /usr/bin/ar rcu
CXX      := /usr/bin/g++
CC       := /usr/bin/gcc
CXXFLAGS :=  -g -O0 -Wall $(Preprocessors)
CFLAGS   :=  -g -O0 -Wall $(Preprocessors)
ASFLAGS  := 
AS       := /usr/bin/as


##
## User defined environment variables
##
CodeLiteDir:=/usr/share/codelite
Srcs=src/_string.c src/autogentoo.c src/chroot.c src/crossdev.c src/emerge.c src/hash.c src/ip_convert.c src/ip_scan.c src/kernel.c src/list.c \
	src/request.c src/response.c src/serve_client.c src/server.c src/srv_handle.c src/testserveclient.c src/testserver.c src/update.c src/update-config.c 

Objects0=$(IntermediateDirectory)/src__string.c$(ObjectSuffix) $(IntermediateDirectory)/src_autogentoo.c$(ObjectSuffix) $(IntermediateDirectory)/src_chroot.c$(ObjectSuffix) $(IntermediateDirectory)/src_crossdev.c$(ObjectSuffix) $(IntermediateDirectory)/src_emerge.c$(ObjectSuffix) $(IntermediateDirectory)/src_hash.c$(ObjectSuffix) $(IntermediateDirectory)/src_ip_convert.c$(ObjectSuffix) $(IntermediateDirectory)/src_ip_scan.c$(ObjectSuffix) $(IntermediateDirectory)/src_kernel.c$(ObjectSuffix) $(IntermediateDirectory)/src_list.c$(ObjectSuffix) \
	$(IntermediateDirectory)/src_request.c$(ObjectSuffix) $(IntermediateDirectory)/src_response.c$(ObjectSuffix) $(IntermediateDirectory)/src_serve_client.c$(ObjectSuffix) $(IntermediateDirectory)/src_server.c$(ObjectSuffix) $(IntermediateDirectory)/src_srv_handle.c$(ObjectSuffix) $(IntermediateDirectory)/src_testserveclient.c$(ObjectSuffix) $(IntermediateDirectory)/src_testserver.c$(ObjectSuffix) $(IntermediateDirectory)/src_update.c$(ObjectSuffix) $(IntermediateDirectory)/src_update-config.c$(ObjectSuffix) 



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild MakeIntermediateDirs
all: $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)

MakeIntermediateDirs:
	@test -d ./Debug || $(MakeDirCommand) ./Debug


$(IntermediateDirectory)/.d:
	@test -d ./Debug || $(MakeDirCommand) ./Debug

PreBuild:
##
## Clean
##
clean:
	$(RM) -r ./Debug/


