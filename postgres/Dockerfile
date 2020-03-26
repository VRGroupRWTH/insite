FROM postgres:12.2-alpine

#run --name blub -e POSTGRES_PASSWORD=mysecretpassword -d postgres

COPY *.sql /docker-entrypoint-initdb.d/

ENV POSTGRES_PASSWORD=docker

EXPOSE 5432
