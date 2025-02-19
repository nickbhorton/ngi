CMAKE_BUILD_DIR:=./build
CMAKE_SOURCE_DIR:=.

all: assignment2 assignment3 assignment4 assignment6

assignment2: $(CMAKE_BUILD_DIR)/assignment2
	cp $(CMAKE_BUILD_DIR)/assignment2 assignment2

$(CMAKE_BUILD_DIR)/assignment2: $(CMAKE_BUILD_DIR)/Makefile
	$(MAKE) -C $(@D) $(@F)

assignment3: $(CMAKE_BUILD_DIR)/assignment3
	cp $(CMAKE_BUILD_DIR)/assignment3 assignment3

$(CMAKE_BUILD_DIR)/assignment3: $(CMAKE_BUILD_DIR)/Makefile
	$(MAKE) -C $(@D) $(@F)

assignment4: $(CMAKE_BUILD_DIR)/assignment4
	cp $(CMAKE_BUILD_DIR)/assignment4 assignment4

$(CMAKE_BUILD_DIR)/assignment4: $(CMAKE_BUILD_DIR)/Makefile
	$(MAKE) -C $(@D) $(@F)

assignment6: $(CMAKE_BUILD_DIR)/assignment6
	cp $(CMAKE_BUILD_DIR)/assignment6 assignment6

$(CMAKE_BUILD_DIR)/assignment6: $(CMAKE_BUILD_DIR)/Makefile
	$(MAKE) -C $(@D) $(@F)

$(CMAKE_BUILD_DIR)/Makefile: $(CMAKE_SOURCE_DIR)/CMakeLists.txt
	cmake -S $(<D) -B $(@D)

clean:
	rm -rf build
	rm -f nbh_assignment.zip
	rm -f assignment*

zip: nbh_assignment.zip 

nbh_assignment.zip:
	zip -r nbh_assignment.zip . -x ".cache/*" -x .clang* -x ".git/*" -x assignment* -x "compile_commands.json" -x .gitignore -x .gitmodules -x "deps/imgui/.git/*" -x "deps/imgui/.github/*"
	

.PHONY: all clean zip
