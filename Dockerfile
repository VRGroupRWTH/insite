FROM postgres:12.2-alpine

COPY *.sql /docker-entrypoint-initdb.d/

ENV POSTGRES_PASSWORD=postgres

RUN echo "listen_addresses='0.0.0.0'" >> /usr/local/share/postgresql/postgresql.conf.sample

EXPOSE 5432
