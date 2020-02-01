
DIRSCLEAN=$(addsuffix .clean,$(DIRS))

subdir: $(DIRS)

subdir_clean:
	for x in $(DIRS); do make -C $$x clean; done;

$(DIRS):
	make -C $@

$(DIRSCLEAN):
	make -C $@ clean

.PHONY: subdir subdir_clean $(DIRS) $(DIRSCLEAN)
