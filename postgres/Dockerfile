FROM postgres:12.2-alpine

COPY *.sql /docker-entrypoint-initdb.d/

ENV POSTGRES_PASSWORD=postgres

EXPOSE 5432
