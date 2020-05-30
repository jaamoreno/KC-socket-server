# ---------------------------- (1)
FROM ubuntu:bionic AS step1

RUN apt-get update && \
    apt-get install -y make && \
    apt-get install -y gcc && \
    apt-get install -y git && \
    apt-get install -y libmysqlclient-dev

WORKDIR /tmp
RUN git clone https://github.com/jaamoreno/KC-socket-server.git myapp
WORKDIR /tmp/myapp
RUN sh -x makeall.sh

# ---------------------------- (2)
FROM ubuntu:bionic AS step2

RUN apt-get update && \
    apt-get install -y curl && \
    apt-get install -y mysql-client && \
    apt-get install -y libmysqlclient-dev

RUN mkdir /supervisor
COPY --from=step1 /tmp/myapp/supervisor/supervisor /supervisor
COPY --from=step1 /tmp/myapp/initDB.sql /supervisor

EXPOSE $SUPERVISOR_LISTENPORT

ENTRYPOINT /supervisor/supervisor
