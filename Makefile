cproxy: src/cproxy.h src/cproxy.c src/consumer.c src/producer.c
	gcc src/cproxy.c src/consumer.c src/producer.c -o cproxy