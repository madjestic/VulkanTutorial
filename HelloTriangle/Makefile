# Build Directory
BUILD_DIR = build

# if ./build exists, clean it up and run the build process -> generate image
# else, create ./build and run ^
out:
	if [ -e "$(BUILD_DIR)/" ] ; \
	then \
			rm -rf ./build/* ; \
			cd build &&	cmake .. && make ; \
	else \
			mkdir ./build ; \
			cd build &&	cmake .. && make ; \
	fi ;

clean:
	@rm -rf ./build/* ;	

tags:
	cd ./build &&	rdm &
	cd ./build &&	rc -J .
