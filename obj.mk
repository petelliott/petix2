%.s.o: %.s
	$(CC) $(CFLAGS) -c $^ -o $@

%.c.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@
